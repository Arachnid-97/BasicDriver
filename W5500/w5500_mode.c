#include "w5500_mode.h"
#include "w5500.h"
#include "w5500_reg.h"
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"


static LocalNet_TypeDef    Local_Net;
static RemoteNet_TypeDef   Socket_0;           // 请根据使用的 Socket端口定义结构体

static W5500_Drv_Hooks* rw_Data = NULL;

// static _Bool Detect_Gateway( uint8_t S );

uint16_t SSIZE[MAX_SOCK_NUM] = {0,0,0,0,0,0,0,0};     // Max Tx buffer
uint16_t RSIZE[MAX_SOCK_NUM] = {0,0,0,0,0,0,0,0};     // Max Rx buffer

//uint8_t W5500_Tx_Buf[] = {0};
//uint8_t W5500_Rx_Buf[] = {0};

/************************************************
函数名称 ： W5500_TCP_Server
功    能 ： TCP服务端处理
参    数 ： 无
返 回 值 ： 无
*************************************************/
void W5500_TCP_Server(void)
{
    uint16_t len = 0;
    uint8_t buf[2048] = {0};

	switch(Get_Sn_SR(SOCK_TCP_S_PORT))										/*获取socket的状态*/
	{
		case SOCK_CLOSED:													/*socket处于关闭状态*/
            Socket_Config(SOCK_TCP_S_PORT, TCP_SERVER, Local_Net.LPort);/*打开socket*/
            break;

		case SOCK_INIT:														/*socket已初始化状态*/
            Socket_Listen(SOCK_TCP_S_PORT);								/*socket建立监听*/
            break;

		case SOCK_ESTABLISHED:										        /*socket处于连接建立状态*/
            if(Get_Sn_IR(SOCK_TCP_S_PORT) & Sn_IR_CON)
            {
                Set_Sn_IR(SOCK_TCP_S_PORT, Sn_IR_CON);					/*清除接收中断标志位*/
            }

            len = TCP_Buf_Recv(SOCK_TCP_S_PORT, buf, RSIZE[SOCK_TCP_S_PORT]);/*接收来自Client的数据*/
            if(len > 0)
            {
                buf[len] = '\0'; 											/*添加字符串结束符*/
                printf("TCP Server Receive %d: %s\n", len, buf);
                TCP_Buf_Send(SOCK_TCP_S_PORT, buf, len);					/*向Client发送数据*/
            }
            break;

		case SOCK_CLOSE_WAIT:												/*socket处于等待关闭状态*/
            Socket_Colse(SOCK_TCP_S_PORT);
            break;

        default:
            break;
	}

}

/************************************************
函数名称 ： W5500_TCP_Client
功    能 ： TCP客户端处理
参    数 ： 无
返 回 值 ： 无
*************************************************/
void W5500_TCP_Client(void)
{
    uint16_t len = 0;
    uint8_t buf[2048] = {0};

	switch(Get_Sn_SR(SOCK_TCP_C_PORT))								  		/*获取socket的状态*/
	{
		case SOCK_CLOSED:											        /*socket处于关闭状态*/
            Socket_Config(SOCK_TCP_C_PORT, TCP_CLIENT, Local_Net.LPort);
            break;

		case SOCK_INIT:													    /*socket处于初始化状态*/
            Socket_Connect(SOCK_TCP_C_PORT, Socket_0.RIp, Socket_0.RPort);/*socket连接服务器*/
            break;

		case SOCK_ESTABLISHED: 												/*socket处于连接建立状态*/
            if(Get_Sn_IR(SOCK_TCP_C_PORT) & Sn_IR_CON)
            {
                Set_Sn_IR(SOCK_TCP_C_PORT, Sn_IR_CON); 					/*清除接收中断标志位*/
            }

            len = TCP_Buf_Recv(SOCK_TCP_C_PORT, buf, RSIZE[SOCK_TCP_C_PORT]);/*接收来自Server的数据*/
            if(len > 0)
            {
                buf[len] = '\0';  										/*添加字符串结束符*/
                printf("TCP Client Receive %d: %s\n", len, buf);
                TCP_Buf_Send(SOCK_TCP_C_PORT, buf, len);					/*向Server发送数据*/
            }
            break;

		case SOCK_CLOSE_WAIT: 											    /*socket处于等待关闭状态*/
            Socket_Colse(SOCK_TCP_C_PORT);
            break;

        default:
            break;
	}
}

