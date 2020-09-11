#include "nrf24l01_drv.h"
#include "bsp_uart.h"


/************************************************
函数名称 ： NRF24L_Delay_us
功    能 ： 软件毫秒延时
参    数 ： Count ---- 次数
返 回 值 ： 无
*************************************************/
static void NRF24L_Delay_us( uint32_t Count )
{
	while(Count)
	{
		Count--;
	}
}

/************************************************
函数名称 ： NRF24L_Delay_ms
功    能 ： 软件毫秒延时
参    数 ： Count ---- 次数
返 回 值 ： 无
*************************************************/
static void NRF24L_Delay_ms( uint32_t Count )
{
	uint16_t i;

	while(Count--)
	{
		for(i = 1240; i > 0; i--);      // 根据震荡时间为 i取值
	}
}

/************************************************
函数名称 ： NRF24L_Data_RW
功    能 ： NRF24L01数据读写 
参    数 ： Data ---- 数据 
返 回 值 ： Data ---- 数据
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
函数名称 ： NRF24L_Write_RegByte
功    能 ： NRF24L01写寄存器一个字节 
参    数 ： Address ---- 地址
			Value ---- 输入值 
返 回 值 ： 无
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
函数名称 ： NRF24L_Read_RegByte
功    能 ： NRF24L01读寄存器一个字节 
参    数 ： Address ---- 地址	
返 回 值 ： value ---- 输入值 
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
函数名称 ： NRF24L_Write_RegnByte
功    能 ： NRF24L01写寄存器多个字节（最大 5字节宽度） 
参    数 ： Address ---- 地址
			pData ---- 输入值 
			Len ---- 字节数 
返 回 值 ： 无
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
函数名称 ： NRF24L_Read_RegnByte
功    能 ： NRF24L01读寄存器多个字节（最大 5字节宽度）
参    数 ： Address ---- 地址
			pData ---- 读出值
			Len ---- 字节数
返 回 值 ： 无
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
函数名称 ： NRF24L_Write_Buf
功    能 ： NRF24L01写数据（最大 32字节宽度）
参    数 ： pBuf ---- 数据
			Len ---- 字节数
返 回 值 ： 无
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
函数名称 ： NRF24L_Read_Buf
功    能 ： NRF24L01读数据（最大 32字节宽度）
参    数 ： pBuf ---- 数据
			Len ---- 字节数
返 回 值 ： 无
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
函数名称 ： NRF24L_Clean_TX_Flush
功    能 ： 清除TX FIFO寄存器
参    数 ： 无
返 回 值 ： 无
*************************************************/
static void NRF24L_Clean_TX_Flush(void)
{
	NRF_CSN(LOW);
	NRF24L_Data_RW(FLUSH_TX);
	NRF_CSN(HIGH);
}

/************************************************
函数名称 ： NRF24L_Clean_RX_Flush
功    能 ： 清除RX FIFO寄存器
参    数 ： 无
返 回 值 ： 无
*************************************************/
static void NRF24L_Clean_RX_Flush(void)
{
	NRF_CSN(LOW);
	NRF24L_Data_RW(FLUSH_RX);
	NRF_CSN(HIGH);
}

