#ifndef _LEDS_H_
#define _LEDS_H_
#include <stdint.h>
void LEDs_Init(void);
void LEDs_BouncingSpeedUp(void);
void LEDs_BouncingSpeedDown(void);
void LEDs_SetBouncingSpeedTo(int32_t speed);
void LEDs_NotifyOnTimerIsr(void);



#endif