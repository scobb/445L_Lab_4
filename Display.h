#ifndef _DISPLAY_H_
#define _DISPLAY_H_
#include <stdint.h>

void Display_drawScreen(uint32_t currentRps, uint32_t desiredRps);
void Display_updateDesiredSpeed(uint32_t desiredRps);
void Display_Init();


#endif