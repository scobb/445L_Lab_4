#include "Display.h"
#include "ST7735.h"
#include "fixed.h"

void Display_drawScreen(uint32_t currentRps, uint32_t desiredRps){
	ST7735_SetCursor(0, 0);
	Fixed_uDecOut2(desiredRps);
	Fixed_uDecOut2(currentRps);
	
}