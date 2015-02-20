#include "inc/tm4c123gh6pm.h"
#include "PWM.h"
#include "tach.h"
#include "Display.h"
#include "motor.h"

#define STEP 5000
uint32_t Period;
uint32_t desiredSpeed;
uint16_t speed;
uint16_t dutyCycle;
void Motor_Init() {
	desiredSpeed = 0;//initialize to 0 rps
	speed = 40000;//1000 HZ
	dutyCycle = 30000;//75% duty
	PWM0A_Init(speed, dutyCycle);//From sample code
}

uint32_t Motor_getDesiredRps() {
	//This might just be returning a variable that has the current speed
	return Tach_getMeasurement();
}

void Motor_decreaseSpeed(){
	//This either means decrease speed/dutyCycle or just dutyCycle. I will start with just dutyCycle
	desiredSpeed = desiredSpeed - 5;
}

void Motor_increaseSpeed() {
	//We will only change speed when necessary
	desiredSpeed = desiredSpeed + 5;
}

void Motor_off() {
	PWM0A_Duty(0);
}

void Motor_updateOutput(uint32_t measuredRpsThousands) {
	//code from the book. Is this what we want here?
	uint32_t motorSpeed = 800000000/Period;
	int32_t E = desiredSpeed-motorSpeed;
	int32_t U = U+(3*E)/64;//3 may need to be changed
	if (U < 40) U = 40;
	else if (U > 39960) U = 39960;
	PWM0A_Duty(U);
}