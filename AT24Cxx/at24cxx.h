#ifndef __AT24CXX_H
#define __AT24CXX_H


#include "stm32f4xx.h"


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


/* The M24C08W contains 4 blocks (128byte each) with the adresses below: E2 = 0 
   EEPROM Addresses defines */
#define EEPROM_BLOCK0_ADDRESS       0xA0 /* E2 = 0 */
// #define EEPROM_BLOCK1_ADDRESS       0xA2 /* E2 = 0 */
// #define EEPROM_BLOCK2_ADDRESS       0xA4 /* E2 = 0 */
// #define EEPROM_BLOCK3_ADDRESS       0xA6 /* E2 = 0 */

/* AT24C设备内存总容量 x8bit */
#define AT24C_MEMORY_CAPACITY       512

/* AT24C01/02每页有 8个字节 
 * AT24C04/08/16每页有 16个字节 */
#define AT24C_PAGE_SIZE             16

/* 使用 16位数据则定义该宏,否者默认 8位数据 */
// #define AT24CXX_16BIT_ADDR

/* 使用 10位字地址则定义该宏,否者默认(标准) 7位字地址 */
// #define I2C_10BITS_ADDRESS

/* --------------------------------------------------------------------- */

/* I2C EEPROM Interface */
#define EE_I2C_DMA                      DMA1   
#define EE_I2C_DMA_CHANNEL              DMA_Channel_7
#define EE_I2C_DMA_STREAM_TX            DMA1_Stream7
#define EE_I2C_DMA_STREAM_RX            DMA1_Stream2 
#define EE_I2C_DMA_FLAG_TX_TC           DMA_FLAG_TCIF7
#define EE_I2C_DMA_IT_FLAG_TX           DMA_IT_TCIF7
#define EE_I2C_DMA_FLAG_RX_TC           DMA_FLAG_TCIF2
#define EE_I2C_DMA_IT_FLAG_RX           DMA_IT_TCIF2
#define EE_I2C_DMA_CLK                  RCC_AHB1Periph_DMA1
#define EE_I2C_DR_Address               (&(AT24C_I2Cx->DR))

#define EE_I2C_DMA_TX_IRQn              DMA1_Stream7_IRQn
#define EE_I2C_DMA_RX_IRQn              DMA1_Stream2_IRQn
#define EE_I2C_DMA_TX_IRQHandler        DMA1_Stream7_IRQHandler
#define EE_I2C_DMA_RX_IRQHandler        DMA1_Stream2_IRQHandler
#define EE_I2C_DMA_PREPRIO              0
#define EE_I2C_DMA_SUBPRIO              0

#define EE_DIRECTION_TX             0
#define EE_DIRECTION_RX             1
                                 
/* IIC时钟速率 */
#define I2C_SPEED                   400000
/* 这个地址只要与 STM32外挂的 I2C器件地址不一样即可 */
#define I2C_SLAVE_ADDRESS           0x0A

#define AT24C_READ                  0x01
#define AT24C_WRITE                 0x00


/* AT24Cxx IICx */
#define AT24C_I2Cx              I2C2
#define AT24C_I2C_CLK           RCC_APB1Periph_I2C2
#define AT24C_I2C_CLOCK_FUN(x, y)   RCC_APB1PeriphClockCmd(x, y)

/* AT24Cxx GPIO */
#define AT24C_SCL_CLOCK_FUN(x, y)   RCC_AHB1PeriphClockCmd(x, y)
#define AT24C_SCL_CLK           RCC_AHB1Periph_GPIOB
#define AT24C_SCL_PORT          GPIOB
#define AT24C_SCL_PINS          GPIO_Pin_10
#define AT24C_SCL_SOURCE        GPIO_PinSource10
#define AT24C_SDA_CLOCK_FUN(x, y)   RCC_AHB1PeriphClockCmd(x, y)
#define AT24C_SDA_CLK           RCC_AHB1Periph_GPIOB
#define AT24C_SDA_PORT          GPIOB
#define AT24C_SDA_PINS          GPIO_Pin_11
#define AT24C_SDA_SOURCE        GPIO_PinSource11

#define AT24C_I2C_GPIO_AF_MAP   GPIO_AF_I2C2

/* 如果 IO的桥接线一样就直接用下面的宏，否则分开配置 */
#define AT24C_IO_CLOCK_FUN(x, y)    RCC_AHB1PeriphClockCmd(x, y)


void EE_DMA_TxWait(void);
void EE_DMA_RxWait(void);

uint8_t AT24Cxx_Write_Byte( uint8_t Byte, uint16_t Address );
uint8_t AT24Cxx_Read_Byte( uint16_t Address );
uint8_t AT24Cxx_Page_Program( uint8_t *pBuffer, uint16_t Address, uint16_t Len );
void AT24Cxx_Write_EEPROM( uint8_t *pBuffer, uint16_t Address, uint16_t Len );
uint8_t AT24Cxx_Read_EEPROM( uint8_t *pBuffer, uint16_t Address, uint16_t Len );
void AT24Cxx_Config(void);
void AT24Cxx_Init(void);

/* Start and End of critical section: these callbacks should be typically used
   to disable interrupts when entering a critical section of I2C communication
   You may use default callbacks provided into this driver by uncommenting the 
   define USE_DEFAULT_CRITICAL_CALLBACK in stm32_evel_i2c_ee.h file..
   Or you can comment that line and implement these callbacks into your 
   application */
void EE_EnterCriticalSection_UserCallback(void);
void EE_ExitCriticalSection_UserCallback(void);


#endif /* __AT24CXX_H */


/*---------------------------- END OF FILE ----------------------------*/

