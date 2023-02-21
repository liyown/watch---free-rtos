#include "exit.h"
#include "key.h"
#include "delay.h"
#include "motor.h"
#include "led.h"
int speed[]= {900,100,1100};
int speedindex = 0;
u8 flagOELD=1; //模式切换
void exit_init(void)
{
	key_init();//初始化按键对应io模式
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_AF,0,0,GPIO_PUPD_PD); 
	Ex_NVIC_Config(GPIO_A,0,RTIR);		//上升沿触发
	MY_NVIC_Init(5,0,EXTI0_IRQn,4);    	//抢占2，子优先级2，组2
}
int flagPWM = 0;
//外部中断0服务程序
void EXTI0_IRQHandler(void)
{
    delay_ms(10);    //消抖
	if(WK_UP==1)     //KEY_UP按键
	{	

	}
	EXTI->PR=1<<0;  //清除LINE0上的中断标志位
}

