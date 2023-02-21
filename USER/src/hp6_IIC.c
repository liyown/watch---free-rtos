#include "stm32f4xx.h"
#include "delay.h"


#define HP6_SDA_H   (GPIOA->BSRRL = 1<<1)
#define HP6_SDA_L   (GPIOA->BSRRH = 1<<1)  //PA1

#define HP6_SCL_H   (GPIOA->BSRRL = 1<<2)  //PA2
#define HP6_SCL_L   (GPIOA->BSRRH = 1<<2)

#define HP6_SDA_IN   ( !!(GPIOA->IDR & (1<<1)) )//PA1



//SCL  ---  PA2
//SDA  ---  PA1
void HP6IIC_Init()
{
	RCC->AHB1ENR |= 1<<0;//PA
	
	GPIOA->MODER &= ~(0XF<<2);
	GPIOA->MODER |= (0X5<<2);  //ͨ�����
	    
	GPIOA->OTYPER &= ~(1<<2);// PA2  SCL �������
	GPIOA->OTYPER |= (1<<1);//  PA1  SDA ��©���
	
//  /*****IIC_SCL����Ϊ��©���****/
//  RCC->AHB1ENR |= 1<<0;//ʹ��PAʱ��
//  GPIOA->MODER &=~(3<<4);//����
//  GPIOA->MODER |= 1<<4;//GPIOA2����ͨ�����ģʽ
//  GPIOA->OTYPER |= 1<<2;//GPIOA2����Ϊ��©ģʽ
//  /****IIC_SDA����Ϊ��©���****/
//  GPIOA->MODER &=~(3<<2);//����
//  GPIOA->MODER |= 1<<2;//GPIOA1������ͨģʽ
//  GPIOA->OTYPER |= 1<<1;//GPIOA1����Ϊͨ�����ģʽ
	
	HP6_SDA_H;
	HP6_SCL_H;	
}



void HP6IIC_Start(void)
{
  delay_us(5);
  HP6_SCL_H;
  HP6_SDA_H;
  delay_us(5);//��ʱ5us
  HP6_SDA_L;
  delay_us(5);//��ʱ5us
  HP6_SCL_L;

}

void HP6IIC_Stop(void)
{
  HP6_SDA_L;
  delay_us(5);
  HP6_SCL_H;
  delay_us(5);//��ʱ5us
  HP6_SDA_H;
  delay_us(5);//��ʱ5us
	
}

//data �������͵�����
//����ֵ��0��ʾӦ��1��ʾ��Ӧ��
u8 HP6IIC_WriteByte(u8 data)
{
  u8 i;
  u8 ack = 2;
  for(i=0;i<8;i++)  //8��ʱ�Ӵ���8λ���ݣ������λ��ʼ����
  {
    if(data &(1<<(7-i)))  
    {
      HP6_SDA_H;
    }
    else
    {
      HP6_SDA_L;
    }
    delay_us(5);//��ʱ
    HP6_SCL_H; 
    delay_us(5);//��ʱ
    HP6_SCL_L;
  }
	
	//����Ӧ���
  HP6_SDA_H;//SDA����ߵ͵�ƽ�л�Ϊ����
  delay_us(5);
  HP6_SCL_H;
  if(HP6_SDA_IN)//SDA����ĵ�ƽΪ�ߵ�ƽ��ʾ��Ӧ��
  {
    ack = 1;
  }
  else  //SDA����ĵ�ƽΪ�͵�ƽ��ʾӦ��
  {
    ack = 0;
  }
  delay_us(5);
  HP6_SCL_L;
  return ack;
}


//����ֵ��ʾ��ȡ��������
//�β�ack����ʾ��ȡһ���ֽ����ݺ������Ӧ��0��Ӧ��1����Ӧ��
u8 HP6IIC_ReadByte(u8 ack)
{
  u8 i;
  u8 data = 0;
  HP6_SDA_H;//SDA����ߵ�ƽ���л�Ϊ����
  for(i=0;i<8;i++)  //����8��ʱ�ӣ���ȡ8λ����
  {
    delay_us(10);
    HP6_SCL_H;
    data = data <<1;
    if(HP6_SDA_IN)
    {
      data |= 1<<0;
    }
    else   //SDA����ĵ͵�ƽ��ƽ
    {
      data &=~(1<<0);
    }	
    delay_us(10); 
    HP6_SCL_L;		
  }
  //�����ھŸ�ʱ�ӣ�����Ӧ���źš�
  if(ack == 0)
  {
    HP6_SDA_L;   
  }
  else
  {
    HP6_SDA_H;
  }
  delay_us(10);
  HP6_SCL_H;
  delay_us(10);
  HP6_SCL_L;
  return data;
}
