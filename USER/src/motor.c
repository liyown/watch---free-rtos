#include "motor.h"

void MOTOR_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<0;	//TIM2时钟使能 
	RCC->AHB1ENR|=1<<1; //使能PORTB时钟
	
	
	GPIO_Set(GPIOB,PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//复用功能,上拉输出
	GPIO_AF_Set(GPIOB,10,1);	//PB10,AF1

	
	
	TIM2->ARR=arr;			//设定计数器自动重装值 
	TIM2->PSC=psc;			//预分频器不分频
	TIM2->CCMR2|=6<<4;  	//CH3 PWM1模式		 
	TIM2->CCMR2|=1<<3; 	    //CH3预装载使能	   
	TIM2->CCER|=1<<8;   	//OC3 输出使能	
    TIM2->CCER|=~(1<<9);   	//OC3 低电平有效	
	TIM2->CR1|=1<<7;   	    //ARPE使能 
	TIM2->CR1|=1<<0;    	//使能定时器2 	
}
