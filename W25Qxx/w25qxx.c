#include "./W25Qxx/w25qxx.h"
#include "bsp_spi.h"
#include "bsp_uart.h"


/* �Ƿ�����ģ�� SPI ,֧�� mode 0 and mode 3*/
#define USE_SIMULATE_SPI		0

#define MAX_TIME_OUT	((uint32_t)0x1000)

static __IO uint32_t W25Q_TimeOut = MAX_TIME_OUT;

/************************************************
�������� �� TimeOut_Callback
��    �� �� �ȴ���ʱ�ص�����
��    �� �� ErrorCode ---- �������
�� �� ֵ �� ����ֵ 0
*************************************************/

#if (0 == USE_SIMULATE_SPI)
static uint8_t TimeOut_Callback( char ErrorCode )
{
	/* �ȴ���ʱ��Ĵ���,���������Ϣ */
	W25Q_DEBUG_PRINTF("SPI �ȴ���ʱ!	  EerrorCode = %d\n",ErrorCode);

	return 0;
}

#endif /* USE_SIMULATE_SPI */

/************************************************
�������� �� SPI_Flash_SendByte
��    �� �� ʹ��SPI����/ ����һ���ֽڵ�����
��    �� �� wData ---- д����
�� �� ֵ �� rData ---- ������
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
�������� �� W25Qxx_Busy_Wait
��    �� �� W25Qxxæ�ȴ�
��    �� �� ��
�� �� ֵ �� ��
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
�������� �� W25Qxx_Read_JEDECID
��    �� �� �� W25QxxJEDEC_ID�������̡����͡�������
��    �� �� ��
�� �� ֵ �� temp[0] ---- JEDEC_ID
*************************************************/
uint32_t W25Qxx_Read_JEDECID(void)
{
	uint32_t temp[4] = {0};

    W25Q_CS(LOW);
	
	SPI_Flash_SendByte(W25Q_JEDEC_ID);
	temp[1] = SPI_Flash_SendByte(W25Q_DUMMY_BYTE);		// ������
	temp[2] = SPI_Flash_SendByte(W25Q_DUMMY_BYTE);		// ����
	temp[3] = SPI_Flash_SendByte(W25Q_DUMMY_BYTE);		// ����
	temp[0] = (temp[1] << 16) | (temp[2] << 8) | temp[3];
	
    W25Q_CS(HIGH);
	
	return temp[0];
}

