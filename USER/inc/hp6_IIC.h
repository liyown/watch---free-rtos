/*******************************************************************************
Copyright 2008 - 2016 深圳市信盈达科技有限公司. All rights reserved.
文件名:        ii.h
描述   :       模拟iic头文件
作者   :       Jahol Fan
版本   :       V1.0
修改   :   
完成日期：     2016.9.19
*******************************************************************************/
#ifndef HP6_IIC_H
#define HP6_IIC_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "stm32f4xx.h"
/*********************************************************************
 * TYPEDEFS
 */
/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */



/*********************************************************************
 * MACROS
 */

/*********************************************************************
 *PUBLIC FUNCTIONS DECLARE
 */
void HP6IIC_Init(void);
void HP6IIC_Start(void);
void HP6IIC_Stop(void);
u8 HP6IIC_WriteByte(u8 data);
u8  HP6IIC_ReadByte(u8 ack);



/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* HP6_IIC_H */
