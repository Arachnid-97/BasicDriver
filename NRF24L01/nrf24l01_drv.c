#include "nrf24l01_drv.h"
#include "bsp_uart.h"


/************************************************
�������� �� NRF24L_Delay_us
��    �� �� ���������ʱ
��    �� �� Count ---- ����
�� �� ֵ �� ��
*************************************************/
static void NRF24L_Delay_us( uint32_t Count )
{
	while(Count)
	{
		Count--;
	}
}

/************************************************
�������� �� NRF24L_Delay_ms
��    �� �� ���������ʱ
��    �� �� Count ---- ����
�� �� ֵ �� ��
*************************************************/
static void NRF24L_Delay_ms( uint32_t Count )
{
	uint16_t i;

	while(Count--)
	{
		for(i = 1240; i > 0; i--);      // ������ʱ��Ϊ iȡֵ
	}
}

/************************************************
�������� �� NRF24L_Data_RW
��    �� �� NRF24L01���ݶ�д 
��    �� �� Data ---- ���� 
�� �� ֵ �� Data ---- ����
*************************************************/
static uint8_t NRF24L_Data_RW( uint8_t Data )
{
	uint8_t i;
	
	NRF_SCK(LOW);
	for(i = 0;i < 8;i++)
	{
		if(Data & 0x80)
		{
			NRF_MOSI(HIGH);
		}
		else
		{
			NRF_MOSI(LOW);
		}
		NRF24L_Delay_us(1);
		Data <<= 1;
		NRF_SCK(HIGH);
		NRF24L_Delay_us(1);
		Data |= NRF_MISO;
		NRF_SCK(LOW);
		NRF24L_Delay_us(1);
	}
	
//	UART_SendByte(Data);
	return Data;
}

/************************************************
�������� �� NRF24L_Write_RegByte
��    �� �� NRF24L01д�Ĵ���һ���ֽ� 
��    �� �� Address ---- ��ַ
			Value ---- ����ֵ 
�� �� ֵ �� ��
*************************************************/
static void NRF24L_Write_RegByte( uint8_t Address, uint8_t Value )
{	
	NRF_CE(LOW);
	NRF_CSN(LOW);
	NRF24L_Data_RW(WRITE_REG | Address);
	NRF24L_Data_RW(Value);
	NRF_CSN(HIGH);
}

/************************************************
�������� �� NRF24L_Read_RegByte
��    �� �� NRF24L01���Ĵ���һ���ֽ� 
��    �� �� Address ---- ��ַ	
�� �� ֵ �� value ---- ����ֵ 
*************************************************/
static uint8_t NRF24L_Read_RegByte( uint8_t Address )
{	
	uint8_t value = 0;
	 
	NRF_CE(LOW);
	NRF_CSN(LOW);
	NRF24L_Data_RW(READ_REG | Address);
	value = NRF24L_Data_RW(NOP);
	NRF_CSN(HIGH);
	
	return value;
}

/************************************************
�������� �� NRF24L_Write_RegnByte
��    �� �� NRF24L01д�Ĵ�������ֽڣ���� 5�ֽڿ�ȣ� 
��    �� �� Address ---- ��ַ
			pData ---- ����ֵ 
			Len ---- �ֽ��� 
�� �� ֵ �� ��
*************************************************/
static void NRF24L_Write_RegnByte( uint8_t Address, const uint8_t *pData ,uint8_t Len )
{	
	NRF_CE(LOW);
	NRF_CSN(LOW);
	NRF24L_Data_RW(WRITE_REG | Address);
	while(Len--)
	{
		NRF24L_Data_RW(*pData++);
	}	
	NRF_CSN(HIGH);
}

/************************************************
�������� �� NRF24L_Read_RegnByte
��    �� �� NRF24L01���Ĵ�������ֽڣ���� 5�ֽڿ�ȣ�
��    �� �� Address ---- ��ַ
			pData ---- ����ֵ
			Len ---- �ֽ���
�� �� ֵ �� ��
*************************************************/
static void NRF24L_Read_RegnByte( uint8_t Address, uint8_t *pData, uint8_t Len )
{
	NRF_CE(LOW);
	NRF_CSN(LOW);
	NRF24L_Data_RW(READ_REG | Address);
	while (Len--)
	{
		*pData = NRF24L_Data_RW(NOP);
		pData++;
	}
	NRF_CSN(HIGH);
}

