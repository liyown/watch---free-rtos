#include "spi.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK NANO STM32F4������
//SPI��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2019/4/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2019-2029
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////    
 
//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ						  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI2�ĳ�ʼ��
void SPI2_Init(void)
{
	u16 tempreg=0;
	RCC->AHB1ENR|=1<<1;    	//ʹ��PORTBʱ��	   
	RCC->APB1ENR|=1<<14;   	//SPI2ʱ��ʹ�� 
	GPIO_Set(GPIOB,7<<13,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PB13~15���ù������	
  	GPIO_AF_Set(GPIOB,13,5);	//PB3,AF5
 	GPIO_AF_Set(GPIOB,14,5);	//PB4,AF5
 	GPIO_AF_Set(GPIOB,15,5);	//PB5,AF5 

	//����ֻ���SPI�ڳ�ʼ��
	RCC->APB1RSTR|=1<<14;	//��λSPI2
	RCC->APB1RSTR&=~(1<<14);//ֹͣ��λSPI2
	tempreg|=0<<10;			//ȫ˫��ģʽ	
	tempreg|=1<<9;			//���nss����
	tempreg|=1<<8;			 
	tempreg|=1<<2;			//SPI����  
	tempreg|=0<<11;			//8λ���ݸ�ʽ	
	tempreg|=1<<1;			//����ģʽ��SCKΪ1 CPOL=1 
	tempreg|=1<<0;			//���ݲ����ӵ�2��ʱ����ؿ�ʼ,CPHA=1  
 	//��SPI1����APB2������.ʱ��Ƶ�����Ϊ48MhzƵ��.
	tempreg|=7<<3;			//Fsck=Fpclk1/256
	tempreg|=0<<7;			//MSB First  
	tempreg|=1<<6;			//SPI���� 
	SPI2->CR1=tempreg; 		//����CR1  
	SPI2->I2SCFGR&=~(1<<11);//ѡ��SPIģʽ
	SPI2_ReadWriteByte(0xff);//��������		 
	
}   
//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   (SPI 48M@sys  96M)
//SPI_BaudRatePrescaler_8   8��Ƶ   (SPI 12M@sys  96M)
//SPI_BaudRatePrescaler_16  16��Ƶ  (SPI 6M@sys   96M)
//SPI_BaudRatePrescaler_256 256��Ƶ (SPI 375K@sys 96M)  
void SPI2_SetSpeed(u8 SpeedSet)
{	
	SpeedSet&=0X07;			//���Ʒ�Χ
	SPI2->CR1&=0XFFC7; 
	SPI2->CR1|=SpeedSet<<3;	//����SPI2�ٶ�  
	SPI2->CR1|=1<<6; 		//SPI�豸ʹ�� 	
} 

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 TxData)
{		
    u16 retry=0;				 
	while((SPI2->SR&1<<1)==0)//�ȴ���������	
	{
		retry++;
		if(retry>0XFFFE)return 0;
	}			  
	SPI2->DR=TxData;	 	  //����һ��byte 
	retry=0;
	while((SPI2->SR&1<<0)==0) //�ȴ�������һ��byte  
	{
		retry++;
		if(retry>0XFFFE)return 0;
	}	  						    
	return SPI2->DR;          //�����յ�������	
   
}

