#include "tach.h"
#include "inc/tm4c123gh6pm.h"
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
#define CYCLES_PER_SECOND 80000000
#define N 4

uint32_t Period;
uint32_t static first;
int32_t Done;
void Tach_Init(){
	long sr;
  sr = StartCritical();
  SYSCTL_RCGCTIMER_R |= 0x02;// activate timer0
  SYSCTL_RCGCGPIO_R |= 0x02; // activate port B
  while((SYSCTL_PRGPIO_R&0x0002) == 0){};// ready?
	first = 0;
	Done = 0;
		
	// configure PB6 for input capture
  GPIO_PORTB_DIR_R &= ~0x40;       // make PB6 in
  GPIO_PORTB_AFSEL_R |= 0x40;      // enable alt funct on PB6
  GPIO_PORTB_DEN_R |= 0x40;        // enable digital I/O on PB6
                                   // configure PB6 as T0CCP0
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xF0FFFFFF)+0x07000000;
  GPIO_PORTB_AMSEL_R &= ~0x40;     // disable analog functionality on PB6
		
	// configure timer1a
  TIMER1_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER1_CFG_R = TIMER_CFG_16_BIT; // configure for 16-bit timer mode
                                   // configure for capture mode, default down-count settings
  TIMER1_TAMR_R = (TIMER_TAMR_TACMR|TIMER_TAMR_TAMR_CAP);
                                   // configure for rising edge event
  TIMER1_CTL_R &= ~(TIMER_CTL_TAEVENT_POS|0xC);
  TIMER1_TAILR_R = TIMER_TAILR_M;  // max start value
  TIMER1_IMR_R |= TIMER_IMR_CAEIM; // enable capture match interrupt
  TIMER1_ICR_R = TIMER_ICR_CAECINT;// clear timer0A capture match flag
  TIMER1_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 16-b, +edge timing, interrupts
                                   // Timer0A=priority 2
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00000400; // top 3 bits
  NVIC_EN0_R = NVIC_EN0_INT21;     // enable interrupt 21 in NVIC
  EndCritical(sr);
	
}
uint32_t Tach_getMeasurement(){
	while (!Done){
		WaitForInterrupt();
	}
	uint32_t rps = CYCLES_PER_SECOND/(N*Period);
	Done = 0;
	return rps;
}

void Timer1A_Handler(void){
	TIMER1_ICR_R = 0x00000004;
	Period = (first - TIMER1_TAR_R)&0x00FFFFFF;
	first = TIMER1_TAR_R;
	Done = 1;
}