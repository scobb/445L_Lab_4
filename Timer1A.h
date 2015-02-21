#ifdef _TIMER_1A_
#define _TIMER_1A_

void Timer1A_Init(uint32_t period);
void setHeartbeatTask(void(*task)(void));
#endif