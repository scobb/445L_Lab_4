#include "inc/tm4c123gh6pm.h"
#include "PWM.h"
#include "tach.h"
#include "Display.h"
#include "motor.h"

#define STEP 5000
uint16_t speed;
uint16_t dutyCycle;
void Motor_Init() {
	speed = 40000;//1000 HZ
	dutyCycle = 30000;//75% duty
	PWM0A_Init(speed, dutyCycle);//From sample code
}

uint32_t Motor_getDesiredRps() {
	//Need a formula for this part
	return Tach_getMeasurement();
}

void Motor_decreaseSpeed(){
	//This either means decrease speed/dutyCycle or just dutyCycle. I will start with just dutyCycle
	if (dutyCycle-STEP >= 0){
		PWM0A_Duty(dutyCycle-STEP);
	}
}

void Motor_increaseSpeed() {
	//We will only change speed when necessary
	if (dutyCycle+STEP <= speed){
		PWM0A_Duty(dutyCycle+STEP);
	}
}

void Motor_off() {
	PWM0A_Duty(0);
}

void Motor_updateOutput(uint32_t measuredRpsThousands) {
	Display_drawScreen(measuredRpsThousands, Motor_getDesiredRps());
}