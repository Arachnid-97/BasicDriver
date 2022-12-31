#include "w5500.h"
#include "w5500_reg.h"


static W5500_Drv_Hooks device = {NULL, NULL, NULL, NULL};

void W5500_InitHooks(W5500_Drv_Hooks* Hooks)
{
    if(Hooks == NULL)
        return;

    device.write_byte_fn = Hooks->write_byte_fn;
    device.write_buf_fn = Hooks->write_buf_fn;

    device.read_byte_fn = Hooks->read_byte_fn;
    device.read_buf_fn = Hooks->read_buf_fn;
}

W5500_Drv_Hooks* Get_W5500_Hooks(void)
{
    if(device.write_byte_fn == NULL
        || device.write_buf_fn == NULL
        || device.read_byte_fn == NULL
        || device.read_buf_fn == NULL)
        return NULL;

    return &device;
}

    /* ----- Common Register Function ----- */

/************************************************
函数名称 ： Set_MR
功    能 ： 模式设置
参    数 ： Value ---- 数据
返 回 值 ： 无
*************************************************/
void Set_MR( uint8_t Value )
{
    device.write_byte_fn(MR, COMM_REG, Value);
}

/************************************************
函数名称 ： Set_GAR
功    能 ： 设置网关 IP地址
参    数 ： pData ---- 数据
返 回 值 ： 无
*************************************************/
void Set_GAR( const uint8_t *pData )
{
    device.write_buf_fn(GAR0, COMM_REG, pData, 4);
}

/************************************************
函数名称 ： Get_GAR
功    能 ： 读区网关 IP地址
参    数 ： pData ---- 数据
返 回 值 ： 无
*************************************************/
void Get_GAR( uint8_t *pData )
{
    device.read_buf_fn(GAR0, COMM_REG, pData, 4);
}

/************************************************
函数名称 ： Set_SUBR
功    能 ： 设置子网掩码地址
参    数 ： pData ---- 数据
返 回 值 ： 无
*************************************************/
void Set_SUBR( const uint8_t *pData )
{
    device.write_buf_fn(SUBR0, COMM_REG, pData, 4);
}

/************************************************
函数名称 ： Get_SUBR
功    能 ： 读取子网掩码地址
参    数 ： pData ---- 数据
返 回 值 ： 无
*************************************************/
void Get_SUBR( uint8_t *pData )
{
    device.read_buf_fn(SUBR0, COMM_REG, pData, 4);
}

/************************************************
函数名称 ： Set_SHAR
功    能 ： 设置源 MAC地址
参    数 ： pData ---- 数据
返 回 值 ： 无
*************************************************/
void Set_SHAR( const uint8_t *pData )
{
    device.write_buf_fn(SHAR0, COMM_REG, pData, 6);
}

/************************************************
函数名称 ： Get_SHAR
功    能 ： 读取源 MAC地址
参    数 ： pData ---- 数据
返 回 值 ： 无
*************************************************/
void Get_SHAR( uint8_t *pData )
{
    device.read_buf_fn(SHAR0, COMM_REG, pData, 6);
}

/************************************************
函数名称 ： Set_SIPR
功    能 ： 设置源 IP地址
参    数 ： pData ---- 数据
返 回 值 ： 无
*************************************************/
void Set_SIPR( const uint8_t *pData )
{
    device.write_buf_fn(SIPR0, COMM_REG, pData, 4);
}

/************************************************
函数名称 ： Get_SIPR
功    能 ： 读取源 IP地址
参    数 ： pData ---- 数据
返 回 值 ： 无
*************************************************/
void Get_SIPR( uint8_t *pData )
{
    device.read_buf_fn(SIPR0, COMM_REG, pData, 4);
}

/************************************************
函数名称 ： Get_IR
功    能 ： 读 IR中断
参    数 ： 无
返 回 值 ： IR State ---- 中断状态
*************************************************/
uint8_t Get_IR(void)
{
    return device.read_byte_fn(IR, COMM_REG);
}

/************************************************
函数名称 ： Clean_IR
功    能 ： 清 IR中断
参    数 ： Mask >>>> IR register values
返 回 值 ： 无
*************************************************/
void Clean_IR( uint8_t Mask )
{
    device.write_byte_fn(IR, COMM_REG, ~Mask | Get_IR());
}

/************************************************
函数名称 ： Set_RTR
功    能 ： 设置重试时间值
参    数 ： Value ---- 时间值
返 回 值 ： 无
*************************************************/
void Set_RTR( uint16_t Value )
{
    device.write_byte_fn(RTR0, COMM_REG, (uint8_t)((Value & 0xFF00) >> 8));
    device.write_byte_fn(RTR1, COMM_REG, (uint8_t)(Value & 0x00FF));
}

