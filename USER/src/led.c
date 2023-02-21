#include "led.h"
#include "sys.h"
void LED_Init(void)
{
	RCC->AHB1ENR 	|=1<<0;
	
	
	GPIOA->MODER 	&=~(0x3<<2*7);
	GPIOA->MODER 	|=0x1<<2*7;
	
	GPIOA->OTYPER 	&=0<<7;
	
	GPIOA->OSPEEDR 	&=~(0x3<<2*7);
	GPIOA->OSPEEDR 	|=0x3<<2*7;	
	PAout(7)=1;
}

void LED_pwm_Init(u16 arr,u16 psc)
{	
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能 
	RCC->AHB1ENR|=1<<0; //使能PORTA时钟
	GPIO_Set(GPIOA,PIN7,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//复用功能,上拉输出
	GPIO_AF_Set(GPIOA,7,2);	//PC6,AF2 
	
	TIM3->ARR=arr;			//设定计数器自动重装值 
	TIM3->PSC=psc;			//预分频器不分频
	TIM3->CCMR1|=6<<12;  	//CH2 PWM1模式		 
	TIM3->CCMR1|=1<<11; 	    //CH2预装载使能	   
	TIM3->CCER|=1<<4;   	//OC2 输出使能	
    TIM3->CCER&=~(1<<5);   	//OC2 低电平有效	
	TIM3->CR1|=1<<7;   	    //ARPE使能 
	TIM3->CR1|=1<<0;    	//使能定时器3 
	
}
