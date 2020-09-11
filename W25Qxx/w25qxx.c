#include "./W25Qxx/w25qxx.h"
#include "bsp_spi.h"
#include "bsp_uart.h"


/* 是否启用模拟 SPI ,支持 mode 0 and mode 3*/
#define USE_SIMULATE_SPI		0

#define MAX_TIME_OUT	((uint32_t)0x1000)

static __IO uint32_t W25Q_TimeOut = MAX_TIME_OUT;

/************************************************
函数名称 ： TimeOut_Callback
功    能 ： 等待超时回调函数
参    数 ： ErrorCode ---- 错误代号
返 回 值 ： 错误值 0
*************************************************/

#if (0 == USE_SIMULATE_SPI)
static uint8_t TimeOut_Callback( char ErrorCode )
{
	/* 等待超时后的处理,输出错误信息 */
	W25Q_DEBUG_PRINTF("SPI 等待超时!	  EerrorCode = %d\n",ErrorCode);

	return 0;
}

#endif /* USE_SIMULATE_SPI */

/************************************************
函数名称 ： SPI_Flash_SendByte
功    能 ： 使用SPI发送/ 返回一个字节的数据
参    数 ： wData ---- 写数据
返 回 值 ： rData ---- 读数据
*************************************************/
static uint8_t SPI_Flash_SendByte( uint8_t wData )
{
	
#if USE_SIMULATE_SPI
	return Write_SPI_Byte(wData);
	
#else
	W25Q_TimeOut = MAX_TIME_OUT;
	
	/* Wait for W25Q_SPIx Tx buffer empty */
	while(SPI_I2S_GetFlagStatus(W25Q_SPIx, SPI_I2S_FLAG_TXE) == RESET)
	{
		if(0 == (W25Q_TimeOut--))
			return TimeOut_Callback(0);
	}

	/* Send byte through the W25Q_SPIx peripheral */
	SPI_I2S_SendData(W25Q_SPIx, wData);
	
	W25Q_TimeOut = MAX_TIME_OUT;

	/* Wait for W25Q_SPIx data reception */
	while(SPI_I2S_GetFlagStatus(W25Q_SPIx, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if(0 == (W25Q_TimeOut--))
			return TimeOut_Callback(1);
	}
	
	/* Return the byte read from the W25Q_SPIx bus */
	return SPI_I2S_ReceiveData(W25Q_SPIx);
		
#endif /* USE_SIMULATE_SPI */
}

/************************************************
函数名称 ： W25Qxx_Busy_Wait
功    能 ： W25Qxx忙等待
参    数 ： 无
返 回 值 ： 无
*************************************************/
static void W25Qxx_Busy_Wait(void)
{
	uint8_t flash_status = 0;
	
    W25Q_CS(LOW);
	
	SPI_Flash_SendByte(W25Q_STATUS_REG1);
	
	do
	{
		flash_status = SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
	}while(flash_status & BIT_BUSY);
	
    W25Q_CS(HIGH);
}

/************************************************
函数名称 ： W25Qxx_Read_JEDECID
功    能 ： 读 W25QxxJEDEC_ID（制造商、类型、容量）
参    数 ： 无
返 回 值 ： temp[0] ---- JEDEC_ID
*************************************************/
uint32_t W25Qxx_Read_JEDECID(void)
{
	uint32_t temp[4] = {0};

    W25Q_CS(LOW);
	
	SPI_Flash_SendByte(W25Q_JEDEC_ID);
	temp[1] = SPI_Flash_SendByte(W25Q_DUMMY_BYTE);		// 制造商
	temp[2] = SPI_Flash_SendByte(W25Q_DUMMY_BYTE);		// 类型
	temp[3] = SPI_Flash_SendByte(W25Q_DUMMY_BYTE);		// 容量
	temp[0] = (temp[1] << 16) | (temp[2] << 8) | temp[3];
	
    W25Q_CS(HIGH);
	
	return temp[0];
}

/************************************************
函数名称 ： W25Qxx_Read_Manufacturer_ID
功    能 ： 读 W25Qxx制造商 ID
参    数 ： 无
返 回 值 ： id_num ---- 制造商 ID
*************************************************/
uint16_t W25Qxx_Read_Manufacturer_ID(void)
{
	uint16_t id_num = 0;

    W25Q_CS(LOW);
	
	SPI_Flash_SendByte(W25Q_MANUFACTURER_ID);
	SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
	SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
	SPI_Flash_SendByte(0x00);
	id_num |= SPI_Flash_SendByte(W25Q_DUMMY_BYTE) << 8;
	id_num |= SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
	
    W25Q_CS(HIGH);
	
	return id_num;
}

/************************************************
函数名称 ： W25Qxx_Read_DeviceID
功    能 ： 读 W25Qxx设备 ID
参    数 ： 无
返 回 值 ： id_num ---- 设备 ID
*************************************************/
uint8_t W25Qxx_Read_DeviceID(void)
{
	uint8_t id_num = 0;

    W25Q_CS(LOW);
	
	SPI_Flash_SendByte(W25Q_DEVICE_ID);
	SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
	SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
	SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
	id_num = SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
	
    W25Q_CS(HIGH);
	
	return id_num;
}

/************************************************
函数名称 ： W25Qxx_Page_Program
功    能 ： W25Qxx页编程（调用本函数写入数据前需要先擦除扇区）
参    数 ： pBuffer ---- 数据
			Address ---- 地址
			Len ---- 长度
返 回 值 ： 无
*************************************************/
void W25Qxx_Page_Program( uint8_t *pBuffer, uint32_t Address, uint16_t Len )
{
	W25Qxx_Write_Enable();
	
    W25Q_CS(LOW);
	
	SPI_Flash_SendByte(W25Q_PAGE_PROGRAM);
	SPI_Flash_SendByte((Address & 0xFF0000) >> 16);
	SPI_Flash_SendByte((Address & 0xFF00) >> 8);
	SPI_Flash_SendByte(Address & 0xFF);
	
	if(Len > W25Q_PAGE_SIZE)
	{
		Len = W25Q_PAGE_SIZE;
		W25Q_DEBUG_PRINTF("W25Qxx Page Program data too large!\n"); 
	}
	while(Len--)
	{
		SPI_Flash_SendByte(*pBuffer);
		pBuffer++;
	}
	
    W25Q_CS(HIGH);
	
	W25Qxx_Busy_Wait();
}

/************************************************
函数名称 ： W25Qxx_Write_Flash
功    能 ： 写 W25Qxx闪存数据（调用本函数写入数据前需要先擦除扇区）
参    数 ： pBuffer ---- 数据
			Address ---- 地址
			Len ---- 长度
返 回 值 ： 无
*************************************************/
void W25Qxx_Write_Flash( uint8_t *pBuffer, uint32_t Address, uint16_t Len )
{
	uint8_t NumOfPage = 0, NumOfSingle = 0;
	uint8_t Addr = 0, count = 0, temp = 0;
	
	/* mod运算求余，若 Address是 W25Q_PAGE_SIZE整数倍，运算结果 Addr值为 0 */
	Addr = Address % W25Q_PAGE_SIZE;
	
	/* 差count个数据值，刚好可以对齐到页地址 */
	count = W25Q_PAGE_SIZE - Addr;
	
	/* 计算出要写多少整数页 */
	NumOfPage =  Len / W25Q_PAGE_SIZE;
	
	/* 计算出剩余不满一页的字节数 */
	NumOfSingle = Len % W25Q_PAGE_SIZE;
	
	/* Addr = 0,则 Address刚好按页对齐 */
	if(0 == Addr)
	{
		/* Len <= W25Q_PAGE_SIZE */
		if(0 == NumOfPage) 
		{
			/* 不到一页 or 刚好一页 */
			W25Qxx_Page_Program(pBuffer, Address, Len);
		}
		else /* Len > W25Q_PAGE_SIZE */
		{ 
			/* 先把整数页的都写了 */
			while(NumOfPage--)
			{
				W25Qxx_Page_Program(pBuffer, Address, W25Q_PAGE_SIZE);
				Address += W25Q_PAGE_SIZE;
				pBuffer += W25Q_PAGE_SIZE;
			}
			/* 若有多余的不满一页的数据，下一页把它写完 */
			if(NumOfSingle != 0)
			{
				W25Qxx_Page_Program(pBuffer, Address, NumOfSingle);
			}
		}
	}
	/* 若地址与 W25Q_PAGE_SIZE不对齐  */
	else 
	{
		/* Len < W25Q_PAGE_SIZE */
		if(0 == NumOfPage)
		{
			/* 当前页剩余的 count个位置比 NumOfSingle小，一页写不完 */
			if(NumOfSingle > count) 
			{
				/* 先写满当前页 */
				W25Qxx_Page_Program(pBuffer, Address, count);
						
				temp = NumOfSingle - count;
				Address += count;
				pBuffer += count;
				/* 再写剩余的数据 */
				W25Qxx_Page_Program(pBuffer, Address, temp);
			}
			else /* 当前页剩余的 count个位置能写完 NumOfSingle个数据 */
			{
				W25Qxx_Page_Program(pBuffer, Address, Len);
			}
		}
		else /* Len > W25Q_PAGE_SIZE */
		{
			/* 地址不对齐多出的 count分开处理，不加入这个运算 */
			Len -= count;
			NumOfPage =  Len / W25Q_PAGE_SIZE;
			NumOfSingle = Len % W25Q_PAGE_SIZE;
			
			if(count != 0)
			{
				/* 先写完count个数据，为的是让下一次要写的地址对齐 */
				W25Qxx_Page_Program(pBuffer, Address, count);
					
				/* 接下来就重复地址对齐的情况 */
				Address +=  count;
				pBuffer += count;
			}
			
			/* 把整数页都写了 */
			while(NumOfPage--)
			{
				W25Qxx_Page_Program(pBuffer, Address, W25Q_PAGE_SIZE);
				Address +=  W25Q_PAGE_SIZE;
				pBuffer += W25Q_PAGE_SIZE;
			}
			/* 若有多余的不满一页的数据，把它写完*/
			if(NumOfSingle != 0)
			{
				W25Qxx_Page_Program(pBuffer, Address, NumOfSingle);
			}
		}
	}
}

/************************************************
函数名称 ： W25Qxx_Read_Flash
功    能 ： 读 W25Qxx闪存数据
参    数 ： pBuffer ---- 数据
			Address ---- 地址
			Len ---- 长度
返 回 值 ： 无
*************************************************/
void W25Qxx_Read_Flash( uint8_t *pBuffer, uint32_t Address, uint16_t Len )
{
    W25Q_CS(LOW);
	
	SPI_Flash_SendByte(W25Q_READ_DATA);
	SPI_Flash_SendByte((Address & 0xFF0000) >> 16);
	SPI_Flash_SendByte((Address & 0xFF00) >> 8);
	SPI_Flash_SendByte(Address & 0xFF);
	
	/* 读取数据 */
	while(Len--)
	{
		*pBuffer = SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
		pBuffer++;
	}
	
    W25Q_CS(HIGH);
}

/************************************************
函数名称 ： W25Qxx_Sector_Erase
功    能 ： FLASH扇区擦除
参    数 ： Address ---- 擦除地址
返 回 值 ： 无
*************************************************/
void W25Qxx_Sector_Erase( uint32_t Address )
{
	W25Qxx_Write_Enable();
	
    W25Q_CS(LOW);
	
	SPI_Flash_SendByte(W25Q_SECTOR_ERASE);
	SPI_Flash_SendByte((Address & 0xFF0000) >> 16);
	SPI_Flash_SendByte((Address & 0xFF00) >> 8);
	SPI_Flash_SendByte(Address & 0xFF);

    W25Q_CS(HIGH);
	
	W25Qxx_Busy_Wait();
}

/************************************************
函数名称 ： W25Qxx_Chip_Erase
功    能 ： FLASH整片擦除(为了安全起见，若要调用，请先调用 W25Qxx_Write_Enable函数)
参    数 ： 无
返 回 值 ： 无
*************************************************/
void W25Qxx_Chip_Erase(void)
{
    W25Q_CS(LOW);
	
	SPI_Flash_SendByte(W25Q_CHIP_ERASE);
	
    W25Q_CS(HIGH);
	
	W25Qxx_Busy_Wait();
}

/************************************************
函数名称 ： W25Qxx_Write_Enable
功    能 ： W25Qxx写使能
参    数 ： 无
返 回 值 ： 无
*************************************************/
void W25Qxx_Write_Enable(void)
{
	uint8_t flash_status = 0;

    W25Q_CS(LOW);
    SPI_Flash_SendByte(W25Q_WRITE_ENABLE);
    W25Q_CS(HIGH);
		
    W25Q_CS(LOW);
	/* 等待写使能位置 1 */
	do
	{
		flash_status = SPI_Flash_SendByte(W25Q_STATUS_REG1);
	}while(!(flash_status & BIT_WEL));
    W25Q_CS(HIGH);
}

/************************************************
函数名称 ： W25Qxx_Write_Disable
功    能 ： W25Qxx写失能
参    数 ： 无
返 回 值 ： 无
*************************************************/
void W25Qxx_Write_Disable(void)
{
	uint8_t flash_status = 0;

    W25Q_CS(LOW);
    SPI_Flash_SendByte(W25Q_WRITE_DISABLE);
    W25Q_CS(HIGH);
	
    W25Q_CS(LOW);
	/* 等待写使能清 0 */
	do
	{
		flash_status = SPI_Flash_SendByte(W25Q_STATUS_REG1);
	}while(!(flash_status & BIT_WEL));
    W25Q_CS(HIGH);

}

/************************************************
函数名称 ： W25Qxx_Power_Down
功    能 ： W25Qxx掉电
参    数 ： 无
返 回 值 ： 无
*************************************************/
void W25Qxx_Power_Down(void)
{
    W25Q_CS(LOW);
    SPI_Flash_SendByte(W25Q_POWER_DOWN);
    W25Q_CS(HIGH);
}

/************************************************
函数名称 ： W25Qxx_Release_PowerDown
功    能 ： W25Qxx唤醒
参    数 ： 无
返 回 值 ： 无
*************************************************/
void W25Qxx_Release_PowerDown(void)
{
    W25Q_CS(LOW);
    SPI_Flash_SendByte(W25Q_RELEASE_POWER_DOWN);
    W25Q_CS(HIGH);
}

/************************************************
函数名称 ： W25Qxx_Config
功    能 ： W25Qxx配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
void W25Qxx_Config(void)
{
	
#if USE_SIMULATE_SPI
	GPIO_InitTypeDef GPIO_InitStructure;
	
	W25Q_CS_APBxClock_FUN(W25Q_CS_CLK, ENABLE);
	
	/* Confugure CS pin as Output Push Pull */
	GPIO_InitStructure.GPIO_Pin = W25Q_CS_PINS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(W25Q_CS_PORT, &GPIO_InitStructure);

	/* Smiulate IO Config */
	Simulate_SPI_Config();
	
	W25Q_CS(HIGH);
	SPI_SCK(HIGH);
	SPI_MOSI(HIGH);

#else
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	/* W25Q_SPIx IO Periph clock enable */
	W25Q_IO_APBxClock_FUN(W25Q_CS_CLK | W25Q_SCK_CLK 	
								| W25Q_MISO_CLK | W25Q_MOSI_CLK, ENABLE);
	
	/* W25Q_SPIx Periph clock enable */
	W25Q_SPI_APBxClock_FUN(W25Q_SPI_CLK, ENABLE);

	/* Configure W25Q_SPIx pins: CS, SCK, MISO and MOSI */	
	/* Confugure CS pin as Output Push Pull */
	GPIO_InitStructure.GPIO_Pin = W25Q_CS_PINS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(W25Q_CS_PORT, &GPIO_InitStructure);

	/* Confugure SCK and MOSI pins as Alternate Function Push Pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	
	GPIO_InitStructure.GPIO_Pin = W25Q_SCK_PINS;
	GPIO_Init(W25Q_SCK_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = W25Q_MOSI_PINS;
	GPIO_Init(W25Q_MOSI_PORT, &GPIO_InitStructure);
	
	/* Confugure MISO pin as Input Floating */
	GPIO_InitStructure.GPIO_Pin = W25Q_MISO_PINS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(W25Q_MISO_PORT, &GPIO_InitStructure);
	
	/* ---------- END ---------- */
	
	W25Q_CS(HIGH);
	
	/* W25Q_SPIx configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(W25Q_SPIx, &SPI_InitStructure);
	
	/* Disable W25Q_SPIx CRC calculation */
	SPI_CalculateCRC(W25Q_SPIx, DISABLE);

	/* Enable W25Q_SPIx */
	SPI_Cmd(SPI1, ENABLE);
	
#endif /* USE_SIMULATE_SPI */
}

