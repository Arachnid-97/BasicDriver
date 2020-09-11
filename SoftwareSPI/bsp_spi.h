#ifndef __BSP_SPI_H
#define __BSP_SPI_H


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

#define SL_SPI_SCK_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
#define SL_SPI_SCK_CLK		RCC_APB2Periph_GPIOA
#define SL_SPI_SCK_PORT		GPIOA
#define SL_SPI_SCK_PINS		GPIO_Pin_5

#define SL_SPI_MOSI_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
#define SL_SPI_MOSI_CLK		RCC_APB2Periph_GPIOA
#define SL_SPI_MOSI_PORT	GPIOA
#define SL_SPI_MOSI_PINS	GPIO_Pin_7

#define SL_SPI_MISO_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
#define SL_SPI_MISO_CLK		RCC_APB2Periph_GPIOA
#define SL_SPI_MISO_PORT	GPIOA
#define SL_SPI_MISO_PINS	GPIO_Pin_6

#define SPI_SCK(x)			GPIO_WriteBit(SL_SPI_SCK_PORT, SL_SPI_SCK_PINS, (BitAction)x)
#define SPI_MOSI(x)			GPIO_WriteBit(SL_SPI_MOSI_PORT, SL_SPI_MOSI_PINS, (BitAction)x)
#define SPI_MISO			GPIO_ReadInputDataBit(SL_SPI_MISO_PORT, SL_SPI_MISO_PINS)

uint8_t Write_SPI_Byte( uint8_t Byte );
uint8_t Read_SPI_Byte(void);
void Simulate_SPI_Config(void);


#endif /* __BSP_SPI_H */


/*---------------------------- END OF FILE ----------------------------*/