/************************************************
函数名称 ： Set_RCR
功    能 ： 设置重试计数值
参    数 ： Value ---- 数据
返 回 值 ： 无
*************************************************/
void Set_RCR( uint8_t Value )
{
    device.write_byte_fn(RCR, COMM_REG, Value);
}

/************************************************
函数名称 ： Get_PHYCFGR
功    能 ： 读取 PHYCFGR配置
参    数 ： 无
返 回 值 ： PHYCFGR State ---- PHY状态
*************************************************/
#include <stdio.h>
uint8_t Get_PHYCFGR(void)
{
    return device.read_byte_fn(PHYCFGR, COMM_REG);
}

/************************************************
函数名称 ： Get_ETH_Link
功    能 ： 获取网络连接状态
参    数 ： 无
返 回 值 ： ETH Link State ---- 连接状态
*************************************************/
uint8_t Get_ETH_Link(void)
{
    return (Get_PHYCFGR() & LINK);
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
    device.write_byte_fn(Sn_MR, Sn_REG(S), Value);
}

/************************************************
函数名称 ： Get_Sn_CR
功    能 ： 读 Socket配置
参    数 ： S ---- Socket number
返 回 值 ： Sn_IR State ---- 中断状态
*************************************************/
uint8_t Get_Sn_CR( uint8_t S )
{
    return device.read_byte_fn(Sn_CR, Sn_REG(S));
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
    device.write_byte_fn(Sn_CR, Sn_REG(S), Value);
    while(Get_Sn_CR(S));    // Wait to process the command
}

/************************************************
函数名称 ： Get_Sn_IR
功    能 ： 读 Sn_IR中断
参    数 ： S ---- Socket number
返 回 值 ： Sn_IR State ---- 中断状态
*************************************************/
uint8_t Get_Sn_IR( uint8_t S )
{
    return device.read_byte_fn(Sn_IR, Sn_REG(S));
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
    device.write_byte_fn(Sn_IR, Sn_REG(S), Value);
}

/************************************************
函数名称 ： Get_Sn_SR
功    能 ： 读 Sn_SR状态
参    数 ： S ---- Socket number
返 回 值 ： Sn_SR State ---- 中断状态
*************************************************/
uint8_t Get_Sn_SR( uint8_t S )
{
    return device.read_byte_fn(Sn_SR, Sn_REG(S));
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
    device.write_byte_fn(Sn_PORT0, Sn_REG(S), (uint8_t)((Value & 0xFF00) >> 8));
    device.write_byte_fn(Sn_PORT1, Sn_REG(S), (uint8_t)(Value & 0x00FF));
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
    device.write_buf_fn(Sn_DHAR0, Sn_REG(S), pValue, 6);
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
    device.write_buf_fn(Sn_DIPR0, Sn_REG(S), pValue, 4);
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
    device.write_byte_fn(Sn_DPORT0, Sn_REG(S), (uint8_t)((Value & 0xFF00) >> 8));
    device.write_byte_fn(Sn_DPORT1, Sn_REG(S), (uint8_t)(Value & 0x00FF));
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
    device.write_byte_fn(Sn_MSSR0, Sn_REG(S), (uint8_t)((Value & 0xFF00) >> 8));
    device.write_byte_fn(Sn_MSSR0, Sn_REG(S), (uint8_t)(Value & 0x00FF));
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
    device.write_byte_fn(Sn_IR, Sn_REG(S), Value);
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

    value = device.read_byte_fn(Sn_TX_FSR0, Sn_REG(S));
    value = (value << 8) + device.read_byte_fn(Sn_TX_FSR1, Sn_REG(S));

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

    value = device.read_byte_fn(Sn_RX_RSR0, Sn_REG(S));
    value = (value << 8) + device.read_byte_fn(Sn_RX_RSR1, Sn_REG(S));

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
    device.write_byte_fn(Sn_TX_WR0, Sn_REG(S), (uint8_t)((Value & 0xFF00) >> 8));
    device.write_byte_fn(Sn_TX_WR1, Sn_REG(S), (uint8_t)(Value & 0x00FF));
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

    value = device.read_byte_fn(Sn_TX_WR0, Sn_REG(S));
    value = (value << 8) + device.read_byte_fn(Sn_TX_WR1, Sn_REG(S));

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
    device.write_byte_fn(Sn_RX_RD0, Sn_REG(S), (uint8_t)((Value & 0xFF00) >> 8));
    device.write_byte_fn(Sn_RX_RD1, Sn_REG(S), (uint8_t)(Value & 0x00FF));
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

    value = device.read_byte_fn(Sn_RX_RD0, Sn_REG(S));
    value = (value << 8) + device.read_byte_fn(Sn_RX_RD1, Sn_REG(S));

    return value;
}


/*---------------------------- END OF FILE ----------------------------*/


