#include "Display.h"
#include "ST7735.h"
#include "fixed.h"
#define BOTTOM_Y 153
#define RIGHT_X 126
#define LABEL_SPACE 15
#define GRAPHIC_SCALAR_DENOM 500
#define NUM_LABELS 13
#define BOTTOM_ROW 15
#define LABEL_SCALAR 5
uint32_t myDesiredRps = 0;
void Display_Init(){
	Output_Init();
	// set up static labels
	printf("Desired: \n");
	printf("Current: \n");
}
void Display_drawScreen(uint32_t currentRpsThousandths, uint32_t desiredRpsThousandths){
	static uint16_t desiredY[RIGHT_X + 1];
	static uint16_t currentY[RIGHT_X + 1];
	int i;
	ST7735_SetCursor(10, 0);
	Fixed_uDecOut3(desiredRpsThousandths);
	ST7735_SetCursor(10, 1);
	Fixed_uDecOut3(currentRpsThousandths);
	for (i = LABEL_SPACE; i < RIGHT_X; ++i) {
		// lazy way to initialize the array. Yep. I'm lazy.
		if (desiredY[i+1] == 0)
			desiredY[i+1] = BOTTOM_Y;
		if (currentY[i+1] == 0)
			currentY[i+1] = BOTTOM_Y;
		
		// clear previous timestep
		ST7735_DrawPixel(i, desiredY[i], ST7735_BLACK);
		ST7735_DrawPixel(i, currentY[i], ST7735_BLACK);
		
		// move up one timestep
		desiredY[i] = desiredY[i+1];
		currentY[i] = currentY[i+1];
		
		// draw new timestep
		ST7735_DrawPixel(i, BOTTOM_Y, ST7735_YELLOW);
		ST7735_DrawPixel(i, desiredY[i], ST7735_BLUE);
		ST7735_DrawPixel(i, currentY[i], ST7735_RED);
	}
	
	// and finally the NEWEST point - clear, load, and redraw
	ST7735_DrawPixel(i, desiredY[RIGHT_X], ST7735_BLACK);
	ST7735_DrawPixel(i, currentY[RIGHT_X], ST7735_BLACK);
	desiredY[RIGHT_X] = BOTTOM_Y - desiredRpsThousandths/GRAPHIC_SCALAR_DENOM;
	currentY[RIGHT_X] = BOTTOM_Y - currentRpsThousandths/GRAPHIC_SCALAR_DENOM;
	ST7735_DrawPixel(RIGHT_X, BOTTOM_Y, ST7735_YELLOW);
	ST7735_DrawPixel(RIGHT_X, desiredY[RIGHT_X], ST7735_BLUE);
	ST7735_DrawPixel(RIGHT_X, currentY[RIGHT_X], ST7735_RED);
	
	// print y axis labels
	for (i = 0; i < NUM_LABELS; ++i){
		ST7735_SetCursor(0, BOTTOM_ROW-i);
		printf("%d", i*LABEL_SCALAR);
	}
}

void Display_updateDesiredSpeed(uint32_t desiredRps) {
	myDesiredRps = desiredRps;
}