#ifndef _TACH_H_
#define _TACH_H_
#include <stdint.h>
void Tach_Init();
uint32_t Tach_getMeasurementThousandths();
uint32_t Tach_getPeriod();


#endif