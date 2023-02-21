#include "led.h"
#include "sys.h"
#include "delay.h"
#include "motor.h"
#include "key.h"
#include "exit.h"
#include "usart.h"	
#include "string.h"	
#include "rtc.h"
#include "spi.h"
#include "i2c.h"
#include "oled.h"
#include "sht20.h"
#include "hp6.h"

#include "FreeRTOS.h"
#include "task.h"


extern u8 flagOELD; //模式切换


//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define STATE_PRIO		3
//任务堆栈大小	
#define STATE_SIZE 		50 
//任务句柄
TaskHandle_t STATE_Handler;
//任务函数
void state(void *pvParameters);

//任务优先级
#define OUTPUT_PRIO		2
//任务堆栈大小	
#define OUTPUT_SIZE 		300  
//任务句柄
TaskHandle_t OUTPUT_Handler;
//任务函数
void output(void *pvParameters);

//任务优先级
#define DATA_PRIO		4
//任务堆栈大小	
#define DATA_SIZE 		300  
//任务句柄
TaskHandle_t DATA_Handler;
//任务函数
void data(void *pvParameters);

u8 cmdstrcmp(u8 *str1,u8 *str2)
{
	while(1)
	{
		if(*str1!=*str2)return 1;//不相等
		if(*str1=='\0')break;//对比完成了.
		str1++;
		str2++;
	}
	return 0;//两个字符串相等
}

struct FLOAT
{
	float temp,humi;
}th;
struct TIME
{
	u8  hour,min,sec,ampm;
}time;
struct DATE
{
	u8 year,month,date,week;
}date;
u8 uiBuf[40];
u8 showflag = 0;
int main()
{
	NVIC_SetPriorityGrouping(__NVIC_PRIO_BITS);//设置系统中断优先级分组4	
	delay_init();
	uart_init(100,115200);      //串口初始化 
	key_init();
	
	MOTOR_PWM_VAL = 0;
	LED_PWM_VAL = 0;
		  
	LED_Init();		  			//初始化LED
	OLED_Init();
	MOTOR_Init(1200-1,0);		//80khz
	SHT2x_Init();
	Hp_6_init();
	while(RTC_Init());		   //RTC初始化，一定要初始化成功	
	RTC_Set_WakeUp(4,0);		//配置WAKE UP中断,1秒钟中断一次 
	RTC_Set_Time(6,9,0,0);
	RTC_Set_Date(21,7,23,5);
	OLED_ShowChinese(0,5,0,16,1);
	OLED_ShowChinese(18,5,1,16,1);
	OLED_ShowChinese(36,5,2,16,1);
	OLED_ShowChinese(78,5,28,16,1);
	OLED_ShowChinese(96,5,29,16,1);
	OLED_ShowChinese(114,5,30,16,1);
	OLED_ShowChinese(48,40,31,16,1);
	OLED_ShowChinese(64,40,32,16,1);
	OLED_Refresh();
	delay_ms(1500);
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建状态切换模块任务
    xTaskCreate((TaskFunction_t )state,     	
                (const char*    )"state",   	
                (uint16_t       )STATE_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )STATE_PRIO,	
                (TaskHandle_t*  )&STATE_Handler);  
	//创建数据获取模块任务			
    xTaskCreate((TaskFunction_t )data,     	
                (const char*    )"led0_task",   	
                (uint16_t       )DATA_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )DATA_PRIO,	
                (TaskHandle_t*  )&DATA_Handler); 
    //创建OLED数据显示模块
    xTaskCreate((TaskFunction_t )output,     
                (const char*    )"led1_task",   
                (uint16_t       )OUTPUT_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )OUTPUT_PRIO,
                (TaskHandle_t*  )&OUTPUT_Handler);         
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//创建状态切换模块任务 
void state(void *pvParameters)
{
	u8 key;
    while(1)
    {
		key=KEY_Scan(0);
		if(key==4)
		{
			PAout(7)=!PAout(7);
			if(flagOELD==4)flagOELD=1;
			else flagOELD++;
			if(flagOELD==3)
			{
				HP_6_OpenRate(); 
			}
			if(flagOELD!=4&flagOELD!=3)
			{
				HP_6_CloseBp();	 
			}
			if(flagOELD==4)
			{
				HP_6_OpenBp();
			}
		}
		if(((time.hour)+12*time.ampm)==6&&time.min==10&&time.sec<=5)
		{
			MOTOR_PWM_VAL = 1000;
			PAout(7)=0;
		}
		else
		{
			MOTOR_PWM_VAL = 0;
			PAout(7)=1;
		}
        vTaskDelay(20);
    }
}   
u8 Bpflag = 1;
////创建数据获取模块任务
void data(void *pvParameters)
{
    u8 pulseBuf[24] = {0};
	u8 BpBuf[24] = {0};
	static u8 oldPulseVal = 0;   //上一次的心率值
    while(1)
    {
		if(flagOELD==1)
		{
			RTC_Get_Time(&time.hour,&time.min,&time.sec,&time.ampm);
			RTC_Get_Date(&date.year,&date.month,&date.date,&date.week);
		}
		else if(flagOELD==2)
		{

			th.temp=SHT2x_GetTempPoll();
			th.humi=SHT2x_GetHumiPoll();
		}
		else if(flagOELD==3)
		{ 
			if(pulseBuf[7] && pulseBuf[7]!= oldPulseVal)  //如果可以获得心率结果，表明测量成功，则关闭心率测量
			{
				showflag = 1;
				oldPulseVal = pulseBuf[7];
				HP_6_CloseRate();
				sprintf((char*)uiBuf,"%03d",pulseBuf[7]); // //byte 7是心率结果
			}
			else //如果获取不到心率结果，则每隔100毫秒钟获取心率值
			{
				HP_6_GetRateResult();
				HP_6_GetResultData(pulseBuf);
				vTaskDelay(100);
			}	
		}
		else if(flagOELD==4)
		{ 
			HP_6_GetBpResult();                //获取血压测量状态
			HP_6_GetResultData(BpBuf);
			if(0 == BpBuf[7])                   //byte 7是血压测量的状态:0测量中，1测量完成、2测量失败
				Bpflag=1;
			else if(1 == BpBuf[7])  
			{
				Bpflag=2;
				sprintf((char*)uiBuf,"%03d/%03d MMGH", BpBuf[10], BpBuf[11]);//byte 10是血压测量结果的高压,byte 11是血压测量结果的低压	
				HP_6_CloseBp();	 
			}		 
			else if(2 == BpBuf[7])         
			{
				Bpflag=3;
				HP_6_CloseBp();	 
			}		 	 
			else delay_ms(100);
		}
        vTaskDelay(500);
    }
}

