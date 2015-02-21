#include "tach.h"
#include "motor.h"
#include "inc/tm4c123gh6pm.h"
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
#define CYCLES_PER_SECOND 80000000
#define BRUSHES_PER_ROTATION 4
#define TRUE 1
#define FALSE 0

uint32_t Period;
uint32_t static first;
uint32_t rps_thousandths;
int32_t Done;
uint8_t readingReady, periodReady;
void Tach_Init(){
	long sr;
  sr = StartCritical();
  SYSCTL_RCGCTIMER_R |= 0x02;// activate timer0
  SYSCTL_RCGCGPIO_R |= 0x02; // activate port B
  while((SYSCTL_PRGPIO_R&0x0002) == 0){};// ready?
	first = 0;
	readingReady = FALSE;
	// configure PB6 for input capture
  GPIO_PORTB_DIR_R &= ~0x40;       // make PB6 in
  GPIO_PORTB_AFSEL_R |= 0x40;      // enable alt funct on PB6
  GPIO_PORTB_DEN_R |= 0x40;        // enable digital I/O on PB6
                                   // configure PB6 as T0CCP0
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xF0FFFFFF)+0x07000000;
  GPIO_PORTB_AMSEL_R &= ~0x40;     // disable analog functionality on PB6
		
	// configure timer1a
  SYSCTL_RCGCTIMER_R |= 0x01;      // 0) activate timer0
	
	
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = TIMER_CFG_16_BIT; // configure for 16-bit timer mode
                                   // configure for capture mode, default down-count settings
  TIMER0_TAMR_R = (TIMER_TAMR_TACMR|TIMER_TAMR_TAMR_CAP);
                                   // configure for rising edge event
  TIMER0_CTL_R &= ~(TIMER_CTL_TAEVENT_POS|0xC);
  TIMER0_TAILR_R = TIMER_TAILR_M;  // max start value
  TIMER0_IMR_R |= TIMER_IMR_CAEIM; // enable capture match interrupt
	TIMER0_TAPR_R = 0xFF;
  TIMER0_ICR_R = TIMER_ICR_CAECINT;// clear timer0A capture match flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 16-b, +edge timing, interrupts
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x400000000; // top 3 bits
  NVIC_EN0_R = NVIC_EN0_INT19;     // enable interrupt 21 in NVIC
  EndCritical(sr);
	
}
uint32_t Tach_getMeasurementThousandths(){
	// demote to 32 bit when passed back.
	while (!readingReady){
		// non busy wait--wake up to check afater interrupts
		return 0;
		WaitForInterrupt();
	}
	long sr = StartCritical();
	uint32_t tmp = rps_thousandths;
	readingReady = FALSE;
	EndCritical(sr);
	return tmp;
}

uint32_t Tach_getPeriod(){
	while (!periodReady){
		// non busy wait--wake up to check after interrupts
		WaitForInterrupt();
	}
	periodReady = FALSE;
	long sr = StartCritical();
	uint32_t tmp = Period;
	EndCritical(sr);
	return tmp;
	
}

void Timer0A_Handler(void){
	TIMER0_ICR_R = 0x00000004;
	Period = (first - TIMER0_TAR_R)&0x00FFFFFF;
	first = TIMER0_TAR_R;
	periodReady = TRUE;
	
	// calculate using a 64 bit to accomodate 1000 * 80,000,000
	rps_thousandths = ( 1000 * (uint64_t) CYCLES_PER_SECOND)/(BRUSHES_PER_ROTATION * Period);
	
	// set the flag
	readingReady = TRUE;
	
}