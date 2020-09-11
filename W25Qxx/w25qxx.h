#ifndef __W25QXX_H
#define __W25QXX_H


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

#define _W25Q_DEBUG			1
#define W25Q_DEBUG_PRINTF(fmt,arg...)		do{\
											if(_W25Q_DEBUG)\
												printf("<<-FLASH-DEBUG->> < %s >[%d]\n"fmt"\n",__FILE__,__LINE__, ##arg);\
											}while(0)

/* W25Qxx SPIx */
#define W25Q_SPIx        		SPI1
#define W25Q_SPI_CLK     		RCC_APB2Periph_SPI1
#define W25Q_SPI_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)

/* W25Qxx GPIO */
#define W25Q_CS_APBxClock_FUN(x, y)		RCC_APB2PeriphClockCmd(x, y)
#define W25Q_CS_CLK				RCC_APB2Periph_GPIOC
#define W25Q_CS_PORT   		    GPIOC
#define W25Q_CS_PINS   		    GPIO_Pin_0

#define W25Q_SCK_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
#define W25Q_SCK_CLK			RCC_APB2Periph_GPIOA
#define W25Q_SCK_PORT     		GPIOA
#define W25Q_SCK_PINS   	    GPIO_Pin_5
#define W25Q_MISO_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
#define W25Q_MISO_CLK			RCC_APB2Periph_GPIOA
#define W25Q_MISO_PORT   		GPIOA
#define W25Q_MISO_PINS    		GPIO_Pin_6
#define W25Q_MOSI_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
#define W25Q_MOSI_CLK			RCC_APB2Periph_GPIOA
#define W25Q_MOSI_PORT    		GPIOA
#define W25Q_MOSI_PINS    		GPIO_Pin_7

/* 如果 IO的桥接线一样就直接用下面的宏，否则分开配置 */
#define W25Q_IO_APBxClock_FUN(x, y)		RCC_APB2PeriphClockCmd(x, y)


#define W25Q_CS(x)				GPIO_WriteBit(W25Q_CS_PORT, W25Q_CS_PINS, (BitAction)x)


//#define JEDEC_ID              0xEF3015		//W25X16
//#define JEDEC_ID              0xEF4015		//W25Q16
//#define JEDEC_ID              0xEF4018		//W25Q128
#define JEDEC_ID              0xEF4017		//W25Q64

#define W25Q_DUMMY_BYTE			0xFF	// 可以为任意字节

/* ---------- 命令字节 ---------- */
#define W25Q_WRITE_ENABLE		0x06
#define W25Q_WRITE_DISABLE		0x04
#define W25Q_STATUS_REG1		0x05
#define W25Q_STATUS_REG2		0x35
#define W25Q_PAGE_PROGRAM		0x02
#define W25Q_SECTOR_ERASE		0x20
#define W25Q_BLOCK32_ERASE		0x52
#define W25Q_BLOCK64_ERASE		0xD8
#define W25Q_POWER_DOWN			0xB9
#define W25Q_READ_DATA			0x03
#define W25Q_FAST_READ			0x0B
#define W25Q_RELEASE_POWER_DOWN	0xAB
#define W25Q_DEVICE_ID			0xAB
#define W25Q_MANUFACTURER_ID	0x90
#define W25Q_JEDEC_ID			0x9F
#define W25Q_UNIQUE_ID_NUM		0x4B

#define W25Q_CHIP_ERASE			0xC7

/* ------------ End ------------ */

/* Status Registers Bit */
#define BIT_BUSY				0x01
#define BIT_WEL					0x02

#define W25Q_PAGE_SIZE			256

/* ID read function */
uint32_t W25Qxx_Read_JEDECID(void);
uint16_t W25Qxx_Read_Manufacturer_ID(void);
uint8_t W25Qxx_Read_DeviceID(void);

/* Programming function */
void W25Qxx_Page_Program( uint8_t *pBuffer, uint32_t Address, uint16_t Len );
void W25Qxx_Write_Flash( uint8_t *pBuffer, uint32_t Address, uint16_t Len );
void W25Qxx_Read_Flash( uint8_t *pBuffer, uint32_t Address, uint16_t Len );
void W25Qxx_Sector_Erase( uint32_t Address );
void W25Qxx_Chip_Erase(void);

void W25Qxx_Write_Enable(void);
void W25Qxx_Write_Disable(void);
void W25Qxx_Power_Down(void);
void W25Qxx_Release_PowerDown(void);
void W25Qxx_Config(void);
void W25Qxx_Init(void);


#endif	/* __W25QXX_H */


/*---------------------------- END OF FILE ----------------------------*/

