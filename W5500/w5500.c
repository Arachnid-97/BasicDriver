#include "w5500.h"
#include "w5500_drv.h"


    /* ----- Common Register Function ----- */

/************************************************
�������� �� Set_MR
��    �� �� ģʽ����
��    �� �� Value ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_MR( uint8_t Value )
{
    W5500_Write_Byte(MR, COMM_REG, Value);
}

/************************************************
�������� �� Set_GAR
��    �� �� �������� IP��ַ
��    �� �� pData ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_GAR( const uint8_t *pData )
{
    W5500_Write_Buf(GAR0, COMM_REG, pData, 4);
}

/************************************************
�������� �� Get_GAR
��    �� �� �������� IP��ַ
��    �� �� pData ---- ����
�� �� ֵ �� ��
*************************************************/
void Get_GAR( uint8_t *pData )
{
    W5500_Read_Buf(GAR0, COMM_REG, pData, 4);
}

/************************************************
�������� �� Set_SUBR
��    �� �� �������������ַ
��    �� �� pData ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_SUBR( const uint8_t *pData )
{
    W5500_Write_Buf(SUBR0, COMM_REG, pData, 4);
}

/************************************************
�������� �� Get_SUBR
��    �� �� ��ȡ���������ַ
��    �� �� pData ---- ����
�� �� ֵ �� ��
*************************************************/
void Get_SUBR( uint8_t *pData )
{
    W5500_Read_Buf(SUBR0, COMM_REG, pData, 4);
}

/************************************************
�������� �� Set_SHAR
��    �� �� ����Դ MAC��ַ
��    �� �� pData ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_SHAR( const uint8_t *pData )
{
    W5500_Write_Buf(SHAR0, COMM_REG, pData, 6);
}

/************************************************
�������� �� Get_SHAR
��    �� �� ��ȡԴ MAC��ַ
��    �� �� pData ---- ����
�� �� ֵ �� ��
*************************************************/
void Get_SHAR( uint8_t *pData )
{
    W5500_Read_Buf(SHAR0, COMM_REG, pData, 6);
}

/************************************************
�������� �� Set_SIPR
��    �� �� ����Դ IP��ַ
��    �� �� pData ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_SIPR( const uint8_t *pData )
{
    W5500_Write_Buf(SIPR0, COMM_REG, pData, 4);
}

/************************************************
�������� �� Get_SIPR
��    �� �� ��ȡԴ IP��ַ
��    �� �� pData ---- ����
�� �� ֵ �� ��
*************************************************/
void Get_SIPR( uint8_t *pData )
{
    W5500_Read_Buf(SIPR0, COMM_REG, pData, 4);
}

/************************************************
�������� �� Get_IR
��    �� �� �� IR�ж�
��    �� �� ��
�� �� ֵ �� IR State ---- �ж�״̬
*************************************************/
uint8_t Get_IR(void)
{
    return W5500_Read_Byte(IR, COMM_REG);
}

/************************************************
�������� �� Clean_IR
��    �� �� �� IR�ж�
��    �� �� Mask >>>> IR register values
�� �� ֵ �� ��
*************************************************/
void Clean_IR( uint8_t Mask )
{
    W5500_Write_Byte(IR, COMM_REG, ~Mask | Get_IR());
}

/************************************************
�������� �� Set_RTR
��    �� �� ��������ʱ��ֵ
��    �� �� Value ---- ʱ��ֵ
�� �� ֵ �� ��
*************************************************/
void Set_RTR( uint16_t Value )
{
    W5500_Write_Byte(RTR0, COMM_REG, (uint8_t)(Value & 0xFF00) >> 8);
    W5500_Write_Byte(RTR1, COMM_REG, (uint8_t)(Value & 0x00FF));
}

/************************************************
�������� �� Set_RCR
��    �� �� �������Լ���ֵ
��    �� �� Value ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_RCR( uint8_t Value )
{
    W5500_Write_Byte(RCR, COMM_REG, Value);
}

/************************************************
�������� �� Get_PHYCFGR
��    �� �� ��ȡ PHYCFGR����
��    �� �� ��
�� �� ֵ �� PHYCFGR State ---- PHY״̬
*************************************************/
uint8_t Get_PHYCFGR(void)
{
    return W5500_Read_Byte(PHYCFGR, COMM_REG);
}

    /* ----- Socket Register Function ----- */