/************************************************
函数名称 ： W5500_UDP_Deal
功    能 ： UDP处理
参    数 ： 无
返 回 值 ： 无
*************************************************/
void W5500_UDP_Deal(void)
{
    uint16_t len = 0;
    uint8_t buf[2048] = {0};

	switch(Get_Sn_SR(SOCK_UDP_PORT))                                   /*获取socket的状态*/
	{
		case SOCK_CLOSED:                                              /*socket处于关闭状态*/
            Socket_Config(SOCK_UDP_PORT, UDP_MODE, Local_Net.LPort);/*初始化socket*/
            break;

		case SOCK_UDP:                                                 /*socket初始化完成*/
            len = UDP_Buf_Recv(SOCK_UDP_PORT, buf, RSIZE[SOCK_UDP_PORT], Socket_0.RIp, Socket_0.RPort);/*W5500接收发送来的数据*/
            if(len > 0)                                           /*接收到数据*/
            {                
                buf[len] = '\0';                                                    /*添加字符串结束符*/
                printf("UDP Receive %d: %s\n", len, buf);                                  /*打印接收缓存*/
                UDP_Buf_Send(SOCK_UDP_PORT, buf, len, Socket_0.RIp, Socket_0.RPort);/*W5500把接收到的数据发送给Remote*/
            }
            break;

        default:
            break;
	}

}

/************************************************
函数名称 ： Socket_Disconnect
功    能 ： Socket断开
参    数 ： S ---- Socket number
返 回 值 ： 无
*************************************************/
void Socket_Disconnect( uint8_t S )
{
    Set_Sn_CR(S, Sn_CR_DISCON);
    while(Get_Sn_SR(S) != SOCK_CLOSED);     // 等待 Socket断开
}

/************************************************
函数名称 ： Socket_Colse
功    能 ： Socket关闭
参    数 ： S ---- Socket number
返 回 值 ： 无
*************************************************/
void Socket_Colse( uint8_t S )
{
    Set_Sn_CR(S, Sn_CR_CLOSE);
    while(Get_Sn_SR(S) != SOCK_CLOSED);     // 等待 Socket关闭
    Set_Sn_IR(S, 0xFF);                     // 清 Socket所有中断
}

/************************************************
函数名称 ： TCP_Buf_Recv
功    能 ： TCP数据接收
参    数 ： S ---- Socket number
            pBuf ---- 数据
返 回 值 ： ret ---- 成功接收的长度
*************************************************/
uint16_t TCP_Buf_Recv( uint8_t S, uint8_t *pBuf, uint16_t Len )
{
    uint8_t status = 0;
    uint16_t freesize = 0;
    uint16_t addr = 0;
    uint16_t ret = 0;

    if(Get_Sn_IR(S) & Sn_IR_RECV)
    {
        Set_Sn_IR(S, Sn_IR_RECV);					/*清除接收中断标志位*/

        freesize = Get_Sn_Rx_RSR(S);
        status = Get_Sn_SR(S);
        if((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT))
        {
            Socket_Colse(S);
            printf("TCP Receive Disconnect!!!\n");
            ret = 0;
            return ret;
        }

        ret = freesize > Len ? Len : freesize;
        if(0 == ret)
        {
            return ret;
        }

        addr = Get_Sn_Rx_RD(S);

        rw_Data->read_buf_fn(addr, Sn_RX_BUFFER(S), pBuf, ret); // 读取数据

        addr += ret;
        Set_Sn_Rx_RD(S, addr);
        Set_Sn_CR(S, Sn_CR_RECV);
    }

    return ret;
}

