#ifndef __OLED_H
#define __OLED_H 

#include "sys.h"
#include "stdlib.h"	

//-----------------测试LED端口定义---------------- 

#define LED_ON GPIO_ResetBits(GPIOC,GPIO_Pin_12)
#define LED_OFF GPIO_SetBits(GPIOC,GPIO_Pin_12)

//-----------------OLED端口定义---------------- 

#define OLED_SCL_Clr() PBout(3) = 0//SCL
#define OLED_SCL_Set() PBout(3) =1

#define OLED_SDA_Clr() PBout(5) =0//SDA5
#define OLED_SDA_Set() PBout(5) =1

#define OLED_RES_Clr() PBout(13) =0//RES15
#define OLED_RES_Set() PBout(13) =1

#define OLED_DC_Clr()  PAout(15) =0//DC15
#define OLED_DC_Set()  PAout(15) =1
 		     
#define OLED_CS_Clr()  PBout(7) =0//CS7
#define OLED_CS_Set()  PBout(7) =1


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

void OLED_ClearPoint(u8 x,u8 y);
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void OLED_WR_Byte(u8 dat,u8 mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode);
void OLED_DrawCircle(u8 x,u8 y,u8 r);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode);
void OLED_ShowChar6x8(u8 x,u8 y,u8 chr,u8 mode);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode);
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode);
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode);
void OLED_Init(void);

#endif

