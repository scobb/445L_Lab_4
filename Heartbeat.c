#include "Heartbeat.h"
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include "Timer1A.h"

#define LED (*((volatile uint32_t *)0x40025008)) //Uses PF1. Looks like mainly for colors (1=red)

void blinkHeartbeat(){
	LED ^= 0x02;
}

void Heartbeat_Init(){
	// assumes PORTF already initialized
	volatile uint32_t delay;
  SYSCTL_RCGCGPIO_R |= 0x00000020;  // 1) activate clock for Port F
  delay = SYSCTL_RCGCGPIO_R;        // allow time for clock to start
  GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R |= 0x02;           // allow changes to PF4-0
	GPIO_PORTF_DIR_R |= 0x02;        // make PF1 output (PF1 built-in LEDs)
  GPIO_PORTF_AFSEL_R &= ~0x02;     // disable alt funct on PF1
  GPIO_PORTF_DEN_R |= 0x02;        // enable digital I/O on PF1
                                   // configure PF1 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFFF0F)+0x00000000;
  GPIO_PORTF_AMSEL_R &= ~0x02;
	
	LED = 0x00;
	setHeartbeatTask(&blinkHeartbeat);
	
}
