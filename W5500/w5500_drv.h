#ifndef __W5500_DRV_H
#define __W5500_DRV_H


#include "stm32f4xx.h"


#ifndef ENABLE
#define ENABLE              1
#endif /* ENABLE */

#ifndef DISABLE
#define DISABLE             0
#endif /* DISABLE */

#ifndef HIGH
#define HIGH                1
#endif /* HIGH */

#ifndef LOW
#define LOW                 0
#endif /* LOW */


#define W5500_INT_Clock_FUN(x, y)    RCC_AHB1PeriphClockCmd(x, y)
#define W5500_INT_CLK            RCC_AHB1Periph_GPIOA
#define W5500_INT_PORT           GPIOA
#define W5500_INT_PINS           GPIO_Pin_4
#define W5500_INT_SOURCE         GPIO_PinSource4

#define W5500_RST_Clock_FUN(x, y)    RCC_AHB1PeriphClockCmd(x, y)
#define W5500_RST_CLK            RCC_AHB1Periph_GPIOA
#define W5500_RST_PORT           GPIOA
#define W5500_RST_PINS           GPIO_Pin_5
#define W5500_RST_SOURCE         GPIO_PinSource5


#define W5500_SCS_Clock_FUN(x, y)    RCC_AHB1PeriphClockCmd(x, y)
#define W5500_SCS_CLK            RCC_AHB1Periph_GPIOE
#define W5500_SCS_PORT           GPIOE
#define W5500_SCS_PINS           GPIO_Pin_4
#define W5500_SCS_SOURCE         GPIO_PinSource4

#define W5500_SCLK_Clock_FUN(x, y)   RCC_AHB1PeriphClockCmd(x, y)
#define W5500_SCLK_CLK           RCC_AHB1Periph_GPIOE
#define W5500_SCLK_PORT          GPIOE
#define W5500_SCLK_PINS          GPIO_Pin_2
#define W5500_SCLK_SOURCE        GPIO_PinSource2
#define W5500_MISO_Clock_FUN(x, y)   RCC_AHB1PeriphClockCmd(x, y)
#define W5500_MISO_CLK           RCC_AHB1Periph_GPIOE
#define W5500_MISO_PORT          GPIOE
#define W5500_MISO_PINS          GPIO_Pin_5
#define W5500_MISO_SOURCE        GPIO_PinSource5
#define W5500_MOSI_Clock_FUN(x, y)   RCC_AHB1PeriphClockCmd(x, y)
#define W5500_MOSI_CLK           RCC_AHB1Periph_GPIOE
#define W5500_MOSI_PORT          GPIOE
#define W5500_MOSI_PINS          GPIO_Pin_6
#define W5500_MOSI_SOURCE        GPIO_PinSource6

#define W5500_INT           GPIO_ReadInputDataBit(W5500_INT_PORT, W5500_INT_PINS)
#define W5500_RST(x)        GPIO_WriteBit(W5500_RST_PORT, W5500_RST_PINS, (BitAction)x)
#define W5500_SCS(x)        GPIO_WriteBit(W5500_SCS_PORT, W5500_SCS_PINS, (BitAction)x)
#define W5500_SCLK(x)       GPIO_WriteBit(W5500_SCLK_PORT, W5500_SCLK_PINS, (BitAction)x)
#define W5500_MISO          GPIO_ReadInputDataBit(W5500_MISO_PORT, W5500_MISO_PINS)
#define W5500_MOSI(x)       GPIO_WriteBit(W5500_MOSI_PORT, W5500_MOSI_PINS, (BitAction)x)


void W5500_Delay_ms( uint32_t Count );
void W5500_Write_Byte( uint16_t Address, uint8_t Control, uint8_t Value );
void W5500_Write_Buf( uint16_t Address, uint8_t Control, const uint8_t *pBuf, uint16_t Len );
uint8_t W5500_Read_Byte( uint16_t Address, uint8_t Control );
void W5500_Read_Buf( uint16_t Address, uint8_t Control, uint8_t *pBuf, uint16_t Len );
void W5500_Init(void);


#endif /* __W5500_DRV_H */


/*---------------------------- END OF FILE ----------------------------*/


