#include "Timer1A.h"
#include "inc/tm4c123gh6pm.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

void (*HeartbeatTask)(void);   // user function
void Timer1A_Init(uint32_t period){long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x02;      // 0) activate TIMER1
  // Timer1AFunction = task;             // user function (this line also allows time to finish activating)
  TIMER1_CTL_R &= ~0x00000001;     // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;       // 2) configure for 32-bit timer mode
  TIMER1_TAMR_R = 0x00000002;      // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = period-1;       // 4) reload value
  TIMER1_TAPR_R = 0;               // 5) 12.5ns TIMER1A
  TIMER1_ICR_R = 0x00000001;       // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R |= 0x00000001;      // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF1FFF)|0x00004000; // 8) priority 2
  NVIC_EN0_R |= NVIC_EN0_INT21;     // 9) enable interrupt 19 in NVIC
  TIMER1_CTL_R |= 0x00000001;      // 10) enable TIMER1A
  EndCritical(sr);
}
void setHeartbeatTask(void(*task)(void)){
	HeartbeatTask = task;
}

void Timer1A_Handler(void){
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
	if (HeartbeatTask){
		(*HeartbeatTask)();
	}
}