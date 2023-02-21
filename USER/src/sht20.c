#include "sht20.h"
#include "delay.h"
#include "usart.h"
#include <stdio.h>

SHT2x_data SHT20; 
//��ʼ��IIC
void IIC_Init(void)
{					     

	RCC->AHB1ENR|=1<<1;    //ʹ��PORTBʱ��
	GPIO_Set(GPIOB,PIN8|PIN9,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PB8/PB9���� 
	IIC_SCL=1;
	IIC_SDA=1;
	
}
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();//SDA����Ϊ����  
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
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
int IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	
	return IIC_Wait_Ack();
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}

//��������
u8 SHT2x_Init(void)
{
	u8 err;
    IIC_Init();
    err = SHT2x_SoftReset();
	return err;
}

u8 SHT2x_SoftReset(void) //SHT20�����λ
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
    IIC_Start();				//����IIC��ʼ�ź�
    ack =IIC_Send_Byte(I2C_ADR_W);			//IIC����һ���ֽ� 
	ack =IIC_Send_Byte(TRIG_TEMP_MEASUREMENT_POLL);
    do {
        delay_ms(100);               
        IIC_Start();				//����IIC��ʼ�ź�
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
    IIC_Start();				//����IIC��ʼ�ź�
    ack =IIC_Send_Byte(I2C_ADR_W);			//IIC����һ���ֽ� 	
	ack =IIC_Send_Byte(TRIG_HUMI_MEASUREMENT_POLL);    
    do {
        delay_ms(100);               
        IIC_Start();				//����IIC��ʼ�ź�
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



