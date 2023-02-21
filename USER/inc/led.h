#ifndef __LED
#define __LED
#include "sys.h" 
#define	LED_PWM_VAL TIM3->CCR2
void LED_Init(void);
void LED_pwm_Init(u16 arr,u16 psc);
#endif

