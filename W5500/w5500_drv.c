#include "w5500_drv.h"
#include <string.h>
#include "w5500.h"
#include "w5500_mode.h"
#include "bsp_uart.h"
//#define NDEBUG              //取消断言
#include <assert.h>         // 断言库


#define _USART_DEBUG

LocalNet_TypeDef    Local_Net;
RemoteNet_TypeDef   Socket_0;           // 请根据使用的 Socket端口定义结构体

uint16_t SSIZE[MAX_SOCK_NUM] = {0,0,0,0,0,0,0,0};     // Max Tx buffer
uint16_t RSIZE[MAX_SOCK_NUM] = {0,0,0,0,0,0,0,0};     // Max Rx buffer

/************************************************
函数名称 ： W5500_Delay_us
功    能 ： 软件毫秒延时
参    数 ： Count ---- 次数
返 回 值 ： 无
*************************************************/
static void W5500_Delay_us( uint32_t Count )
{
    while(Count)
    {
        Count--;
    }
}

/************************************************
函数名称 ： W5500_Delay_ms
功    能 ： 软件毫秒延时
参    数 ： Count ---- 次数
返 回 值 ： 无
*************************************************/

#if 1
void W5500_Delay_ms( uint32_t Count )
{
    uint16_t i;

    while(Count--)
    {
        for(i = 1240;i > 0;i--);        // 根据震荡时间为 i取值
    }
}

#endif

/************************************************
函数名称 ： W5500_Data_RW
功    能 ： W5500数据读写
参    数 ： Data ---- 数据
返 回 值 ： Data ---- 数据
*************************************************/
static uint8_t W5500_Data_RW( uint8_t Data )
{
    uint8_t i;

    W5500_SCLK(LOW);

    for(i = 0;i < 8;i++)
    {
		if(Data & 0x80)
		{
			W5500_MOSI(HIGH);
		}
		else
		{
			W5500_MOSI(LOW);
		}
		Data <<= 1;
        W5500_SCLK(HIGH);
        Data |= W5500_MISO;
        W5500_SCLK(LOW);
    }

    return Data;
}

/************************************************
函数名称 ： W5500_Write_Byte
功    能 ： W5500写一个字节
参    数 ： Address ---- 地址
            Control ---- 控制
			Value ---- 输入值
返 回 值 ： 无
*************************************************/
void W5500_Write_Byte( uint16_t Address, uint8_t Control, uint8_t Value )
{
    W5500_SCS(LOW);
	W5500_Data_RW((Address & 0xFF00) >> 8);
	W5500_Data_RW(Address & 0x00FF);
	W5500_Data_RW(Control | 0x04);
	W5500_Data_RW(Value);
    W5500_SCS(HIGH);
}

/************************************************
函数名称 ： W5500_Write_Buf
功    能 ： W5500写 N个字节
参    数 ： Address ---- 地址
            Control ---- 控制
			pBuf ---- 数据
            Len ---- 长度
返 回 值 ： 无
*************************************************/
void W5500_Write_Buf( uint16_t Address, uint8_t Control, const uint8_t *pBuf, uint16_t Len )
{
    assert(Len > 0);

    W5500_SCS(LOW);
	W5500_Data_RW((Address & 0xFF00) >> 8);
	W5500_Data_RW(Address & 0x00FF);
	W5500_Data_RW(Control | 0x04);
    while(Len--)
    {
        W5500_Data_RW(*pBuf++);
    }
    W5500_SCS(HIGH);
}

/************************************************
函数名称 ： W5500_Read_Byte
功    能 ： W5500读一个字节
参    数 ： Address ---- 地址
            Control ---- 控制
返 回 值 ： data ---- 数据
*************************************************/
uint8_t W5500_Read_Byte( uint16_t Address, uint8_t Control )
{
    uint8_t data = 0;

    W5500_SCS(LOW);
	W5500_Data_RW((Address & 0xFF00) >> 8);
	W5500_Data_RW(Address & 0x00FF);
	W5500_Data_RW(Control & ~0x04);
	data = W5500_Data_RW(0xFF);
    W5500_SCS(HIGH);

    return data;
}

/************************************************
函数名称 ： W5500_Read_Buf
功    能 ： W5500读 N个字节
参    数 ： Address ---- 地址
            Control ---- 控制
			pBuf ---- 数据
            Len ---- 长度
返 回 值 ： 无
*************************************************/
void W5500_Read_Buf( uint16_t Address, uint8_t Control, uint8_t *pBuf, uint16_t Len )
{
    assert(Len > 0);

    W5500_SCS(LOW);
	W5500_Data_RW((Address & 0xFF00) >> 8);
	W5500_Data_RW(Address & 0x00FF);
	W5500_Data_RW(Control & ~0x04);
    while(Len--)
    {
        *pBuf = W5500_Data_RW(0xFF);
        pBuf++;
    }
    W5500_SCS(HIGH);
}

/************************************************
函数名称 ： W5500_Reset
功    能 ： W5500重置
参    数 ： 无
返 回 值 ： 无
*************************************************/
static void W5500_Reset(void)
{
    W5500_RST(LOW);
    W5500_Delay_us(800);        // 复位时钟周期至少 500us
    W5500_RST(HIGH);
    W5500_Delay_us(100);
    while(0 == (Get_PHYCFGR() & LINK));    //等待以太网连接完成
}

