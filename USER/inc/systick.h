#ifndef __SYSTICK
#define __SYSTICK
#include "sys.h"

void systick_ms(u32 ms);
void systick_us(u32 us);
void systick_interrupt_ms(u32 ms);
void systick_interrupt_us(u32 us);
#endif

