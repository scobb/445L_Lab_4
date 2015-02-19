#include "ButtonManager.h"
#include "inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "SysTick.h"
#include <stdint.h>

long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
#define PD0       (*((volatile uint32_t *)0x40007004))
#define PD1       (*((volatile uint32_t *)0x40007008))
#define PD2       (*((volatile uint32_t *)0x40007010))
#define PD3       (*((volatile uint32_t *)0x40007020))
#define PD6       (*((volatile uint32_t *)0x40007100))
#define TRUE 1
#define FALSE 0
typedef struct {
  uint32_t readValue;
	uint8_t isLow;
	void(*handler)(void);
} buttonStatus;

void ButtonManager_Init(){
	volatile uint32_t delay;
	
  SYSCTL_RCGCGPIO_R |= 0x00000008;  // 1) activate clock for Port D
  delay = SYSCTL_RCGCGPIO_R;        // allow time for clock to start
  // GPIO_PORTD_LOCK_R = GPIO_LOCK_KEY;   // Not using PD7 so don't need to unlock
  // GPIO_PORTD_CR_R |= 0x03;           // Don't need to unlock PD7
  GPIO_PORTD_AFSEL_R &= ~0x03;        // 6) disable alt funct on PD1-0
  GPIO_PORTD_DIR_R &= ~0x03;          // 5) PD1-0 are in
  GPIO_PORTD_AMSEL_R &= ~0x03;        // 3) disable analog on PD
  GPIO_PORTD_PUR_R |= 0x03;          // enable pull-up on PD
  GPIO_PORTD_DEN_R |= 0x03;          // 7) enable digital I/O on PD
  GPIO_PORTD_PCTL_R &= ~0x0F00FFFF; // configure PD1-0 as GPIO
	
  GPIO_PORTD_IS_R &= ~0x03;     // (d) PD0, 1 is edge-sensitive
  GPIO_PORTD_IBE_R &= ~0x03;    //     PD0, 1 is not both edges
  GPIO_PORTD_IEV_R &= ~0x03;    //     PD0, 1 falling edge event
  GPIO_PORTD_ICR_R = 0x03;      // (e) clear flag4
  GPIO_PORTD_IM_R |= 0x03;      // (f) arm interrupt on PD1-0 *** No IME bit as mentioned in Book ***
  NVIC_PRI0_R = (NVIC_PRI0_R&0x0FFFFFFF)|0x80000000; // (g) priority 4
  NVIC_EN0_R |= NVIC_EN0_INT3;      // (h) enable interrupt 19 in NVIC
	
	
}
void increaseSpeedPressed(){
	// Calls int newSpeed = Motor_increaseSpeed();
	
	// Calls Display_updateDesiredSpeed(newSpeed);
	
}
void decreaseSpeedPressed(){
	// Calls int newSpeed = Motor_increaseSpeed();
	
	// Calls Display_updateDesiredSpeed(newSpeed);
	
}
void ButtonManager_setHandler(uint32_t portAddress, void(*handler)(void)){
	// need to store these handlers somehow. Would be easiest to use a hashtable with portAddress as key
	buttonStatus bs = {(*(volatile uint32_t*)portAddress), FALSE, handler};
}
void CheckDebounce(buttonStatus* buttons, uint8_t numPorts){
	// private function to allow us to debounce all buttons
	uint8_t i;
	SysTick_Wait10ms(40);
	for (i=0; i < numPorts; ++i){
		// if a button was low before and is still low, call its handler
		if (buttons[i].isLow && buttons[i].readValue == 0){
			buttons[i].handler();
		}
	}
}

void GPIOPortD_Handler(void){
	// handler for port D -- all 5 buttons
	GPIO_PORTD_ICR_R = 0x03;      // acknowledge flag 0-1
	uint8_t i;
	uint8_t needCheck = FALSE;
	
	// TODO - store these in a "member" hash table updated by setHandler method
	buttonStatus ports[2] = {
		{PD0, FALSE, &increaseSpeedPressed},
		{PD1, FALSE ,&decreaseSpeedPressed}
	};
	
	// check all ports to see if any is low
	for (i=0; i < 2; i++){
		if (ports[i].readValue == 0){
			ports[i].isLow = TRUE;
			needCheck = TRUE;
		}
	}
	if (needCheck)
		CheckDebounce(&ports[0], 2);
}

