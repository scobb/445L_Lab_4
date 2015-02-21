// ST7735TestMain.c
// Runs on LM4F120/TM4C123
// Test the functions in ST7735.c by printing basic
// patterns to the LCD.
// Daniel Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground
#include <stdio.h>
#include <stdint.h>
#include "ST7735.h"
#include "PLL.h"
#include "tach.h"
#include "heartbeat.h"
#include "Timer0A.h"
#include "SysTick.h"
#include "inc/tm4c123gh6pm.h"
#include "ButtonManager.h"
#include "Display.h"
#include "Motor.h"
#include "Timer1A.h"

#define HALF_A_SECOND 40000000

void EnableInterrupts(void);  // Enable interrupts

int main(void){
	// 80 MHz
  PLL_Init();
	
	// Heartbeat timer, PF1
	Timer1A_Init(HALF_A_SECOND);
	Heartbeat_Init();
	
	// SysTick, used for debounce
	//SysTick_Init();
	
	// Buttons activated
	ButtonManager_Init();
	
	// LCD output activated
  Output_Init();
	
	// Tachometer sensor activated
	Tach_Init();
	
	Motor_Init();
	
	// Enable interrupts
	EnableInterrupts();
  // printf("hello world");
  while(1){
		uint32_t currentRps = Tach_getMeasurement();
		uint32_t desiredRps = Motor_getDesiredRps();
		Display_drawScreen(currentRps, desiredRps);
  }
} 