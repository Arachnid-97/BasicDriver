#include "nrf24l01_drv.h"
#include "bsp_uart.h"


#define NRF_DYNAMIC_PACKET		0

#define TIME_DELAY				10

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
    NRF24L_Data_RW(NRF_WRITE_REG | Address);
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
    NRF24L_Data_RW(NRF_READ_REG | Address);
    value = NRF24L_Data_RW(NRF_NOP);
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
    NRF24L_Data_RW(NRF_READ_REG | Address);
    while (Len--)
    {
        *pData = NRF24L_Data_RW(NRF_NOP);
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
    NRF24L_Data_RW(NRF_WR_TX_PLOAD);
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
    NRF24L_Data_RW(NRF_WR_TX_PLOAD);
    while (Len--)
    {
        *pBuf = NRF24L_Data_RW(NRF_NOP);
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
    NRF24L_Data_RW(NRF_FLUSH_TX);
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
    NRF24L_Data_RW(NRF_FLUSH_RX);
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

    if((Const > 0) && (Const <= NRF_TX_PLOAD_WIDTH))
    {
        NRF_CE(LOW);
        NRF24L_Write_RegByte(NRF_CONFIG, 0x0E);				// ת���ɷ���ģʽ
        NRF24L_Write_Buf(pData, Const);						// д���ݵ�TX BUF
        NRF_CE(HIGH);
        NRF24L_Delay_us(TIME_DELAY);
        while(NRF_IRQ)										// �ȴ����ͽ���
        {
            if(0xFF == state)
            {
                break;
            }
            state++;
        }
        NRF_CE(LOW);
        state = NRF24L_Read_RegByte(NRF_STATUS);			// ��ȡ״̬�Ĵ�����ֵ
        NRF24L_Write_RegByte(NRF_STATUS, state);			// ��� TX_DS�� MAX_RT�жϱ�־

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
        NRF24L_Clean_TX_Flush();							// �建��

        NRF24L_Write_RegByte(NRF_CONFIG, 0x0F);				// ������ɣ�ת���ɽ���ģʽ

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
�������� �� NRF24L_ReceivePacket
��    �� �� NRF24L01���ݰ�����
��    �� �� pData ---- ���ݰ�
			Const ---- ����
�� �� ֵ �� 0 ---- ����ʧ��
			1 ---- ���ճɹ�
*************************************************/
uint8_t NRF24L_ReceivePacket( uint8_t *pData, uint8_t Const )
{
    uint8_t state = 0x70;

    if((Const > 0) && (Const <= NRF_RX_PLOAD_WIDTH))
    {
        NRF_CE(LOW);
        state = NRF24L_Read_RegByte(NRF_STATUS);			// ��ȡ״̬�Ĵ�����ֵ
        NRF24L_Write_RegByte(NRF_STATUS, state);			// ��� RX_DR�жϱ�־
        if(state & NRF_RX_OK)
        {
            NRF24L_Read_Buf(pData, Const);					// ����RX BUF������
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
�������� �� NRF24L_Check
��    �� �� NRF24L01���
��    �� �� ��
�� �� ֵ �� 0 / 1
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
�������� �� NRF24L_Config
��    �� �� NRF24L01����
��    �� �� ��
�� �� ֵ �� ��
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
�������� �� NRF24L_Init
��    �� �� NRF24L01��ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
uint8_t NRF24L_Init(void)
{
    /* ����ͨ������Ƶ�� 2400 + NRF_Freq MHz */
    const uint8_t NRF_Freq = 120;				// 0 ~ 125

    /* TX��RX ��ַ����*/
    const uint8_t tx_addr[5] = {0x34, 0x43, 0x10, 0x10, 0x01};				// ע�⣺�Զ˵�rx,tx��ַ�����˽���
    const uint8_t rx_addr[5] = {0x34, 0x43, 0x10, 0x10, 0x02};

	NRF24L_Config();

	NRF24L_Delay_ms(2);
    if(NRF24L_Check())
    {
        NRF_CE(LOW);
        NRF24L_Write_RegByte(NRF_SETUP_AW, NRF_ADDR_WIDTH - 2);				// ���õ�ַ���
        NRF24L_Write_RegnByte(NRF_TX_ADDR, tx_addr, NRF_ADDR_WIDTH);		// Ҫ���͵�Ŀ�ĵ�ַ
        NRF24L_Write_RegnByte(NRF_RX_ADDR_P0, rx_addr, NRF_ADDR_WIDTH);		// ����(����)��ַ

        /* ��Ƶ����Ҫ���ա���˫��Ҫһ�� */
        NRF24L_Write_RegByte(NRF_EN_AA, 0x00);								// ʹ��ͨ��0���Զ�Ӧ��
        NRF24L_Write_RegByte(NRF_EN_RXADDR, 0x01);							// ʹ��ͨ��0�Ľ��յ�ַ

	#if (NRF_DYNAMIC_PACKET == 1)
        NRF24L_Write_RegByte(NRF_DYNPD, (1 << 0)); 							// ʹ��ͨ�� 0��̬���ݳ���
        NRF24L_Write_RegByte(NRF_FEATRUE, 0x07);
        NRF24L_Read_RegByte(NRF_DYNPD);
        NRF24L_Read_RegByte(NRF_FEATRUE);

	#elif NRF_DYNAMIC_PACKET == 0
        NRF24L_Write_RegByte(NRF_RX_PW_P0, NRF_RX_PLOAD_WIDTH);				// ѡ��ͨ��0����Ч���ݿ��(��̬)

	#endif /* NRF_DYNAMIC_PACKET */

        NRF24L_Write_RegByte(NRF_SETUP_RETR, 0x18);							// �����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:8��
        NRF24L_Write_RegByte(NRF_RF_CH, NRF_Freq);							// ���� RFƵ�� = 2.4 + 0.120 GHz
        NRF24L_Write_RegByte(NRF_RF_SETUP, 0x0F);							// ����TX���������0db���桢2Mbps�����������濪��
        NRF24L_Write_RegByte(NRF_CONFIG, 0x0F);								// �������жϡ�ʹ�� 16Bit_CRCУ�顢����ģʽ���ϵ�
        NRF_CE(HIGH);														// CEΪ��,�������ģʽ

        DEBUG_PRINTF("NRF Init success...\r\n");
        return 1;
    }

    DEBUG_PRINTF("NRF Init fail...\r\n");
    return 0;
}


/*---------------------------- END OF FILE ----------------------------*/



