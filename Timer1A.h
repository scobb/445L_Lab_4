#ifdef _TIMER_1A_
#define _TIMER_1A_

void Timer1A_Init(void(*task)(void), uint32_t period);
void setTimer1AHandler(void(*task)(void));
#endif