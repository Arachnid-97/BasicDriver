#include "nrf24l01_drv.h"
#include "bsp_uart.h"


#define NRF_DYNAMIC_PACKET		0

#define TIME_DELAY				10

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
    for(i = 0; i < 8; i++)
    {
        if(Data & 0x80)
        {
            NRF_MOSI(HIGH);
        }
        else
        {
            NRF_MOSI(LOW);
        }
        NRF24L_Delay_us(3);
        Data <<= 1;
        NRF_SCK(HIGH);
        NRF24L_Delay_us(3);
        Data |= NRF_MISO;
        NRF_SCK(LOW);
        NRF24L_Delay_us(3);
    }

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
    NRF24L_Data_RW(NRF_WRITE_REG | Address);
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
    NRF24L_Data_RW(NRF_READ_REG | Address);
    value = NRF24L_Data_RW(NRF_NOP);
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
static void NRF24L_Write_RegnByte( uint8_t Address, const uint8_t *pData,uint8_t Len )
{
    NRF_CE(LOW);
    NRF_CSN(LOW);
    NRF24L_Data_RW(NRF_WRITE_REG | Address);
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
    NRF24L_Data_RW(NRF_READ_REG | Address);
    while (Len--)
    {
        *pData = NRF24L_Data_RW(NRF_NOP);
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
    NRF24L_Data_RW(NRF_WR_TX_PLOAD);
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
    NRF24L_Data_RW(NRF_WR_TX_PLOAD);
    while (Len--)
    {
        *pBuf = NRF24L_Data_RW(NRF_NOP);
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
    NRF24L_Data_RW(NRF_FLUSH_TX);
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
    NRF24L_Data_RW(NRF_FLUSH_RX);
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

    if((Const > 0) && (Const <= NRF_TX_PLOAD_WIDTH))
    {
        NRF_CE(LOW);
        NRF24L_Write_RegByte(NRF_CONFIG, 0x0E);				// 转换成发送模式
        NRF24L_Write_Buf(pData, Const);						// 写数据到TX BUF
        NRF_CE(HIGH);
        NRF24L_Delay_us(TIME_DELAY);
        while(NRF_IRQ)										// 等待发送结束
        {
            if(0xFF == state)
            {
                break;
            }
            state++;
        }
        NRF_CE(LOW);
        state = NRF24L_Read_RegByte(NRF_STATUS);			// 读取状态寄存器的值
        NRF24L_Write_RegByte(NRF_STATUS, state);			// 清除 TX_DS或 MAX_RT中断标志

        switch(state & 0x30)
        {
        case NRF_TX_OK:
            state = 1;
            break;
        case NRF_TX_MAX:
            state = 0xFF;
            break;
        default :
            state = 0;
            break;
        }
        NRF24L_Clean_TX_Flush();							// 清缓存

        NRF24L_Write_RegByte(NRF_CONFIG, 0x0F);				// 发送完成，转换成接收模式

        NRF_CE(HIGH);
    }
    else
    {
        state = 0;
    }

	DEBUG_PRINTF("status=%d", state);

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
uint8_t NRF24L_ReceivePacket( uint8_t *pData, uint8_t Const )
{
    uint8_t state = 0x70;

    if((Const > 0) && (Const <= NRF_RX_PLOAD_WIDTH))
    {
        NRF_CE(LOW);
        state = NRF24L_Read_RegByte(NRF_STATUS);			// 读取状态寄存器的值
        NRF24L_Write_RegByte(NRF_STATUS, state);			// 清除 RX_DR中断标志
        if(state & NRF_RX_OK)
        {
            NRF24L_Read_Buf(pData, Const);					// 读出RX BUF的数据
            NRF24L_Clean_RX_Flush();

//			USART_SendString(DEBUG_UART, pData, Const);

            state = 1;
        }
		else
		{
			state = 0;
		}
		NRF_CE(HIGH);
		NRF24L_Delay_us(TIME_DELAY);
    }
	else
	{
		state = 0;
	}

    return state;
}

/************************************************
函数名称 ： NRF24L_Check
功    能 ： NRF24L01检测
参    数 ： 无
返 回 值 ： 0 / 1
*************************************************/
static uint8_t NRF24L_Check(void)
{
    const uint8_t check_in_buf[5] = {0x11,0x22,0x33,0x44,0x55};
    uint8_t check_out_buf[5] = {0};

    NRF_CE(LOW);

    NRF24L_Write_RegnByte(NRF_TX_ADDR, check_in_buf, NRF_ADDR_WIDTH);

//	USART_SendString(DEBUG_UART, check_in_buf, NRF_ADDR_WIDTH);

    NRF24L_Read_RegnByte(NRF_TX_ADDR, check_out_buf, NRF_ADDR_WIDTH);

//	USART_SendString(DEBUG_UART, check_out_buf, NRF_ADDR_WIDTH);

    NRF_CE(HIGH);
    NRF24L_Delay_us(TIME_DELAY);

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
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, (FunctionalState)ENABLE);

    /* Configure PC.3 pin as output push pull */
    GPIO_InitStructure.GPIO_Pin = NRF_CSN_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(NRF_CSN_GPIO_PORT, &GPIO_InitStructure);

    /* Configure PC.6 pin as output push pull */
    GPIO_InitStructure.GPIO_Pin = NRF_SCK_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(NRF_SCK_GPIO_PORT, &GPIO_InitStructure);

    /* Configure PB.15 pin as input pull up */
    GPIO_InitStructure.GPIO_Pin = NRF_MISO_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(NRF_MISO_GPIO_PORT, &GPIO_InitStructure);

    /* Configure PC.7 pin as output push pull */
    GPIO_InitStructure.GPIO_Pin = NRF_MOSI_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(NRF_MOSI_GPIO_PORT, &GPIO_InitStructure);

    /* Configure PC.2 pin as output push pull */
    GPIO_InitStructure.GPIO_Pin = NRF_CE_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(NRF_CE_GPIO_PORT, &GPIO_InitStructure);

    /* Configure PC.8 pin as input pull up */
    GPIO_InitStructure.GPIO_Pin = NRF_IRQ_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(NRF_IRQ_GPIO_PORT, &GPIO_InitStructure);

    NRF_CE(LOW);
    NRF_CSN(HIGH);
    NRF_SCK(LOW);
}

/************************************************
函数名称 ： NRF24L_Init
功    能 ： NRF24L01初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
uint8_t NRF24L_Init(void)
{
    /* 工作通道接收频率 2400 + NRF_Freq MHz */
    const uint8_t NRF_Freq = 120;				// 0 ~ 125

    /* TX、RX 地址配置*/
    const uint8_t tx_addr[5] = {0x34, 0x43, 0x10, 0x10, 0x01};				// 注意：对端的rx,tx地址跟本端交换
    const uint8_t rx_addr[5] = {0x34, 0x43, 0x10, 0x10, 0x02};

	NRF24L_Config();

	NRF24L_Delay_ms(2);
    if(NRF24L_Check())
    {
        NRF_CE(LOW);
        NRF24L_Write_RegByte(NRF_SETUP_AW, NRF_ADDR_WIDTH - 2);				// 设置地址宽度
        NRF24L_Write_RegnByte(NRF_TX_ADDR, tx_addr, NRF_ADDR_WIDTH);		// 要发送的目的地址
        NRF24L_Write_RegnByte(NRF_RX_ADDR_P0, rx_addr, NRF_ADDR_WIDTH);		// 自身(接收)地址

        /* 射频参数要求收、发双方要一致 */
        NRF24L_Write_RegByte(NRF_EN_AA, 0x00);								// 使能通道0的自动应答
        NRF24L_Write_RegByte(NRF_EN_RXADDR, 0x01);							// 使能通道0的接收地址

	#if (NRF_DYNAMIC_PACKET == 1)
        NRF24L_Write_RegByte(NRF_DYNPD, (1 << 0)); 							// 使能通道 0动态数据长度
        NRF24L_Write_RegByte(NRF_FEATRUE, 0x07);
        NRF24L_Read_RegByte(NRF_DYNPD);
        NRF24L_Read_RegByte(NRF_FEATRUE);

	#elif NRF_DYNAMIC_PACKET == 0
        NRF24L_Write_RegByte(NRF_RX_PW_P0, NRF_RX_PLOAD_WIDTH);				// 选择通道0的有效数据宽度(静态)

	#endif /* NRF_DYNAMIC_PACKET */

        NRF24L_Write_RegByte(NRF_SETUP_RETR, 0x18);							// 设置自动重发间隔时间:500us + 86us;最大自动重发次数:8次
        NRF24L_Write_RegByte(NRF_RF_CH, NRF_Freq);							// 设置 RF频率 = 2.4 + 0.120 GHz
        NRF24L_Write_RegByte(NRF_RF_SETUP, 0x0F);							// 设置TX发射参数：0db增益、2Mbps、低噪声增益开启
        NRF24L_Write_RegByte(NRF_CONFIG, 0x0F);								// 开所有中断、使能 16Bit_CRC校验、接收模式、上电
        NRF_CE(HIGH);														// CE为高,进入接收模式

        DEBUG_PRINTF("NRF Init success...\r\n");
        return 1;
    }

    DEBUG_PRINTF("NRF Init fail...\r\n");
    return 0;
}


/*---------------------------- END OF FILE ----------------------------*/