/************************************************
函数名称 ： W25Qxx_Init
功    能 ： W25Qxx初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void W25Qxx_Init(void)
{
	uint32_t FlashID = 0;
	
	W25Qxx_Config();
	
#if(_W25Q_DEBUG)
	FlashID = W25Qxx_Read_JEDECID();
	W25Q_DEBUG_PRINTF("FlashID is 0x%X,Manufacturer Device ID is 0x%X\r\n",	\
				FlashID, W25Qxx_Read_DeviceID());
	if(FlashID != JEDEC_ID)
	{
		/* 读取错误处理 */
		W25Q_DEBUG_PRINTF("SPI read-write Error, please check the connection between MCU and SPI Flash\n");
	}
	else
	{
		/* 读取成功处理 */
		W25Q_DEBUG_PRINTF("SPI read-write succeed\n");
		
//		uint8_t Tx_buff[] = "FLASH读写测试实验\r\n";
//		uint8_t Rx_buff[] = "FLASH读写测试实验\r\n";

//		W25Qxx_Sector_Erase(0x0100);
//		W25Qxx_Write_Flash(Tx_buff, 0x0100, (sizeof(Tx_buff) / sizeof(*(Tx_buff))));
//		W25Qxx_Read_Flash(Rx_buff, 0x0100, (sizeof(Tx_buff) / sizeof(*(Tx_buff))));
//		W25Q_DEBUG_PRINTF("读出的数据：%s\n", Rx_buff);
	}

#endif /* _W25Q_DEBUG */
}


/*---------------------------- END OF FILE ----------------------------*/