/************************************************
函数名称 ： TCP_Buf_Send
功    能 ： TCP数据发送
参    数 ： S ---- Socket number
            pBuf ---- 数据
            Len ---- 长度
返 回 值 ： ret ---- 成功发送的长度
*************************************************/
uint16_t TCP_Buf_Send( uint8_t S, const uint8_t *pBuf, uint16_t Len )
{
    uint8_t status = 0;
    uint16_t freesize = 0;
    uint16_t addr = 0;
    uint16_t ret = 0;

    if(0 == Len)
    {
        return ret;
    }

    ret = Len > SSIZE[S] ? SSIZE[S] : Len;

    do{
        freesize = Get_Sn_Tx_FSR(S);
        status = Get_Sn_SR(S);
        if((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT))
        {
            Socket_Colse(S);
            printf("TCP Send Disconnect!!!\n");
            ret = 0;
            return ret;
        }
    }while(freesize < ret);  

    addr = Get_Sn_Tx_WR(S);

    rw_Data->write_buf_fn(addr, Sn_TX_BUFFER(S), pBuf, ret);  // 写入数据

    addr += ret;
    Set_Sn_Tx_WR(S, addr);
    Set_Sn_CR(S, Sn_CR_SEND);

    while((Get_Sn_IR(S) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK)
    {
        if(Get_Sn_IR(S) & Sn_IR_TIMEOUT)
        {
            Set_Sn_IR(S, Sn_IR_TIMEOUT);

        // status = Get_Sn_SR(S);
        // if((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT))
        // {
            Socket_Colse(S);
            printf("TCP Send Timeout!!!\n");
            ret = 0;
            return ret;
        }
    }
    Set_Sn_IR(S, Sn_IR_SEND_OK);

    return ret;
}

/************************************************
函数名称 ： UDP_Buf_Recv
功    能 ： UDP数据接收（协议：IP(4 Byte) + Port(2 Byte) + data_len(2 Byte) + data(n Byte)）
参    数 ： S ---- Socket number
            pBuf ---- 数据
            Len ---- 长度
            Addr ---- remote IP
            Port ---- remote port
返 回 值 ： ret ---- 成功接收的长度
*************************************************/
uint16_t UDP_Buf_Recv( uint8_t S, uint8_t *pBuf, uint16_t Len, uint8_t *Addr, uint16_t Port )
{
    uint8_t head[8] = {0};
    uint8_t status = 0;
    uint16_t freesize = 0;
    uint16_t addr = 0;
    uint16_t ret = 0;

    if(Get_Sn_IR(S) & Sn_IR_RECV)
    {
        Set_Sn_IR(S, Sn_IR_RECV);              /*清接收中断*/

        freesize = Get_Sn_Rx_RSR(S);
        status = Get_Sn_SR(S);
        if((status != SOCK_UDP) && (status != SOCK_CLOSE_WAIT))
        {
            Socket_Colse(S);
            printf("UDP Receive Disconnect!!!\n");
            ret = 0;
            return ret;
        }

        ret = freesize > Len ? Len : freesize;
        if(0 == ret)
        {
            return ret;
        }

        addr = Get_Sn_Rx_RD(S);
        rw_Data->read_buf_fn(addr, Sn_RX_BUFFER(S), head, 8);
        addr += 8;

        /* 获取远程 IP */
        *Addr = head[0];
        *(Addr + 1) = head[1];
        *(Addr + 2) = head[2];
        *(Addr + 3) = head[3];

        /* 获取远程端口 */
        Port = head[4];
        Port = (Port << 8) + head[5];

        /* 获取数据长度 */
        ret = head[6];
        ret = (ret << 8) + head[7];

        ret = Len > ret ? ret : Len;

        rw_Data->read_buf_fn(addr, Sn_RX_BUFFER(S), pBuf, ret);  // 读取数据

        addr += ret;
        Set_Sn_Rx_RD(S, addr);
        Set_Sn_CR(S, Sn_CR_RECV);
    }

    return ret;
}

/************************************************
函数名称 ： UDP_Buf_Send
功    能 ： UDP数据发送（协议：IP(4 Byte) + Port(2 Byte) + data_len(2 Byte) + data(n Byte)）
参    数 ： S ---- Socket number
            pBuf ---- 数据
            Len ---- 长度
            Addr ---- remote IP
            Port ---- remote port
返 回 值 ： 无
*************************************************/
uint16_t UDP_Buf_Send( uint8_t S, const uint8_t *pBuf, uint16_t Len, const uint8_t *Addr, uint16_t Port )
{
    uint8_t status = 0;
    uint16_t freesize = 0;
    uint16_t addr = 0;
    uint16_t ret = 0;

    if(((0 == *Addr) && (0 == *(Addr + 1)) \
        && (0 == *(Addr + 2)) && (0 == *(Addr + 3))) \
            || (0 == Port) || (0 == Len))
    {
        return 0;
    }

    /* 写入远程 IP、端口 */
    Set_Sn_DIPR(S, Addr);
    Set_Sn_DPORT(S, Port);

    ret = Len > SSIZE[S] ? SSIZE[S] : Len;

    do{
        freesize = Get_Sn_Tx_FSR(S);
        status = Get_Sn_SR(S);
        if((status != SOCK_UDP) && (status != SOCK_CLOSE_WAIT))
        {
            Socket_Colse(S);
            printf("UDP Send Disconnect!!!\n");
            ret = 0;
            return ret;
        }
    }while(freesize < ret);  

    addr = Get_Sn_Tx_WR(S);

    rw_Data->write_buf_fn(addr, Sn_TX_BUFFER(S), pBuf, ret);  // 写入数据

    addr += ret;
    Set_Sn_Tx_WR(S, addr);
    Set_Sn_CR(S, Sn_CR_SEND);

    while((Get_Sn_IR(S) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK)
    {
        if(Get_Sn_IR(S) & Sn_IR_TIMEOUT)
        {
            Set_Sn_IR(S, Sn_IR_TIMEOUT);
            Socket_Colse(S);
            printf("UDP Send Timeout!!!\n");
            ret = 0;
            return ret;
        }
    }
    Set_Sn_IR(S, Sn_IR_SEND_OK);

    return Len;
}

/************************************************
函数名称 ： Socket_Listen
功    能 ： 设置指定 Socket(0~7)作为服务器等待远程主机的连接
参    数 ： S ---- Socket number
            Port ---- 端口号
返 回 值 ： 0 / 1
*************************************************/
_Bool Socket_Listen( uint8_t S )
{
    if(Get_Sn_SR(S) == SOCK_INIT)
    {
        Set_Sn_CR(S, Sn_CR_LISTEN);         // 设置 Socket为监听模式

        // if(Get_Sn_SR(S) == SOCK_LISTEN)
        {
            return 1;
        }
    }

    return 0;
}

/************************************************
函数名称 ： Socket_Connect
功    能 ： 设置指定 Socket(0~7)为客户端与远程服务器连接
参    数 ： S ---- Socket number
            Address ---- 远程 IP
            Port ---- 端口号
返 回 值 ： 0 / 1
*************************************************/
_Bool Socket_Connect( uint8_t S, uint8_t *Address, uint16_t Port )
{
    uint8_t status = 0;

    if(Get_Sn_SR(S) == SOCK_INIT)
    {
        if(((Address[0] != 0xFF) && (Address[1] != 0xFF)
                && (Address[2] != 0xFF) && (Address[3] != 0xFF)) ||
           ((Address[0] == 0x00) && (Address[1] != 0x00)
                && (Address[2] != 0x00) && (Address[3] != 0x00)) ||
           (Port != 0x00))
        {
            Set_Sn_DIPR(S, Address);
            Set_Sn_DPORT(S, Port);
            uint8_t pValue[4];
            rw_Data->read_buf_fn(Sn_DIPR0, Sn_REG(S), pValue, 4);
            printf("remote ip: %d %d %d %d\n", pValue[0], pValue[1], pValue[2], pValue[3]);
            rw_Data->read_buf_fn(Sn_DPORT0, Sn_REG(S), pValue, 2);
            printf("remote port: %d\n", (pValue[0]<<8) + pValue[1]);
            Set_Sn_CR(S, Sn_CR_CONNECT);                 // 建立连接

            do{
                status = Get_Sn_SR(S);
                // printf("TCP Client Connect status %X\n", status);

                if(status == SOCK_ESTABLISHED)
                {
                    printf("TCP Client Connect Succeed!\n");
                    return 1;
                }
                else if(Get_Sn_IR(S) & Sn_IR_TIMEOUT)
                {
                    Set_Sn_IR(S, Sn_IR_TIMEOUT);
                    printf("TCP Client Connect Timeout!!!\n");
                    break;
                }
                else if(status == SOCK_CLOSED)
                {
                    break;
                }
            }while(status == SOCK_SYNSENT || status == SOCK_SYNRECV);
        }
    }

    printf("TCP Client Connect Fail!!!\n");
    return 0;
}

/************************************************
函数名称 ： Socket_TCP
功    能 ： 设置指定 Socket(0~7)为 TCP模式
参    数 ： S ---- Socket number
            Port ---- 端口号
返 回 值 ： 0 / 1
*************************************************/
_Bool Socket_TCP( uint8_t S, uint16_t Port )
{
    uint16_t temp_port = 8080;

    Set_Sn_MR(S, Sn_MR_TCP);
    if(Port != 0)
    {
        Set_Sn_PORT(S, Port);               // 配置 Socket端口
    }
    else
    {
        temp_port = Local_Net.LPort + 1;
        Set_Sn_PORT(S, temp_port);
    }
    Set_Sn_CR(S, Sn_CR_OPEN);               // 打开 Socket

    if(Get_Sn_SR(S) == SOCK_INIT)
    {
        printf("TCP Client Config Succeed!\n");
        return 1;
    }

    Socket_Colse(S);
    printf("TCP Client Config Fail!!!\n");
    return 0;
}

/************************************************
函数名称 ： Socket_UDP
功    能 ： 设置指定Socket(0~7)为 UDP(广播)模式
参    数 ： S ---- Socket number
            Port ---- 端口号
返 回 值 ： 0 / 1
*************************************************/
_Bool Socket_UDP( uint8_t S, uint16_t Port )
{
    uint16_t temp_port = 2000;

    Socket_Colse(S);
    Set_Sn_MR(S, Sn_MR_UDP);
    if(Port != 0)
    {
        Set_Sn_PORT(S, Port);               // 配置 Socket端口
    }
    else
    {
        temp_port = Local_Net.LPort + 1;
        Set_Sn_PORT(S, temp_port);
    }
    Set_Sn_CR(S, Sn_CR_OPEN);               // 打开 Socket

    if(Get_Sn_SR(S) == SOCK_UDP)
    {
        printf("UDP Config Succeed!\n");
        return 1;
    }

    Socket_Colse(S);
    printf("UDP Config Fail!!!\n");
    return 0;
}

/************************************************
函数名称 ： Socket_MACRAW
功    能 ： 设置为 以太网 MAC通信模式
参    数 ： S ---- Socket number(仅为Socket 0)
            Port ---- 端口号
返 回 值 ： 0 / 1
*************************************************/
_Bool Socket_MACRAW( uint8_t S, uint16_t Port )
{
    if(0 == S)
    {
        return 1;
    }

    return 0;
}

/************************************************
函数名称 ： Socket_Config
功    能 ： Socket 配置
参    数 ： S ---- Socket number
            Protocol ---- 协议 >>>> 端口的运行模式
            Port ---- Socket的端口号
返 回 值 ： 0 / 1
*************************************************/
uint8_t Socket_Config( uint8_t S, uint8_t Protocol, uint16_t Port )
{
    uint8_t flag = 0;

    // if(0 == Detect_Gateway(S))
    //     return 0;

    switch(Protocol)
    {
        case TCP_SERVER:
            if(Socket_TCP(S, Port))
            {
                flag = TCP_SERVER;
            }
            break;

        case TCP_CLIENT:
            if(Socket_TCP(S, Port))
            {
                flag = TCP_CLIENT;
            }
            break;

        case UDP_MODE:
            if(Socket_UDP(S, Port))
            {
                flag = UDP_MODE;
            }
            break;

        case MACRAW_MODE:
            if(Socket_MACRAW(S, Port))
            {
                flag = MACRAW_MODE;
            }
            break;

        default:
            Socket_Colse(S);
            break;
    }

    return flag;
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

    for (i = 0; i < MAX_SOCK_NUM; i++)       // 按每个通道设置 Tx和 Rx内存的大小
    {
        rw_Data->write_byte_fn(Sn_RXBUF_SIZE, Sn_REG(i), Tx_size[i]);
        rw_Data->write_byte_fn(Sn_TXBUF_SIZE, Sn_REG(i), Rx_size[i]);

#ifdef _USART_DEBUG
        printf("Tx_size[%d]: %d, Sn_TXBUF_SIZE = %d\r\n",i, Tx_size[i], IINCHIP_READ(Sn_TXMEM_SIZE(i)));
        printf("Rx_size[%d]: %d, Sn_RXBUF_SIZE = %d\r\n",i, Rx_size[i], IINCHIP_READ(Sn_RXMEM_SIZE(i)));

#endif /* _USART_DEBUG */

        SSIZE[i] = (uint16_t)(0);
        RSIZE[i] = (uint16_t)(0);

        if (ssum <= 16384)  // all max send size
        {
            SSIZE[i] = (uint16_t)Tx_size[i]*(1024);
        }

        if (rsum <= 16384)  // all max receive size
        {
            RSIZE[i]=(uint16_t)Rx_size[i]*(1024);
        }
        ssum += SSIZE[i];
        rsum += RSIZE[i];
    }
}

/************************************************
函数名称 ： Detect_Gateway
功    能 ： 检查网关服务器
参    数 ： 无
返 回 值 ： 无
*************************************************/
#if 0
static _Bool Detect_Gateway( uint8_t S )
{
    uint8_t ret = 0;

    Set_Sn_DIPR(S, Socket_0.RIp);
    Set_Sn_MR(S, Sn_MR_TCP);
    Set_Sn_CR(S, Sn_CR_OPEN);

    if(Get_Sn_SR(S) == SOCK_INIT)
    {
        Set_Sn_CR(S, Sn_CR_CONNECT);

        do{
            if(Get_Sn_IR(S) & Sn_IR_TIMEOUT)
            {
                Set_Sn_IR(S, Sn_IR_TIMEOUT);
                printf("Detect Gateway Timeout!!!\n");
                break;
            }
            else if(rw_Data->read_byte_fn(Sn_DHAR0, Sn_REG(S)) != 0xFF)
            {
                Socket_Colse(S);
                printf("Succeed Detect Gateway!\n");
                ret = 1;
                break;
            }
        }while(1);
    }

    return ret;
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
    uint8_t local_ip[4] = {192, 168, 1, 110};                   /* 定义 W5500默认IP地址 */
    uint8_t subnet[4] = {255, 255, 255, 0};                     /* 定义 W5500默认子网掩码 */
    uint8_t gateway[4] = {192, 168, 1, 1};                      /* 定义 W5500默认网关 */
    uint8_t dns_server[4] = {114, 114, 114, 114};               /* 定义 W5500默认 DNS */
    uint8_t local_mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};/* 定义 W5500默认 MAC地址 */
    uint16_t local_port = 5000;                                 /* 定义本地端口 */

    /* 定义远程端 IP信息 */
    uint8_t remote_ip[4] = {192, 168, 1, 100};                  /* 远程 IP地址 */
    uint8_t remote_mac[6] = {0x00, 0x06, 0x5D, 0x18, 0x02, 0x11};/* 远程 MAC地址 */
    uint16_t remote_port = 8080;                                /* 远程端口号 */

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
函数名称 ： W5500_Net_Config
功    能 ： 网络配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
void W5500_Net_Config(void)
{
    uint8_t temp[4] = {0};

    Load_Net_Parameters();

    Set_GAR(Local_Net.Gw);                              // 设置网关(Gateway)的IP地址
    Set_SUBR(Local_Net.Sub);                            // 设置子网掩码(MASK)值
    Set_SHAR(Local_Net.LMac);                           // 设置源 MAC地址
    Set_SIPR(Local_Net.LIp);                            // 设置源 IP地址

    Get_SIPR (temp);
    printf("IP : %d.%d.%d.%d\n", temp[0],temp[1],temp[2],temp[3]);

    Get_SUBR(temp);
    printf("MASK : %d.%d.%d.%d\n", temp[0],temp[1],temp[2],temp[3]);

    Get_GAR(temp);
    printf("Gateway : %d.%d.%d.%d\n", temp[0],temp[1],temp[2],temp[3]);

    rw_Data = Get_W5500_Hooks();
}

/*---------------------------- END OF FILE ----------------------------*/


