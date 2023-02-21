#include "sht20.h"
#include "delay.h"
#include "usart.h"
#include <stdio.h>

SHT2x_data SHT20; 
//初始化IIC
void IIC_Init(void)
{					     

	RCC->AHB1ENR|=1<<1;    //使能PORTB时钟
	GPIO_Set(GPIOB,PIN8|PIN9,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PB8/PB9设置 
	IIC_SCL=1;
	IIC_SDA=1;
	
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();//SDA设置为输入  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			printf("IIC ACK ERR");
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
int IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	
	return IIC_Wait_Ack();
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}

//函数主体
u8 SHT2x_Init(void)
{
	u8 err;
    IIC_Init();
    err = SHT2x_SoftReset();
	return err;
}

u8 SHT2x_SoftReset(void) //SHT20软件复位
{
	u8 err=0;
	IIC_Start();
    err =IIC_Send_Byte(0x80);
	err =IIC_Send_Byte(0xFE);
	IIC_Stop();
	return err;
}
float TEMP;
float HUMI;

float SHT2x_GetTempPoll(void)
{
    
    u8 ack, tmp1, tmp2;
    u16 ST;
    u16 i=0;
    IIC_Start();				//发送IIC开始信号
    ack =IIC_Send_Byte(I2C_ADR_W);			//IIC发送一个字节 
	ack =IIC_Send_Byte(TRIG_TEMP_MEASUREMENT_POLL);
    do {
        delay_ms(100);               
        IIC_Start();				//发送IIC开始信号
	    ack=IIC_Send_Byte(I2C_ADR_R);	
		i++;
		if(i==1000)break;
    } while(ack!=0);
    tmp1 = IIC_Read_Byte(1);
    tmp2 = IIC_Read_Byte(1);
		   IIC_Read_Byte(0);
    IIC_Stop();
    
    ST = (tmp1 << 8) | (tmp2 << 0);
	
    ST &= ~0x0003;
    TEMP = ((float)ST * 0.00268127f) - 46.85f;
	printf("%6.2f \r\n",TEMP);
    return (TEMP);	  
}

float SHT2x_GetHumiPoll(void)
{
    
    u8 ack, tmp1, tmp2;
    u16 SRH;
	u16 i=0;
    IIC_Start();				//发送IIC开始信号
    ack =IIC_Send_Byte(I2C_ADR_W);			//IIC发送一个字节 	
	ack =IIC_Send_Byte(TRIG_HUMI_MEASUREMENT_POLL);    
    do {
        delay_ms(100);               
        IIC_Start();				//发送IIC开始信号
        ack=IIC_Send_Byte(I2C_ADR_R);	
		i++;
		if(i==100)break;
    } while(ack!=0);
    tmp1 = IIC_Read_Byte(1);
    tmp2 = IIC_Read_Byte(1);
    IIC_Read_Byte(0);
    IIC_Stop();
    SRH = (tmp1 << 8) | (tmp2 << 0);
    SRH &= ~0x0003;
    HUMI = ((float)SRH * 0.00190735f) - 6;
	printf("%6.2f \r\n",HUMI);
    return (HUMI);
}



