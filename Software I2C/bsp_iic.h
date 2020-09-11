#ifndef __BSP_IIC_H
#define __BSP_IIC_H


#include "stm32f10x.h"

//#ifndef ENABLE
//#define ENABLE              1
//#endif /* ENABLE */

//#ifndef DISABLE
//#define DISABLE             0
//#endif /* DISABLE */

#ifndef HIGH
#define HIGH                1
#endif /* HIGH */

#ifndef LOW
#define LOW                 0
#endif /* LOW */

#define SL_IIC_SCL_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
#define SL_IIC_SCL_CLK		RCC_APB2Periph_GPIOB
#define SL_IIC_SCL_PORT		GPIOB
#define SL_IIC_SCL_PINS		GPIO_Pin_6

#define SL_IIC_SDA_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
#define SL_IIC_SDA_CLK		RCC_APB2Periph_GPIOB
#define SL_IIC_SDA_PORT		GPIOB
#define SL_IIC_SDA_PINS		GPIO_Pin_7

#define IIC_SCL(x)			GPIO_WriteBit(SL_IIC_SCL_PORT, SL_IIC_SCL_PINS, (BitAction)x)
#define IIC_SDA(x)			GPIO_WriteBit(SL_IIC_SDA_PORT, SL_IIC_SDA_PINS, (BitAction)x)

#define IIC_SDA_READ		GPIO_ReadInputDataBit(SL_IIC_SDA_PORT, SL_IIC_SDA_PINS)

uint8_t IIC_Start(void);
void IIC_Stop(void);
void IIC_Ack(void);
void IIC_UnAck(void);
uint8_t IIC_Wait_Ack(void);
void Write_IIC_Byte( uint8_t Byte );
uint8_t Read_IIC_Byte(void);
void Simulate_IIC_Config(void);


#endif /* __BSP_IIC_H */


/*---------------------------- END OF FILE ----------------------------*/


