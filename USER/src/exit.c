#include "exit.h"
#include "key.h"
#include "delay.h"
#include "motor.h"
#include "led.h"
int speed[]= {900,100,1100};
int speedindex = 0;
u8 flagOELD=1; //ģʽ�л�
void exit_init(void)
{
	key_init();//��ʼ��������Ӧioģʽ
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_AF,0,0,GPIO_PUPD_PD); 
	Ex_NVIC_Config(GPIO_A,0,RTIR);		//�����ش���
	MY_NVIC_Init(5,0,EXTI0_IRQn,4);    	//��ռ2�������ȼ�2����2
}
int flagPWM = 0;
//�ⲿ�ж�0�������
void EXTI0_IRQHandler(void)
{
    delay_ms(10);    //����
	if(WK_UP==1)     //KEY_UP����
	{	

	}
	EXTI->PR=1<<0;  //���LINE0�ϵ��жϱ�־λ
}