////创建OLED数据显示模块
void output(void *pvParameters)
{
	OLED_Clear();
	u8 data0[15],data1[15];
    while(1)
    {
		if(flagOELD==1)
		{
			OLED_Clear();
			if(time.ampm ==0 )sprintf((char *)data0,"%d:%d:%d,AM\n",time.hour,time.min,time.sec);
			
			if(time.ampm ==1 )sprintf((char *)data0,"%d:%d:%d,PM\n",time.hour,time.min,time.sec);
		
			sprintf((char *)data1,"20%d:%d:%d\n",date.year,date.month,date.date);
			OLED_ShowString(30,5,data0,16,1);
			OLED_ShowString(30,25,data1,16,1);
			switch(date.week)
			{
				case 0:OLED_ShowString(32,45,"Sunday",16,1);break;
				case 1:OLED_ShowString(32,45,"Monday",16,1);break;
				case 2:OLED_ShowString(32,45,"Tuesday",16,1);break;
				case 3:OLED_ShowString(32,45,"Wednesday",16,1);break;
				case 4:OLED_ShowString(32,45,"Thursday",16,1);break;
				case 5:OLED_ShowString(32,45,"Friday",16,1);break;
				case 6:OLED_ShowString(32,45,"Saturday",16,1);break;
			}
			
			OLED_Refresh();
		}
		else if(flagOELD==3)
		{
			if(showflag)
			{
				OLED_Clear();
				OLED_ShowString(40,4,"pulse",16,1);
				OLED_ShowString(45,32,uiBuf,16,1);
				OLED_Refresh();
			}
			else 
			{
				OLED_Clear();
				OLED_ShowString(40,4,"pulse",16,1);
				OLED_ShowString(20,32,"Measuring...",16,1);
				OLED_Refresh();				
			}
		}
		else if(flagOELD==2)
		{
			OLED_Clear();
			sprintf((char *)data0,"temp: %.2f",th.temp);
			sprintf((char *)data1,"humi: %.2f",th.humi);
			OLED_ShowString(20,10,data0,16,1);
			OLED_ShowString(20,32,data1,16,1);
			OLED_Refresh();
		}	
		else if(flagOELD==4)
		{
			OLED_Clear();
			if(Bpflag==1)
			{
				OLED_ShowChinese(40,5,18,16,1);
				OLED_ShowChinese(70,5,19,16,1);
				OLED_ShowChinese(14,38,15,16,1);
				OLED_ShowChinese(58,38,16,16,1);
				OLED_ShowChinese(93,38,17,16,1);
			}
			else if(Bpflag==2)
			{
				OLED_ShowChinese(24,5,20,16,1);
				OLED_ShowChinese(40,5,21,16,1);
				OLED_ShowChinese(56,5,24,16,1);
				OLED_ShowChinese(72,5,25,16,1);
				OLED_ShowString(20,40,uiBuf,16,1);
			}
			else if(Bpflag==3)
			{
				OLED_ShowChinese(30,30,20,16,1);
				OLED_ShowChinese(46,30,21,16,1);
				OLED_ShowChinese(62,30,22,16,1);
				OLED_ShowChinese(78,30,23,16,1);
			}
			OLED_Refresh();
		}		
        vTaskDelay(500);
    }
}

