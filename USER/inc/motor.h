#ifndef __MOTOR
#define __MOTOR
#include "sys.h"
#define MOTOR_PWM_VAL TIM2->CCR3 
void MOTOR_Init(u16 arr,u16 psc);
#endif

