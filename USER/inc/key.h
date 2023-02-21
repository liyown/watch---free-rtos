#ifndef __KEY
#define __KEY
#include "sys.h"

	
#define KEY0  1   	
#define KEY1  1
#define KEY2  1	 
#define WK_UP  PAin(0)	 
 
#define KEY0_PRES	1	//KEY0  
#define KEY1_PRES	2	//KEY1 
#define KEY2_PRES	3	//KEY2
#define WKUP_PRES	4	//WK_UP  

void key_init(void);
u8 KEY_Scan(u8 mode);  //°´¼üÉ¨Ãèº¯Êý
#endif

