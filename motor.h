#include "inc/tm4c123gh6pm.h"
#include "PWM.h"
#include "tach.h"
#include "Display.h"

void Motor_Init();

uint32_t Motor_getDesiredRps();

void Motor_decreaseSpeed();

void Motor_increaseSpeed();

void Motor_off();

void Motor_updateOutput(uint32_t measuredRpsThousands);