/************************************************
�������� �� NRF24L_Write_Buf
��    �� �� NRF24L01д���ݣ���� 32�ֽڿ�ȣ�
��    �� �� pBuf ---- ����
			Len ---- �ֽ���
�� �� ֵ �� ��
*************************************************/
void NRF24L_Write_Buf( const uint8_t *pBuf, uint8_t Len )
{
	NRF_CSN(LOW);
	NRF24L_Data_RW(WR_TX_PLOAD);
	while (Len--)
	{
		NRF24L_Data_RW(*pBuf++);
	}
	NRF_CSN(HIGH);
}

/************************************************
�������� �� NRF24L_Read_Buf
��    �� �� NRF24L01�����ݣ���� 32�ֽڿ�ȣ�
��    �� �� pBuf ---- ����
			Len ---- �ֽ���
�� �� ֵ �� ��
*************************************************/
void NRF24L_Read_Buf( uint8_t *pBuf, uint8_t Len )
{
	NRF_CSN(LOW);
	NRF24L_Data_RW(WR_TX_PLOAD);
	while (Len--)
	{
		*pBuf = NRF24L_Data_RW(NOP);
		pBuf++;
	}
	NRF_CSN(HIGH);
}

/************************************************
�������� �� NRF24L_Clean_TX_Flush
��    �� �� ���TX FIFO�Ĵ���
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
static void NRF24L_Clean_TX_Flush(void)
{
	NRF_CSN(LOW);
	NRF24L_Data_RW(FLUSH_TX);
	NRF_CSN(HIGH);
}

/************************************************
�������� �� NRF24L_Clean_RX_Flush
��    �� �� ���RX FIFO�Ĵ���
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
static void NRF24L_Clean_RX_Flush(void)
{
	NRF_CSN(LOW);
	NRF24L_Data_RW(FLUSH_RX);
	NRF_CSN(HIGH);
}

/************************************************
�������� �� NRF24L_SendPacket
��    �� �� NRF24L01���ݰ�����
��    �� �� pData ---- ���ݰ�
			Const ---- ����
�� �� ֵ �� 0 ---- ����ʧ��
			1 ---- ���ͳɹ�
			0xFF ---- �ﵽ����ط�����������ʧ��
*************************************************/
uint8_t NRF24L_SendPacket( const uint8_t *pData, uint8_t Const )
{
	uint8_t state = 0x70;

	if((Const > 0) && (Const <= TX_PLOAD_WIDTH))
	{
		NRF_CE(LOW);
		NRF24L_Write_RegByte(CONFIG, 0x0E);				// ת���ɷ���ģʽ
	//	NRF_CE(HIGH);
	//	NRF24L_Delay_us(10);
	//	NRF_CE(LOW);									// CE���ͣ�ʹ��24L01����
		NRF24L_Write_Buf(pData, Const);					// д���ݵ�TX BUF
		NRF_CE(HIGH);
		NRF24L_Delay_us(10);
		while(NRF_IRQ)									// �ȴ����ͽ���
		{
			if(0xFF == state)
			{
				break;
			}
			state++;
		}
		NRF_CE(LOW);
		state = NRF24L_Read_RegByte(STATUS);			// ��ȡ״̬�Ĵ�����ֵ
		NRF24L_Write_RegByte(STATUS, state);			// ��� TX_DS�� MAX_RT�жϱ�־
	//	NRF_CE(HIGH);
		
//		UART0_SendByte(state);
				
		switch(state & 0x30)
		{
			case TX_OK:
						state = 1;
						break;
			case TX_MAX:
//						NRF24L_Clean_TX_Flush();
						state = 0xFF;
						break;
			default :
						state = 0;
						break;
		}
		NRF24L_Clean_TX_Flush();						// �建��
		
		NRF24L_Write_RegByte(CONFIG, 0x0F);				// ������ɣ�ת���ɽ���ģʽ
		
		NRF_CE(HIGH);
	}
	else
	{
		state = 0;
	}
		
//	UART0_SendByte(state);
	
	return state;
}

/************************************************
�������� �� NRF24L_ReceivePacket
��    �� �� NRF24L01���ݰ�����
��    �� �� pData ---- ���ݰ�
			Const ---- ����
�� �� ֵ �� 0 ---- ����ʧ��
			1 ---- ���ճɹ�
*************************************************/
bit NRF24L_ReceivePacket( uint8_t *pData, uint8_t Const )
{
	uint8_t state = 0x70;

	NRF24L_Clean_RX_Flush();							// �建��
	
	if((Const > 0) && (Const <= RX_PLOAD_WIDTH))
	{
		NRF_CE(LOW);
		state = NRF24L_Read_RegByte(STATUS);			// ��ȡ״̬�Ĵ�����ֵ
		NRF24L_Write_RegByte(STATUS, state);			// ��� RX_DR�жϱ�־
		if(state & RX_OK)
		{
			NRF24L_Read_Buf(pData, Const);				// ����RX BUF������
			NRF24L_Clean_RX_Flush();
			NRF_CE(HIGH);
			NRF24L_Delay_us(10);
			NRF_CE(LOW);
			
	//		UART0_SendString(pData, Const);
			
			return 1;
		}
	}
	
	return 0;
}

