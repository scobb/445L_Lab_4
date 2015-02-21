#include "inc/tm4c123gh6pm.h"
#include "PWM.h"
#include "tach.h"
#include "Display.h"
#include "motor.h"
#define CYCLES_PER_RPS 40
#define FIVE_RPS 200
#define TEN_RPS 400
#define TWENTY_RPS 800
#define ONE_THOUSAND_HZ 40000
#define CYCLES_PER_PERCENT 40000
#define CYCLES_PER_SECOND 80000000
#define RPS_THOUSANDTHS_MAX 60000
#define RPS_THOUSANDTHS_INCREMENT 5000

uint32_t desiredRpsThousandths;
uint16_t PWM_Frequency;
uint16_t dutyCycle;
void Motor_Init() {
	desiredRpsThousandths = 20000;//initialize to 20 rps (from ppt: desiredSpeed*0.025=rps
	PWM_Frequency = ONE_THOUSAND_HZ;//1000 HZ
	dutyCycle = CYCLES_PER_PERCENT / 2;//50% duty
	PWM0B_Init(PWM_Frequency, dutyCycle);//Initialize PWM with these parameters
}

uint32_t Motor_getDesiredRpsThousandths() {
	return desiredRpsThousandths;
}

void Motor_decreaseSpeed(){
	//This either means decrease speed/dutyCycle or just dutyCycle. I will start with just dutyCycle
	if (desiredRpsThousandths >= 5000) { desiredRpsThousandths-= 5000; }
}

void Motor_increaseSpeed() {
	//We will only change speed when necessary (doing 200 using formula for desiredSpeed in ppt)
	if (desiredRpsThousandths <= 35000) { desiredRpsThousandths+= 5000; }
}

void Motor_off() {
	PWM0B_Duty(0);
}

void Motor_updateOutput(int32_t measuredRpsThousandths) {
	//code from the book. Is this what we want here?
	static int32_t U = 0;
	int32_t E = desiredRpsThousandths - measuredRpsThousandths;
	//printf("des: %u\n", desiredRpsThousandths);
	//printf("meas: %u\n", measuredRpsThousandths);
	
	
	U += (3*E)/64;//3 may need to be changed
	if (U < 40) U = 40;
	else if (U > 39960) U = 39960;
	PWM0B_Duty(U);
}