/************************************************
函数名称 ： Detect_Gateway
功    能 ： 检查网关服务器
参    数 ： 无
返 回 值 ： 无
*************************************************/

#if 0
static void Detect_Gateway(void)
{

}

#endif

/************************************************
函数名称 ： Load_Net_Parameters
功    能 ： 装载网络参数
参    数 ： 无
返 回 值 ： 无
*************************************************/
static void Load_Net_Parameters(void)
{
    /* 定义本地端 IP信息 */
    uint8_t local_ip[4] = {192,168,1,200};				    /* 定义 W5500默认IP地址 */
    uint8_t subnet[4] = {255,255,255,0};				    /* 定义 W5500默认子网掩码 */
    uint8_t gateway[4] = {192,168,1,1};					    /* 定义 W5500默认网关 */
    uint8_t dns_server[4] = {114,114,114,114};			    /* 定义 W5500默认 DNS */
    uint8_t local_mac[6] = {0x00,0x08,0xdc,0x11,0x11,0x11}; /* 定义 W5500默认 MAC地址 */
    uint16_t local_port = 5000;	                       	    /* 定义本地端口 */

    /* 定义远程端 IP信息 */
    uint8_t remote_ip[4] = {192,168,1,100};				    /* 远程 IP地址 */
    uint8_t remote_mac[6] = {0x00,0x06,0x5D,0x18,0x02,0x11};/* 远程 MAC地址 */
    uint16_t remote_port = 8000;						    /* 远程端口号 */

    /* 配置信息 */
    memcpy(Local_Net.LIp, local_ip, 4);
    memcpy(Local_Net.Sub, subnet, 4);
    memcpy(Local_Net.Gw, gateway, 4);
    memcpy(Local_Net.Dns, dns_server, 4);
    memcpy(Local_Net.LMac, local_mac, 6);
    Local_Net.LPort = local_port;

    memcpy(Socket_0.RIp, remote_ip, 4);
    memcpy(Socket_0.RMac, remote_mac, 6);
    Socket_0.RPort = remote_port;
}

/************************************************
函数名称 ： W5500_Config
功    能 ： W5500配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
static void W5500_Config(void)
{
    /* W5500_SCS */
    PD_DDR_DDR1 = 1;
    PD_CR1_C11 = 1;
    PD_CR2_C21 = 0;


    /* W5500_INT */
    PC_DDR_DDR5 = 0;
    PC_CR1_C15 = 1;
    PC_CR2_C25 = 0;


    /* W5500_RST */
    PC_DDR_DDR6 = 1;
    PC_CR1_C16 = 1;
    PC_CR2_C26 = 0;


    /* W5500_SCLK */
    PC_DDR_DDR7 = 1;
    PC_CR1_C17 = 1;
    PC_CR2_C27 = 1;


    /* W5500_MISO */
    PD_DDR_DDR3 = 0;
    PD_CR1_C13 = 1;
    PD_CR2_C23 = 0;


    /* W5500_MOSI */
    PD_DDR_DDR2 = 1;
    PD_CR1_C12 = 1;
    PD_CR2_C22 = 1;

}

/************************************************
函数名称 ： W5500_Init
功    能 ： W5500初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void W5500_Init(void)
{
    uint8_t temp[4] = {0};
    uint8_t txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};   // tx buffer set K bits
    uint8_t rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};   // rx buffet set K bits


    W5500_Config();
    W5500_Reset();                                      // 硬复位 W5500
    Load_Net_Parameters();

    Set_GAR(Local_Net.Gw);                              // 设置网关(Gateway)的IP地址
    Set_SUBR(Local_Net.Sub);                            // 设置子网掩码(MASK)值
    Set_SHAR(Local_Net.LMac);                           // 设置源 MAC地址
    Set_SIPR(Local_Net.LIp);                            // 设置源 IP地址

#ifdef _USART_DEBUG
    Get_SIPR (temp);
    UART1_SendString(temp, 4);
//    printf("IP : %d.%d.%d.%d\r\n", temp[0],temp[1],temp[2],temp[3]);

    Get_SUBR(temp);
    UART1_SendString(temp, 4);
//    printf("MASK : %d.%d.%d.%d\r\n", temp[0],temp[1],temp[2],temp[3]);

    Get_GAR(temp);
    UART1_SendString(temp, 4);
//    printf("Gateway : %d.%d.%d.%d\r\n", temp[0],temp[1],temp[2],temp[3]);

#endif /* _USART_DEBUG */

    Socket_Buf_Init(txsize, rxsize);                    // 设置发送缓冲区和接收缓冲区的大小
    Set_RTR(0x07d0);                                    // 设置重试时间，每一单位数值为100微秒，默认为2000(200ms)
    Set_RCR(8);                                         // 设置重试次数，默认为8次，如果重发的次数超过设定值,则产生超时中断(相关的端口中断寄存器中的Sn_IR 超时位(TIMEOUT)置“1”)

//    if(Socket_Config(SOCK_TCP_C_PORT, TCP_SERVER, Local_Net.LPort) == TCP_CLIENT );
//    {
//
//    }
}


/*---------------------------- END OF FILE ----------------------------*/