/************************************************
�������� �� NRF24L_Check
��    �� �� NRF24L01���
��    �� �� ��
�� �� ֵ �� 0 / 1
*************************************************/
static bit NRF24L_Check(void)
{
	const uint8_t check_in_buf[5] = {0x11,0x22,0x33,0x44,0x55};
	uint8_t check_out_buf[5] = {0};

//	NRF_SCK(LOW);
//	NRF_CSN(HIGH);
	NRF_CE(LOW);

	NRF24L_Write_RegnByte(TX_ADDR, check_in_buf, NRF_ADDR_WIDTH);

//	UART_SendString(check_in_buf, NRF_ADDR_WIDTH);	
	
	NRF24L_Read_RegnByte(TX_ADDR, check_out_buf, NRF_ADDR_WIDTH);
	
//	UART_SendString(check_out_buf, NRF_ADDR_WIDTH);

	NRF_CE(HIGH);
	NRF24L_Delay_us(10);
	
	if ((check_out_buf[0] == check_in_buf[0]) && 
			(check_out_buf[1] == check_in_buf[1]) && 
				(check_out_buf[2] == check_in_buf[2]) && 
					(check_out_buf[3] == check_in_buf[3]) && 
						(check_out_buf[4] == check_in_buf[4]))
		return 1;
	else
		return 0;
}

/************************************************
�������� �� NRF24L_Config
��    �� �� NRF24L01����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void NRF24L_Config(void)
{
	NRF_CE(LOW);    	// P10 0
	NRF_CSN(HIGH);  	// P37 6
	NRF_SCK(LOW);  		// P36 5
}

/************************************************
�������� �� NRF24L_Init
��    �� �� NRF24L01��ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void NRF24L_Init(void)
{
	/* ����ͨ������Ƶ�� 2400 + NRF_Freq MHz */
	const uint8_t NRF_Freq = 120;				// 0 ~ 125

	/* TX��RX ��ַ����*/
	const uint8_t tx_addr[5] = {0x34, 0x43, 0x10, 0x10, 0x01};			// ע�⣺�Զ˵�rx,tx��ַ�����˽���
	const uint8_t rx_addr[5] = {0x34, 0x43, 0x10, 0x10, 0x02};

	NRF24L_Config();

	NRF24L_Delay_ms(2);
	if(NRF24L_Check())
	{
		NRF_CE(LOW);
		NRF24L_Write_RegByte(SETUP_AW, NRF_ADDR_WIDTH - 2);				// ���õ�ַ���
		NRF24L_Write_RegnByte(TX_ADDR, tx_addr, NRF_ADDR_WIDTH);		// Ҫ���͵�Ŀ�ĵ�ַ
		NRF24L_Write_RegnByte(RX_ADDR_P0, rx_addr, NRF_ADDR_WIDTH);		// ����(����)��ַ

		/* ��Ƶ����Ҫ���ա���˫��Ҫһ�� */
		NRF24L_Write_RegByte(EN_AA, 0x01);								// ʹ��ͨ��0���Զ�Ӧ��
		NRF24L_Write_RegByte(EN_RXADDR, 0x01);							// ʹ��ͨ��0�Ľ��յ�ַ
		NRF24L_Write_RegByte(RX_PW_P0, RX_PLOAD_WIDTH);					// ѡ��ͨ��0����Ч���ݿ��
		NRF24L_Write_RegByte(SETUP_RETR, 0x18);							// �����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:8��
		NRF24L_Write_RegByte(RF_CH, NRF_Freq);							// ���� RFƵ�� = 2.4 + 0.120 GHz
		NRF24L_Write_RegByte(RF_SETUP, 0x0F);							// ����TX���������0db���桢2Mbps�����������濪��
		NRF24L_Write_RegByte(CONFIG, 0x0F);								// �������жϡ�ʹ�� 16Bit_CRCУ�顢����ģʽ���ϵ�
		NRF_CE(HIGH);													// CEΪ��,�������ģʽ
	}
}


/*---------------------------- END OF FILE ----------------------------*/