/************************************************
�������� �� Set_MR
��    �� �� ģʽ����
��    �� �� S ---- Socket number
            Value ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_Sn_MR( uint8_t S, uint8_t Value )
{
    W5500_Write_Byte(Sn_MR, Sn_REG(S), Value);
}

/************************************************
�������� �� Get_Sn_CR
��    �� �� �� Socket����
��    �� �� S ---- Socket number
�� �� ֵ �� Sn_IR State ---- �ж�״̬
*************************************************/
uint8_t Get_Sn_CR( uint8_t S )
{
    return W5500_Read_Byte(Sn_CR, Sn_REG(S));
}

/************************************************
�������� �� Set_Sn_CR
��    �� �� д Socket����
��    �� �� S ---- Socket number
            Value ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_Sn_CR( uint8_t S, uint8_t Value )
{
    W5500_Write_Byte(Sn_CR, Sn_REG(S), Value);
}

/************************************************
�������� �� Get_Sn_IR
��    �� �� �� Sn_IR�ж�
��    �� �� S ---- Socket number
�� �� ֵ �� Sn_IR State ---- �ж�״̬
*************************************************/
uint8_t Get_Sn_IR( uint8_t S )
{
    return W5500_Read_Byte(Sn_IR, Sn_REG(S));
}

/************************************************
�������� �� Set_Sn_IR
��    �� �� д Sn_IR�ж�
��    �� �� S ---- Socket number
            Value ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_Sn_IR( uint8_t S, uint8_t Value )
{
    W5500_Write_Byte(Sn_IR, Sn_REG(S), Value);
}

/************************************************
�������� �� Get_Sn_SR
��    �� �� �� Sn_SR״̬
��    �� �� S ---- Socket number
�� �� ֵ �� Sn_SR State ---- �ж�״̬
*************************************************/
uint8_t Get_Sn_SR( uint8_t S )
{
    return W5500_Read_Byte(Sn_SR, Sn_REG(S));
}

/************************************************
�������� �� Set_Sn_PORT
��    �� �� ���� Socket n Դ�˿�
��    �� �� S ---- Socket number
            Value ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_Sn_PORT( uint8_t S, uint16_t Value )
{
    W5500_Write_Byte(Sn_PORT0, Sn_REG(S), (uint8_t)(Value & 0xFF00) >> 8);
    W5500_Write_Byte(Sn_PORT0, Sn_REG(S), (uint8_t)(Value & 0x00FF));
}

/************************************************
�������� �� Set_Sn_DHAR
��    �� �� ���� Socket n Ŀ�� MAC��ַ
��    �� �� S ---- Socket number
            Value ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_Sn_DHAR( uint8_t S, const uint8_t *pValue )
{
    W5500_Write_Buf(Sn_DHAR0, Sn_REG(S), pValue, 6);
}

/************************************************
�������� �� Set_Sn_DIPR
��    �� �� ���� Socket n Ŀ�� IP��ַ
��    �� �� S ---- Socket number
            Value ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_Sn_DIPR( uint8_t S, const uint8_t *pValue )
{
    W5500_Write_Buf(Sn_DIPR0, Sn_REG(S), pValue, 4);
}

/************************************************
�������� �� Set_Sn_DPORT
��    �� �� ���� Socket n Ŀ�Ķ˿�
��    �� �� S ---- Socket number
            Value ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_Sn_DPORT( uint8_t S, uint16_t Value )
{
    W5500_Write_Byte(Sn_DPORT0, Sn_REG(S), (uint8_t)(Value & 0xFF00) >> 8);
    W5500_Write_Byte(Sn_DPORT0, Sn_REG(S), (uint8_t)(Value & 0x00FF));
}

/************************************************
�������� �� Set_Sn_MSSR
��    �� �� ���� TCP / UPD ������䵥Ԫ
��    �� �� S ---- Socket number
            Value ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_Sn_MSSR( uint8_t S, uint16_t Value )
{
    W5500_Write_Byte(Sn_MSSR0, Sn_REG(S), (uint8_t)(Value & 0xFF00) >> 8);
    W5500_Write_Byte(Sn_MSSR0, Sn_REG(S), (uint8_t)(Value & 0x00FF));
}

/************************************************
�������� �� Set_Sn_TTL
��    �� �� ���� Socket n IP����ʱ��
��    �� �� S ---- Socket number
            Value ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_Sn_TTL( uint8_t S, uint8_t Value )
{
    W5500_Write_Byte(Sn_IR, Sn_REG(S), Value);
}

/************************************************
�������� �� Get_Sn_Tx_FSR
��    �� �� �� Socket n ���з��ͻ���
��    �� �� S ---- Socket number
�� �� ֵ �� ��
*************************************************/
uint16_t Get_Sn_Tx_FSR( uint8_t S )
{
    uint16_t value = 0;

    value = W5500_Read_Byte(Sn_TX_FSR0, Sn_REG(S));
    value = (value << 8) + W5500_Read_Byte(Sn_TX_FSR1, Sn_REG(S));

    return value;
}

