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
	if (desiredRpsThousandths > 10000) { desiredRpsThousandths-= 5000; }
	else if (desiredRpsThousandths == 10000)
		// can't sustain 5 rps. Don't get enough current to the motor
		desiredRpsThousandths = 0;
}

void Motor_increaseSpeed() {
	//We will only change speed when necessary (doing 200 using formula for desiredSpeed in ppt)
	if (desiredRpsThousandths == 0)
		desiredRpsThousandths = 10000; 
	else if (desiredRpsThousandths <= 55000) { desiredRpsThousandths+= 5000; }
}

void Motor_off() {
	PWM0B_Duty(0);
}

void Motor_updateOutput(int32_t measuredRpsThousandths) {
	// Integral control
	static int32_t newOutput = 0;
	int32_t E = desiredRpsThousandths - measuredRpsThousandths;
	
	// Control equation for new output
	newOutput += (3*E)/64;
	
	// bound our output
	if (newOutput < 40) newOutput = 40;
	else if (newOutput > 39960) newOutput = 39960;
	if (desiredRpsThousandths == 0){
		newOutput = 0;
	}
	
	// output new duty cycle
	PWM0B_Duty(newOutput);
}