/************************************************
函数名称 ： NRF24L_SendPacket
功    能 ： NRF24L01数据包发送
参    数 ： pData ---- 数据包
			Const ---- 长度
返 回 值 ： 0 ---- 发送失败
			1 ---- 发送成功
			0xFF ---- 达到最大重发次数，发送失败
*************************************************/
uint8_t NRF24L_SendPacket( const uint8_t *pData, uint8_t Const )
{
	uint8_t state = 0x70;

	if((Const > 0) && (Const <= TX_PLOAD_WIDTH))
	{
		NRF_CE(LOW);
		NRF24L_Write_RegByte(CONFIG, 0x0E);				// 转换成发送模式
	//	NRF_CE(HIGH);
	//	NRF24L_Delay_us(10);
	//	NRF_CE(LOW);									// CE拉低，使能24L01配置
		NRF24L_Write_Buf(pData, Const);					// 写数据到TX BUF
		NRF_CE(HIGH);
		NRF24L_Delay_us(10);
		while(NRF_IRQ)									// 等待发送结束
		{
			if(0xFF == state)
			{
				break;
			}
			state++;
		}
		NRF_CE(LOW);
		state = NRF24L_Read_RegByte(STATUS);			// 读取状态寄存器的值
		NRF24L_Write_RegByte(STATUS, state);			// 清除 TX_DS或 MAX_RT中断标志
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
		NRF24L_Clean_TX_Flush();						// 清缓存
		
		NRF24L_Write_RegByte(CONFIG, 0x0F);				// 发送完成，转换成接收模式
		
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
函数名称 ： NRF24L_ReceivePacket
功    能 ： NRF24L01数据包接收
参    数 ： pData ---- 数据包
			Const ---- 长度
返 回 值 ： 0 ---- 接收失败
			1 ---- 接收成功
*************************************************/
bit NRF24L_ReceivePacket( uint8_t *pData, uint8_t Const )
{
	uint8_t state = 0x70;

	NRF24L_Clean_RX_Flush();							// 清缓存
	
	if((Const > 0) && (Const <= RX_PLOAD_WIDTH))
	{
		NRF_CE(LOW);
		state = NRF24L_Read_RegByte(STATUS);			// 读取状态寄存器的值
		NRF24L_Write_RegByte(STATUS, state);			// 清除 RX_DR中断标志
		if(state & RX_OK)
		{
			NRF24L_Read_Buf(pData, Const);				// 读出RX BUF的数据
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
函数名称 ： NRF24L_Check
功    能 ： NRF24L01检测
参    数 ： 无
返 回 值 ： 0 / 1
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
函数名称 ： NRF24L_Config
功    能 ： NRF24L01配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
void NRF24L_Config(void)
{
	NRF_CE(LOW);    	// P10 0
	NRF_CSN(HIGH);  	// P37 6
	NRF_SCK(LOW);  		// P36 5
}

/************************************************
函数名称 ： NRF24L_Init
功    能 ： NRF24L01初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void NRF24L_Init(void)
{
	/* 工作通道接收频率 2400 + NRF_Freq MHz */
	const uint8_t NRF_Freq = 120;				// 0 ~ 125

	/* TX、RX 地址配置*/
	const uint8_t tx_addr[5] = {0x34, 0x43, 0x10, 0x10, 0x01};			// 注意：对端的rx,tx地址跟本端交换
	const uint8_t rx_addr[5] = {0x34, 0x43, 0x10, 0x10, 0x02};

	NRF24L_Config();

	NRF24L_Delay_ms(2);
	if(NRF24L_Check())
	{
		NRF_CE(LOW);
		NRF24L_Write_RegByte(SETUP_AW, NRF_ADDR_WIDTH - 2);				// 设置地址宽度
		NRF24L_Write_RegnByte(TX_ADDR, tx_addr, NRF_ADDR_WIDTH);		// 要发送的目的地址
		NRF24L_Write_RegnByte(RX_ADDR_P0, rx_addr, NRF_ADDR_WIDTH);		// 自身(接收)地址

		/* 射频参数要求收、发双方要一致 */
		NRF24L_Write_RegByte(EN_AA, 0x01);								// 使能通道0的自动应答
		NRF24L_Write_RegByte(EN_RXADDR, 0x01);							// 使能通道0的接收地址
		NRF24L_Write_RegByte(RX_PW_P0, RX_PLOAD_WIDTH);					// 选择通道0的有效数据宽度
		NRF24L_Write_RegByte(SETUP_RETR, 0x18);							// 设置自动重发间隔时间:500us + 86us;最大自动重发次数:8次
		NRF24L_Write_RegByte(RF_CH, NRF_Freq);							// 设置 RF频率 = 2.4 + 0.120 GHz
		NRF24L_Write_RegByte(RF_SETUP, 0x0F);							// 设置TX发射参数：0db增益、2Mbps、低噪声增益开启
		NRF24L_Write_RegByte(CONFIG, 0x0F);								// 开所有中断、使能 16Bit_CRC校验、接收模式、上电
		NRF_CE(HIGH);													// CE为高,进入接收模式
	}
}


/*---------------------------- END OF FILE ----------------------------*/