/************************************************
�������� �� W25Qxx_Read_Manufacturer_ID
��    �� �� �� W25Qxx������ ID
��    �� �� ��
�� �� ֵ �� id_num ---- ������ ID
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
�������� �� W25Qxx_Read_DeviceID
��    �� �� �� W25Qxx�豸 ID
��    �� �� ��
�� �� ֵ �� id_num ---- �豸 ID
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
�������� �� W25Qxx_Page_Program
��    �� �� W25Qxxҳ��̣����ñ�����д������ǰ��Ҫ�Ȳ���������
��    �� �� pBuffer ---- ����
			Address ---- ��ַ
			Len ---- ����
�� �� ֵ �� ��
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
�������� �� W25Qxx_Write_Flash
��    �� �� д W25Qxx�������ݣ����ñ�����д������ǰ��Ҫ�Ȳ���������
��    �� �� pBuffer ---- ����
			Address ---- ��ַ
			Len ---- ����
�� �� ֵ �� ��
*************************************************/
void W25Qxx_Write_Flash( uint8_t *pBuffer, uint32_t Address, uint16_t Len )
{
	uint8_t NumOfPage = 0, NumOfSingle = 0;
	uint8_t Addr = 0, count = 0, temp = 0;
	
	/* mod�������࣬�� Address�� W25Q_PAGE_SIZE�������������� AddrֵΪ 0 */
	Addr = Address % W25Q_PAGE_SIZE;
	
	/* ��count������ֵ���պÿ��Զ��뵽ҳ��ַ */
	count = W25Q_PAGE_SIZE - Addr;
	
	/* �����Ҫд��������ҳ */
	NumOfPage =  Len / W25Q_PAGE_SIZE;
	
	/* �����ʣ�಻��һҳ���ֽ��� */
	NumOfSingle = Len % W25Q_PAGE_SIZE;
	
	/* Addr = 0,�� Address�պð�ҳ���� */
	if(0 == Addr)
	{
		/* Len <= W25Q_PAGE_SIZE */
		if(0 == NumOfPage) 
		{
			/* ����һҳ or �պ�һҳ */
			W25Qxx_Page_Program(pBuffer, Address, Len);
		}
		else /* Len > W25Q_PAGE_SIZE */
		{ 
			/* �Ȱ�����ҳ�Ķ�д�� */
			while(NumOfPage--)
			{
				W25Qxx_Page_Program(pBuffer, Address, W25Q_PAGE_SIZE);
				Address += W25Q_PAGE_SIZE;
				pBuffer += W25Q_PAGE_SIZE;
			}
			/* ���ж���Ĳ���һҳ�����ݣ���һҳ����д�� */
			if(NumOfSingle != 0)
			{
				W25Qxx_Page_Program(pBuffer, Address, NumOfSingle);
			}
		}
	}
	/* ����ַ�� W25Q_PAGE_SIZE������  */
	else 
	{
		/* Len < W25Q_PAGE_SIZE */
		if(0 == NumOfPage)
		{
			/* ��ǰҳʣ��� count��λ�ñ� NumOfSingleС��һҳд���� */
			if(NumOfSingle > count) 
			{
				/* ��д����ǰҳ */
				W25Qxx_Page_Program(pBuffer, Address, count);
						
				temp = NumOfSingle - count;
				Address += count;
				pBuffer += count;
				/* ��дʣ������� */
				W25Qxx_Page_Program(pBuffer, Address, temp);
			}
			else /* ��ǰҳʣ��� count��λ����д�� NumOfSingle������ */
			{
				W25Qxx_Page_Program(pBuffer, Address, Len);
			}
		}
		else /* Len > W25Q_PAGE_SIZE */
		{
			/* ��ַ���������� count�ֿ������������������ */
			Len -= count;
			NumOfPage =  Len / W25Q_PAGE_SIZE;
			NumOfSingle = Len % W25Q_PAGE_SIZE;
			
			if(count != 0)
			{
				/* ��д��count�����ݣ�Ϊ��������һ��Ҫд�ĵ�ַ���� */
				W25Qxx_Page_Program(pBuffer, Address, count);
					
				/* ���������ظ���ַ�������� */
				Address +=  count;
				pBuffer += count;
			}
			
			/* ������ҳ��д�� */
			while(NumOfPage--)
			{
				W25Qxx_Page_Program(pBuffer, Address, W25Q_PAGE_SIZE);
				Address +=  W25Q_PAGE_SIZE;
				pBuffer += W25Q_PAGE_SIZE;
			}
			/* ���ж���Ĳ���һҳ�����ݣ�����д��*/
			if(NumOfSingle != 0)
			{
				W25Qxx_Page_Program(pBuffer, Address, NumOfSingle);
			}
		}
	}
}

/************************************************
�������� �� W25Qxx_Read_Flash
��    �� �� �� W25Qxx��������
��    �� �� pBuffer ---- ����
			Address ---- ��ַ
			Len ---- ����
�� �� ֵ �� ��
*************************************************/
void W25Qxx_Read_Flash( uint8_t *pBuffer, uint32_t Address, uint16_t Len )
{
    W25Q_CS(LOW);
	
	SPI_Flash_SendByte(W25Q_READ_DATA);
	SPI_Flash_SendByte((Address & 0xFF0000) >> 16);
	SPI_Flash_SendByte((Address & 0xFF00) >> 8);
	SPI_Flash_SendByte(Address & 0xFF);
	
	/* ��ȡ���� */
	while(Len--)
	{
		*pBuffer = SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
		pBuffer++;
	}
	
    W25Q_CS(HIGH);
}

