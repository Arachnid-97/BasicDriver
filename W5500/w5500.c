#include "w5500.h"
#include "w5500_drv.h"


    /* ----- Common Register Function ----- */

/************************************************
函数名称 ： Set_MR
功    能 ： 模式设置
参    数 ： Value ---- 数据
返 回 值 ： 无
*************************************************/
void Set_MR( uint8_t Value )
{
    W5500_Write_Byte(MR, COMM_REG, Value);
}

/************************************************
函数名称 ： Set_GAR
功    能 ： 设置网关 IP地址
参    数 ： pData ---- 数据
返 回 值 ： 无
*************************************************/
void Set_GAR( const uint8_t *pData )
{
    W5500_Write_Buf(GAR0, COMM_REG, pData, 4);
}

/************************************************
函数名称 ： Get_GAR
功    能 ： 读区网关 IP地址
参    数 ： pData ---- 数据
返 回 值 ： 无
*************************************************/
void Get_GAR( uint8_t *pData )
{
    W5500_Read_Buf(GAR0, COMM_REG, pData, 4);
}

/************************************************
函数名称 ： Set_SUBR
功    能 ： 设置子网掩码地址
参    数 ： pData ---- 数据
返 回 值 ： 无
*************************************************/
void Set_SUBR( const uint8_t *pData )
{
    W5500_Write_Buf(SUBR0, COMM_REG, pData, 4);
}

/************************************************
函数名称 ： Get_SUBR
功    能 ： 读取子网掩码地址
参    数 ： pData ---- 数据
返 回 值 ： 无
*************************************************/
void Get_SUBR( uint8_t *pData )
{
    W5500_Read_Buf(SUBR0, COMM_REG, pData, 4);
}

/************************************************
函数名称 ： Set_SHAR
功    能 ： 设置源 MAC地址
参    数 ： pData ---- 数据
返 回 值 ： 无
*************************************************/
void Set_SHAR( const uint8_t *pData )
{
    W5500_Write_Buf(SHAR0, COMM_REG, pData, 6);
}

/************************************************
函数名称 ： Get_SHAR
功    能 ： 读取源 MAC地址
参    数 ： pData ---- 数据
返 回 值 ： 无
*************************************************/
void Get_SHAR( uint8_t *pData )
{
    W5500_Read_Buf(SHAR0, COMM_REG, pData, 6);
}

/************************************************
函数名称 ： Set_SIPR
功    能 ： 设置源 IP地址
参    数 ： pData ---- 数据
返 回 值 ： 无
*************************************************/
void Set_SIPR( const uint8_t *pData )
{
    W5500_Write_Buf(SIPR0, COMM_REG, pData, 4);
}

/************************************************
函数名称 ： Get_SIPR
功    能 ： 读取源 IP地址
参    数 ： pData ---- 数据
返 回 值 ： 无
*************************************************/
void Get_SIPR( uint8_t *pData )
{
    W5500_Read_Buf(SIPR0, COMM_REG, pData, 4);
}

/************************************************
函数名称 ： Get_IR
功    能 ： 读 IR中断
参    数 ： 无
返 回 值 ： IR State ---- 中断状态
*************************************************/
uint8_t Get_IR(void)
{
    return W5500_Read_Byte(IR, COMM_REG);
}

/************************************************
函数名称 ： Clean_IR
功    能 ： 清 IR中断
参    数 ： Mask >>>> IR register values
返 回 值 ： 无
*************************************************/
void Clean_IR( uint8_t Mask )
{
    W5500_Write_Byte(IR, COMM_REG, ~Mask | Get_IR());
}

/************************************************
函数名称 ： Set_RTR
功    能 ： 设置重试时间值
参    数 ： Value ---- 时间值
返 回 值 ： 无
*************************************************/
void Set_RTR( uint16_t Value )
{
    W5500_Write_Byte(RTR0, COMM_REG, (uint8_t)(Value & 0xFF00) >> 8);
    W5500_Write_Byte(RTR1, COMM_REG, (uint8_t)(Value & 0x00FF));
}

/************************************************
函数名称 ： Set_RCR
功    能 ： 设置重试计数值
参    数 ： Value ---- 数据
返 回 值 ： 无
*************************************************/
void Set_RCR( uint8_t Value )
{
    W5500_Write_Byte(RCR, COMM_REG, Value);
}

/************************************************
函数名称 ： Get_PHYCFGR
功    能 ： 读取 PHYCFGR配置
参    数 ： 无
返 回 值 ： PHYCFGR State ---- PHY状态
*************************************************/
uint8_t Get_PHYCFGR(void)
{
    return W5500_Read_Byte(PHYCFGR, COMM_REG);
}

    /* ----- Socket Register Function ----- */