/************************************************
�������� �� Get_Sn_Rx_RSR
��    �� �� �� Socket n ���н��ջ���
��    �� �� S ---- Socket number
�� �� ֵ �� ��
*************************************************/
uint16_t Get_Sn_Rx_RSR( uint8_t S )
{
    uint16_t value = 0;

    value = W5500_Read_Byte(Sn_RX_RSR0, Sn_REG(S));
    value = (value << 8) + W5500_Read_Byte(Sn_RX_RSR1, Sn_REG(S));

    return value;
}

/************************************************
�������� �� Set_Sn_Tx_WR
��    �� �� д Socket n ���ն�ָ��Ĵ���
��    �� �� S ---- Socket number
            Value ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_Sn_Tx_WR( uint8_t S, uint16_t Value )
{
    W5500_Write_Byte(Sn_RX_WR0, Sn_REG(S), (uint8_t)(Value & 0xFF00) >> 8);
    W5500_Write_Byte(Sn_RX_WR1, Sn_REG(S), (uint8_t)(Value & 0x00FF));
}

/************************************************
�������� �� Get_Sn_Tx_WR
��    �� �� �� Socket n ���Ͷ�ָ��Ĵ���
��    �� �� S ---- Socket number
�� �� ֵ �� ��
*************************************************/
uint16_t Get_Sn_Tx_WR( uint8_t S )
{
    uint16_t value = 0;

    value = W5500_Read_Byte(Sn_RX_WR0, Sn_REG(S));
    value = (value << 8) + W5500_Read_Byte(Sn_RX_WR1, Sn_REG(S));

    return value;
}

/************************************************
�������� �� Set_Sn_Rx_RD
��    �� �� д Socket n ���ն�ָ��Ĵ���
��    �� �� S ---- Socket number
            Value ---- ����
�� �� ֵ �� ��
*************************************************/
void Set_Sn_Rx_RD( uint8_t S, uint16_t Value )
{
    W5500_Write_Byte(Sn_RX_RD0, Sn_REG(S), (uint8_t)(Value & 0xFF00) >> 8);
    W5500_Write_Byte(Sn_RX_RD1, Sn_REG(S), (uint8_t)(Value & 0x00FF));
}

/************************************************
�������� �� Get_Sn_Rx_RD
��    �� �� �� Socket n ���ն�ָ��Ĵ���
��    �� �� S ---- Socket number
�� �� ֵ �� ��
*************************************************/
uint16_t Get_Sn_Rx_RD( uint8_t S )
{
    uint16_t value = 0;

    value = W5500_Read_Byte(Sn_RX_RD0, Sn_REG(S));
    value = (value << 8) + W5500_Read_Byte(Sn_RX_RD1, Sn_REG(S));

    return value;
}

/************************************************
�������� �� Socket_Buf_Init
��    �� �� ����ʹ�õ�ͨ�����÷��ͺͽ��ջ�������С
��    �� �� S ---- Socket number
�� �� ֵ �� ��
*************************************************/
void Socket_Buf_Init( uint8_t *Tx_size, uint8_t *Rx_size )
{
    uint8_t i;
    uint16_t ssum = 0,rsum = 0;

    for (i = 0 ;i < MAX_SOCK_NUM;i++)       // ��ÿ��ͨ������ Tx�� Rx�ڴ�Ĵ�С
    {
        W5500_Write_Byte(Sn_RXBUF_SIZE, Sn_REG(i), Tx_size[i]);
        W5500_Write_Byte(Sn_TXBUF_SIZE, Sn_REG(i), Rx_size[i]);

#ifdef _USART_DEBUG
        printf("Tx_size[%d]: %d, Sn_TXBUF_SIZE = %d\r\n",i, Tx_size[i], IINCHIP_READ(Sn_TXMEM_SIZE(i)));
        printf("Rx_size[%d]: %d, Sn_RXBUF_SIZE = %d\r\n",i, Rx_size[i], IINCHIP_READ(Sn_RXMEM_SIZE(i)));

#endif /* _USART_DEBUG */

        SSIZE[i] = (uint16_t)(0);
        RSIZE[i] = (uint16_t)(0);

        if (ssum <= 16384)
        {
            SSIZE[i] = (uint16_t)Tx_size[i]*(1024);
        }

        if (rsum <= 16384)
        {
            RSIZE[i]=(uint16_t)Rx_size[i]*(1024);
        }
        ssum += SSIZE[i];
        rsum += RSIZE[i];
    }

}


/*---------------------------- END OF FILE ----------------------------*/


