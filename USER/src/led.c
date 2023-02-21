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
	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ�� 
	RCC->AHB1ENR|=1<<0; //ʹ��PORTAʱ��
	GPIO_Set(GPIOA,PIN7,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//���ù���,�������
	GPIO_AF_Set(GPIOA,7,2);	//PC6,AF2 
	
	TIM3->ARR=arr;			//�趨�������Զ���װֵ 
	TIM3->PSC=psc;			//Ԥ��Ƶ������Ƶ
	TIM3->CCMR1|=6<<12;  	//CH2 PWM1ģʽ		 
	TIM3->CCMR1|=1<<11; 	    //CH2Ԥװ��ʹ��	   
	TIM3->CCER|=1<<4;   	//OC2 ���ʹ��	
    TIM3->CCER&=~(1<<5);   	//OC2 �͵�ƽ��Ч	
	TIM3->CR1|=1<<7;   	    //ARPEʹ�� 
	TIM3->CR1|=1<<0;    	//ʹ�ܶ�ʱ��3 
	
}