/************************************************
函数名称 ： Set_MR
功    能 ： 模式设置
参    数 ： S ---- Socket number
            Value ---- 数据
返 回 值 ： 无
*************************************************/
void Set_Sn_MR( uint8_t S, uint8_t Value )
{
    W5500_Write_Byte(Sn_MR, Sn_REG(S), Value);
}

/************************************************
函数名称 ： Get_Sn_CR
功    能 ： 读 Socket配置
参    数 ： S ---- Socket number
返 回 值 ： Sn_IR State ---- 中断状态
*************************************************/
uint8_t Get_Sn_CR( uint8_t S )
{
    return W5500_Read_Byte(Sn_CR, Sn_REG(S));
}

/************************************************
函数名称 ： Set_Sn_CR
功    能 ： 写 Socket配置
参    数 ： S ---- Socket number
            Value ---- 数据
返 回 值 ： 无
*************************************************/
void Set_Sn_CR( uint8_t S, uint8_t Value )
{
    W5500_Write_Byte(Sn_CR, Sn_REG(S), Value);
}

/************************************************
函数名称 ： Get_Sn_IR
功    能 ： 读 Sn_IR中断
参    数 ： S ---- Socket number
返 回 值 ： Sn_IR State ---- 中断状态
*************************************************/
uint8_t Get_Sn_IR( uint8_t S )
{
    return W5500_Read_Byte(Sn_IR, Sn_REG(S));
}

/************************************************
函数名称 ： Set_Sn_IR
功    能 ： 写 Sn_IR中断
参    数 ： S ---- Socket number
            Value ---- 数据
返 回 值 ： 无
*************************************************/
void Set_Sn_IR( uint8_t S, uint8_t Value )
{
    W5500_Write_Byte(Sn_IR, Sn_REG(S), Value);
}

/************************************************
函数名称 ： Get_Sn_SR
功    能 ： 读 Sn_SR状态
参    数 ： S ---- Socket number
返 回 值 ： Sn_SR State ---- 中断状态
*************************************************/
uint8_t Get_Sn_SR( uint8_t S )
{
    return W5500_Read_Byte(Sn_SR, Sn_REG(S));
}

/************************************************
函数名称 ： Set_Sn_PORT
功    能 ： 设置 Socket n 源端口
参    数 ： S ---- Socket number
            Value ---- 数据
返 回 值 ： 无
*************************************************/
void Set_Sn_PORT( uint8_t S, uint16_t Value )
{
    W5500_Write_Byte(Sn_PORT0, Sn_REG(S), (uint8_t)(Value & 0xFF00) >> 8);
    W5500_Write_Byte(Sn_PORT0, Sn_REG(S), (uint8_t)(Value & 0x00FF));
}

/************************************************
函数名称 ： Set_Sn_DHAR
功    能 ： 设置 Socket n 目的 MAC地址
参    数 ： S ---- Socket number
            Value ---- 数据
返 回 值 ： 无
*************************************************/
void Set_Sn_DHAR( uint8_t S, const uint8_t *pValue )
{
    W5500_Write_Buf(Sn_DHAR0, Sn_REG(S), pValue, 6);
}

/************************************************
函数名称 ： Set_Sn_DIPR
功    能 ： 设置 Socket n 目的 IP地址
参    数 ： S ---- Socket number
            Value ---- 数据
返 回 值 ： 无
*************************************************/
void Set_Sn_DIPR( uint8_t S, const uint8_t *pValue )
{
    W5500_Write_Buf(Sn_DIPR0, Sn_REG(S), pValue, 4);
}

/************************************************
函数名称 ： Set_Sn_DPORT
功    能 ： 设置 Socket n 目的端口
参    数 ： S ---- Socket number
            Value ---- 数据
返 回 值 ： 无
*************************************************/
void Set_Sn_DPORT( uint8_t S, uint16_t Value )
{
    W5500_Write_Byte(Sn_DPORT0, Sn_REG(S), (uint8_t)(Value & 0xFF00) >> 8);
    W5500_Write_Byte(Sn_DPORT0, Sn_REG(S), (uint8_t)(Value & 0x00FF));
}