/************************************************
�������� �� W25Qxx_Sector_Erase
��    �� �� FLASH��������
��    �� �� Address ---- ������ַ
�� �� ֵ �� ��
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
�������� �� W25Qxx_Chip_Erase
��    �� �� FLASH��Ƭ����(Ϊ�˰�ȫ�������Ҫ���ã����ȵ��� W25Qxx_Write_Enable����)
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void W25Qxx_Chip_Erase(void)
{
    W25Q_CS(LOW);
	
	SPI_Flash_SendByte(W25Q_CHIP_ERASE);
	
    W25Q_CS(HIGH);
	
	W25Qxx_Busy_Wait();
}

/************************************************
�������� �� W25Qxx_Write_Enable
��    �� �� W25Qxxдʹ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void W25Qxx_Write_Enable(void)
{
	uint8_t flash_status = 0;

    W25Q_CS(LOW);
    SPI_Flash_SendByte(W25Q_WRITE_ENABLE);
    W25Q_CS(HIGH);
		
    W25Q_CS(LOW);
	/* �ȴ�дʹ��λ�� 1 */
	do
	{
		flash_status = SPI_Flash_SendByte(W25Q_STATUS_REG1);
	}while(!(flash_status & BIT_WEL));
    W25Q_CS(HIGH);
}

/************************************************
�������� �� W25Qxx_Write_Disable
��    �� �� W25Qxxдʧ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void W25Qxx_Write_Disable(void)
{
	uint8_t flash_status = 0;

    W25Q_CS(LOW);
    SPI_Flash_SendByte(W25Q_WRITE_DISABLE);
    W25Q_CS(HIGH);
	
    W25Q_CS(LOW);
	/* �ȴ�дʹ���� 0 */
	do
	{
		flash_status = SPI_Flash_SendByte(W25Q_STATUS_REG1);
	}while(!(flash_status & BIT_WEL));
    W25Q_CS(HIGH);

}

/************************************************
�������� �� W25Qxx_Power_Down
��    �� �� W25Qxx����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void W25Qxx_Power_Down(void)
{
    W25Q_CS(LOW);
    SPI_Flash_SendByte(W25Q_POWER_DOWN);
    W25Q_CS(HIGH);
}

/************************************************
�������� �� W25Qxx_Release_PowerDown
��    �� �� W25Qxx����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void W25Qxx_Release_PowerDown(void)
{
    W25Q_CS(LOW);
    SPI_Flash_SendByte(W25Q_RELEASE_POWER_DOWN);
    W25Q_CS(HIGH);
}

/************************************************
�������� �� W25Qxx_Config
��    �� �� W25Qxx����
��    �� �� ��
�� �� ֵ �� ��
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
�������� �� W25Qxx_Init
��    �� �� W25Qxx��ʼ��
��    �� �� ��
�� �� ֵ �� ��
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
		/* ��ȡ������ */
		W25Q_DEBUG_PRINTF("SPI read-write Error, please check the connection between MCU and SPI Flash\n");
	}
	else
	{
		/* ��ȡ�ɹ����� */
		W25Q_DEBUG_PRINTF("SPI read-write succeed\n");
		
//		uint8_t Tx_buff[] = "FLASH��д����ʵ��\r\n";
//		uint8_t Rx_buff[] = "FLASH��д����ʵ��\r\n";

//		W25Qxx_Sector_Erase(0x0100);
//		W25Qxx_Write_Flash(Tx_buff, 0x0100, (sizeof(Tx_buff) / sizeof(*(Tx_buff))));
//		W25Qxx_Read_Flash(Rx_buff, 0x0100, (sizeof(Tx_buff) / sizeof(*(Tx_buff))));
//		W25Q_DEBUG_PRINTF("���������ݣ�%s\n", Rx_buff);
	}

#endif /* _W25Q_DEBUG */
}


/*---------------------------- END OF FILE ----------------------------*/


