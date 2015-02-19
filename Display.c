#include "Display.h"
#include "ST7735.h"
#include "fixed.h"
uint32_t myDesiredRps = 0;
void Display_drawScreen(uint32_t currentRps, uint32_t desiredRps){
	ST7735_SetCursor(0, 0);
	Fixed_uDecOut2(desiredRps);
	Fixed_uDecOut2(currentRps);
}

void Display_updateDesiredSpeed(uint32_t desiredRps) {
	myDesiredRps = desiredRps;
}