/************************************************
函数名称 ： Set_Sn_MSSR
功    能 ： 设置 TCP / UPD 的最大传输单元
参    数 ： S ---- Socket number
            Value ---- 数据
返 回 值 ： 无
*************************************************/
void Set_Sn_MSSR( uint8_t S, uint16_t Value )
{
    W5500_Write_Byte(Sn_MSSR0, Sn_REG(S), (uint8_t)(Value & 0xFF00) >> 8);
    W5500_Write_Byte(Sn_MSSR0, Sn_REG(S), (uint8_t)(Value & 0x00FF));
}

/************************************************
函数名称 ： Set_Sn_TTL
功    能 ： 设置 Socket n IP生存时间
参    数 ： S ---- Socket number
            Value ---- 数据
返 回 值 ： 无
*************************************************/
void Set_Sn_TTL( uint8_t S, uint8_t Value )
{
    W5500_Write_Byte(Sn_IR, Sn_REG(S), Value);
}

/************************************************
函数名称 ： Get_Sn_Tx_FSR
功    能 ： 读 Socket n 空闲发送缓存
参    数 ： S ---- Socket number
返 回 值 ： 无
*************************************************/
uint16_t Get_Sn_Tx_FSR( uint8_t S )
{
    uint16_t value = 0;

    value = W5500_Read_Byte(Sn_TX_FSR0, Sn_REG(S));
    value = (value << 8) + W5500_Read_Byte(Sn_TX_FSR1, Sn_REG(S));

    return value;
}

/************************************************
函数名称 ： Get_Sn_Rx_RSR
功    能 ： 读 Socket n 空闲接收缓存
参    数 ： S ---- Socket number
返 回 值 ： 无
*************************************************/
uint16_t Get_Sn_Rx_RSR( uint8_t S )
{
    uint16_t value = 0;

    value = W5500_Read_Byte(Sn_RX_RSR0, Sn_REG(S));
    value = (value << 8) + W5500_Read_Byte(Sn_RX_RSR1, Sn_REG(S));

    return value;
}

/************************************************
函数名称 ： Set_Sn_Tx_WR
功    能 ： 写 Socket n 接收读指针寄存器
参    数 ： S ---- Socket number
            Value ---- 数据
返 回 值 ： 无
*************************************************/
void Set_Sn_Tx_WR( uint8_t S, uint16_t Value )
{
    W5500_Write_Byte(Sn_RX_WR0, Sn_REG(S), (uint8_t)(Value & 0xFF00) >> 8);
    W5500_Write_Byte(Sn_RX_WR1, Sn_REG(S), (uint8_t)(Value & 0x00FF));
}

/************************************************
函数名称 ： Get_Sn_Tx_WR
功    能 ： 读 Socket n 发送读指针寄存器
参    数 ： S ---- Socket number
返 回 值 ： 无
*************************************************/
uint16_t Get_Sn_Tx_WR( uint8_t S )
{
    uint16_t value = 0;

    value = W5500_Read_Byte(Sn_RX_WR0, Sn_REG(S));
    value = (value << 8) + W5500_Read_Byte(Sn_RX_WR1, Sn_REG(S));

    return value;
}

/************************************************
函数名称 ： Set_Sn_Rx_RD
功    能 ： 写 Socket n 接收读指针寄存器
参    数 ： S ---- Socket number
            Value ---- 数据
返 回 值 ： 无
*************************************************/
void Set_Sn_Rx_RD( uint8_t S, uint16_t Value )
{
    W5500_Write_Byte(Sn_RX_RD0, Sn_REG(S), (uint8_t)(Value & 0xFF00) >> 8);
    W5500_Write_Byte(Sn_RX_RD1, Sn_REG(S), (uint8_t)(Value & 0x00FF));
}

/************************************************
函数名称 ： Get_Sn_Rx_RD
功    能 ： 读 Socket n 接收读指针寄存器
参    数 ： S ---- Socket number
返 回 值 ： 无
*************************************************/
uint16_t Get_Sn_Rx_RD( uint8_t S )
{
    uint16_t value = 0;

    value = W5500_Read_Byte(Sn_RX_RD0, Sn_REG(S));
    value = (value << 8) + W5500_Read_Byte(Sn_RX_RD1, Sn_REG(S));

    return value;
}

/************************************************
函数名称 ： Socket_Buf_Init
功    能 ： 根据使用的通道设置发送和接收缓冲区大小
参    数 ： S ---- Socket number
返 回 值 ： 无
*************************************************/
void Socket_Buf_Init( uint8_t *Tx_size, uint8_t *Rx_size )
{
    uint8_t i;
    uint16_t ssum = 0,rsum = 0;

    for (i = 0 ;i < MAX_SOCK_NUM;i++)       // 按每个通道设置 Tx和 Rx内存的大小
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


