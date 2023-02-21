#include "systick.h"

void systick_ms(u32 ms)
{
	SysTick->CTRL &=~(0x1<<2);
	SysTick->VAL = 0;
	SysTick->LOAD = 100*1000/8*ms;
	SysTick->CTRL |= 1<<0;
	while(!(SysTick->CTRL & 1<<16));
	SysTick->CTRL &=~(1<<0);
	
}

void systick_us(u32 us)
{
	SysTick->CTRL &=~(0x1<<2);
	SysTick->VAL = 0;
	SysTick->LOAD = 100*1000000/8*us;
	SysTick->CTRL |= 1<<0;
	while(!(SysTick->CTRL & 1<<16));
	SysTick->CTRL &=~(1<<0);
	
}


void systick_interrupt_ms(u32 ms)
{
	SysTick->CTRL &=~(0x1<<2);
	SysTick->VAL = 0;
	SysTick->LOAD = 100*1000/8*ms;
	SysTick->CTRL |= 1<<0;
	SysTick->CTRL |= 1<<1;
	
}

void systick_interrupt_us(u32 us)
{
	SysTick->CTRL &=~(0x1<<2);
	SysTick->VAL = 0;
	SysTick->LOAD = 100*1000000/8*us;
	SysTick->CTRL |= 1<<0;
	SysTick->CTRL |= 1<<1;
	
}
void SysTick_Handler(void)
{
	while(SysTick->CTRL & 1<<16);
	PAout(7)=~PAout(7);
}

