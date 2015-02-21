#include "Display.h"
#include "ST7735.h"
#include "fixed.h"
uint32_t myDesiredRps = 0;
void Display_drawScreen(uint32_t currentRpsThousandths, uint32_t desiredRpsThousandths){
	ST7735_SetCursor(0, 0);
	Fixed_uDecOut3(desiredRpsThousandths);
	Fixed_uDecOut3(currentRpsThousandths);
}

void Display_updateDesiredSpeed(uint32_t desiredRps) {
	myDesiredRps = desiredRps;
}