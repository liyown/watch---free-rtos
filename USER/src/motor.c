#include "motor.h"

void MOTOR_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<0;	//TIM2ʱ��ʹ�� 
	RCC->AHB1ENR|=1<<1; //ʹ��PORTBʱ��
	
	
	GPIO_Set(GPIOB,PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//���ù���,�������
	GPIO_AF_Set(GPIOB,10,1);	//PB10,AF1

	
	
	TIM2->ARR=arr;			//�趨�������Զ���װֵ 
	TIM2->PSC=psc;			//Ԥ��Ƶ������Ƶ
	TIM2->CCMR2|=6<<4;  	//CH3 PWM1ģʽ		 
	TIM2->CCMR2|=1<<3; 	    //CH3Ԥװ��ʹ��	   
	TIM2->CCER|=1<<8;   	//OC3 ���ʹ��	
    TIM2->CCER|=~(1<<9);   	//OC3 �͵�ƽ��Ч	
	TIM2->CR1|=1<<7;   	    //ARPEʹ�� 
	TIM2->CR1|=1<<0;    	//ʹ�ܶ�ʱ��2 	
}
