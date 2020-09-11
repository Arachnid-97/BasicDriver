#ifndef __AT24CXX_H
#define __AT24CXX_H


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

#define _AT24C_DEBUG		1
#define AT24C_DEBUG_PRINTF(fmt,arg...)		do{\
											if(_AT24C_DEBUG)\
												printf("<<-EEPROM-DEBUG->> < %s >[%d]\n"fmt"\n",__FILE__,__LINE__, ##arg);\
											}while(0)

/* I2C EEPROM Interface */
#define EE_I2C_DMA                      DMA1   
#define EE_I2C_DMA_CHANNEL_TX           DMA1_Channel6
#define EE_I2C_DMA_CHANNEL_RX           DMA1_Channel7 
#define EE_I2C_DMA_FLAG_TX_TC           DMA1_IT_TC6   
#define EE_I2C_DMA_FLAG_TX_GL           DMA1_IT_GL6 
#define EE_I2C_DMA_FLAG_RX_TC           DMA1_IT_TC7 
#define EE_I2C_DMA_FLAG_RX_GL           DMA1_IT_GL7    
#define EE_I2C_DMA_CLK                  RCC_AHBPeriph_DMA1
#define EE_I2C_DR_Address               (&(AT24C_I2Cx->DR))

#define EE_I2C_DMA_TX_IRQn              DMA1_Channel6_IRQn
#define EE_I2C_DMA_RX_IRQn              DMA1_Channel7_IRQn
#define EE_I2C_DMA_TX_IRQHandler        DMA1_Channel6_IRQHandler
#define EE_I2C_DMA_RX_IRQHandler        DMA1_Channel7_IRQHandler
#define EE_I2C_DMA_PREPRIO              0
#define EE_I2C_DMA_SUBPRIO              0

#define EE_DIRECTION_TX					0
#define EE_DIRECTION_RX					1
											
/* IIC时钟速率 */
#define I2C_SPEED               		400000
/* 这个地址只要与 STM32外挂的 I2C器件地址不一样即可 */
#define I2C_SLAVE_ADDRESS7     			0x0A

#define AT24C_READ						0x01
#define AT24C_WRITE						0x00


/* The M24C08W contains 4 blocks (128byte each) with the adresses below: E2 = 0 
   EEPROM Addresses defines */
#define EEPROM_BLOCK0_ADDRESS     		0xA0	/* E2 = 0 */ 
//#define EEPROM_BLOCK1_ADDRESS     		0xA2	/* E2 = 0 */  
//#define EEPROM_BLOCK2_ADDRESS     		0xA4	/* E2 = 0 */
//#define EEPROM_BLOCK3_ADDRESS     		0xA6	/* E2 = 0 */
 
/* AT24Cxx IICx */
#define AT24C_I2Cx	        	I2C1
#define AT24C_I2C_CLK     		RCC_APB1Periph_I2C1
#define AT24C_I2C_APBxClock_FUN(x, y)	RCC_APB1PeriphClockCmd(x, y)

/* AT24Cxx GPIO */
#define AT24C_SCL_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
#define AT24C_SCL_CLK			RCC_APB2Periph_GPIOB
#define AT24C_SCL_PORT   		GPIOB
#define AT24C_SCL_PINS   		GPIO_Pin_6
#define AT24C_SDA_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
#define AT24C_SDA_CLK			RCC_APB2Periph_GPIOB
#define AT24C_SDA_PORT   		GPIOB
#define AT24C_SDA_PINS   		GPIO_Pin_7

/* 如果 IO的桥接线一样就直接用下面的宏，否则分开配置 */
#define AT24C_IO_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)


/* AT24C01/02每页有8个字节 */
#define AT24C_PAGE_SIZE           8

/* AT24C04/08A/16A每页有16个字节 */
//#define AT24C_PAGE_SIZE           16	

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


#endif	/* __AT24CXX_H */


/*---------------------------- END OF FILE ----------------------------*/

