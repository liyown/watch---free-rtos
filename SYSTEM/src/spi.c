#include "spi.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK NANO STM32F4开发板
//SPI驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2019/4/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2019-2029
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////    
 
//以下是SPI模块的初始化代码，配置成主机模式						  
//SPI口初始化
//这里针是对SPI2的初始化
void SPI2_Init(void)
{
	u16 tempreg=0;
	RCC->AHB1ENR|=1<<1;    	//使能PORTB时钟	   
	RCC->APB1ENR|=1<<14;   	//SPI2时钟使能 
	GPIO_Set(GPIOB,7<<13,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PB13~15复用功能输出	
  	GPIO_AF_Set(GPIOB,13,5);	//PB3,AF5
 	GPIO_AF_Set(GPIOB,14,5);	//PB4,AF5
 	GPIO_AF_Set(GPIOB,15,5);	//PB5,AF5 

	//这里只针对SPI口初始化
	RCC->APB1RSTR|=1<<14;	//复位SPI2
	RCC->APB1RSTR&=~(1<<14);//停止复位SPI2
	tempreg|=0<<10;			//全双工模式	
	tempreg|=1<<9;			//软件nss管理
	tempreg|=1<<8;			 
	tempreg|=1<<2;			//SPI主机  
	tempreg|=0<<11;			//8位数据格式	
	tempreg|=1<<1;			//空闲模式下SCK为1 CPOL=1 
	tempreg|=1<<0;			//数据采样从第2个时间边沿开始,CPHA=1  
 	//对SPI1属于APB2的外设.时钟频率最大为48Mhz频率.
	tempreg|=7<<3;			//Fsck=Fpclk1/256
	tempreg|=0<<7;			//MSB First  
	tempreg|=1<<6;			//SPI启动 
	SPI2->CR1=tempreg; 		//设置CR1  
	SPI2->I2SCFGR&=~(1<<11);//选择SPI模式
	SPI2_ReadWriteByte(0xff);//启动传输		 
	
}   
//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   (SPI 48M@sys  96M)
//SPI_BaudRatePrescaler_8   8分频   (SPI 12M@sys  96M)
//SPI_BaudRatePrescaler_16  16分频  (SPI 6M@sys   96M)
//SPI_BaudRatePrescaler_256 256分频 (SPI 375K@sys 96M)  
void SPI2_SetSpeed(u8 SpeedSet)
{	
	SpeedSet&=0X07;			//限制范围
	SPI2->CR1&=0XFFC7; 
	SPI2->CR1|=SpeedSet<<3;	//设置SPI2速度  
	SPI2->CR1|=1<<6; 		//SPI设备使能 	
} 

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 TxData)
{		
    u16 retry=0;				 
	while((SPI2->SR&1<<1)==0)//等待发送区空	
	{
		retry++;
		if(retry>0XFFFE)return 0;
	}			  
	SPI2->DR=TxData;	 	  //发送一个byte 
	retry=0;
	while((SPI2->SR&1<<0)==0) //等待接收完一个byte  
	{
		retry++;
		if(retry>0XFFFE)return 0;
	}	  						    
	return SPI2->DR;          //返回收到的数据	
   
}

