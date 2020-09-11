#include "w5500_mode.h"
#include "w5500_drv.h"
#include "w5500.h"


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
    uint16_t temp = 0;

    temp = Get_Sn_SR(SOCK_TCP_S_PORT);

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

                len = Get_Sn_Rx_RSR(SOCK_TCP_S_PORT);
                if(len > 0)
                {
                    TCP_Buf_Recv(SOCK_TCP_S_PORT, buf, len);				/*接收来自Client的数据*/
//                    buf[len] = 0x00; 											/*添加字符串结束符*/
//                    printf("%s\r\n",buf);
//                    TCP_Buf_Send(SOCK_TCP_S_PORT, buf, len);					/*向Client发送数据*/
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
                Socket_Config(SOCK_UDP_PORT, TCP_CLIENT, Local_Net.LPort);
                break;

		case SOCK_INIT:													    /*socket处于初始化状态*/
                Socket_Connect(SOCK_TCP_C_PORT,Socket_0.RIp,Socket_0.RPort);/*socket连接服务器*/
                break;

		case SOCK_ESTABLISHED: 												/*socket处于连接建立状态*/
                if(Get_Sn_IR(SOCK_TCP_C_PORT) & Sn_IR_CON)
                {
                    Set_Sn_IR(SOCK_TCP_C_PORT, Sn_IR_CON); 					/*清除接收中断标志位*/
                }

                len = Get_Sn_Rx_RSR(SOCK_TCP_C_PORT);
                if(len > 0)
                {
                    TCP_Buf_Recv(SOCK_TCP_C_PORT,buf,len); 					/*接收来自Server的数据*/
//                    buf[len] = 0x00;  										/*添加字符串结束符*/
//                    printf("%s\r\n",buf);
//                    TCP_Buf_Send(SOCK_TCP_C_PORT, buf, len);					/*向Server发送数据*/
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
                W5500_Delay_ms(10);
                if(Get_Sn_IR(SOCK_UDP_PORT) & Sn_IR_RECV)
                {
                    Set_Sn_IR(SOCK_UDP_PORT, Sn_IR_RECV);              /*清接收中断*/
                }

                len = Get_Sn_Rx_RSR(SOCK_UDP_PORT);
                if(len > 0)                                           /*接收到数据*/
                {
                    UDP_Buf_Recv(SOCK_UDP_PORT, buf, len, Socket_0.RIp, Socket_0.RPort);    /*W5500接收发送来的数据*/
//                    buf[len - 8] = 0x00;                                                    /*添加字符串结束符*/
//                    printf("%s\r\n",buf);                                                   /*打印接收缓存*/
//                    UDP_Buf_Send(SOCK_UDP_PORT, buf, len - 8, Socket_0.RIp, Socket_0.RPort);/*W5500把接收到的数据发送给Remote*/
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
            Len ---- 长度
返 回 值 ： 无
*************************************************/
void TCP_Buf_Recv( uint8_t S, uint8_t *pBuf, uint16_t Len )
{
    uint16_t addr = 0;

    if(0 == Len)
    {
        return ;
    }

    addr = Get_Sn_Rx_RD(S);

    W5500_Read_Buf(addr, Sn_RX_BUFFER(S), pBuf, Len); // 读取数据

    addr += Len;
    Set_Sn_Rx_RD(S, addr);
    Set_Sn_CR(S, Sn_CR_RECV);
}

/************************************************
函数名称 ： TCP_Buf_Send
功    能 ： TCP数据发送
参    数 ： S ---- Socket number
            pBuf ---- 数据
            Len ---- 长度
返 回 值 ： 无
*************************************************/
void TCP_Buf_Send( uint8_t S, const uint8_t *pBuf, uint16_t Len )
{
    uint16_t addr = 0;

    if(0 == Len)
    {
        return ;
    }

    addr = Get_Sn_Tx_WR(S);

    W5500_Write_Buf(addr, Sn_TX_BUFFER(S), pBuf, Len);  // 写入数据

    addr += Len;
    Set_Sn_Tx_WR(S, addr);
    Set_Sn_CR(S, Sn_CR_SEND);
}

/************************************************
函数名称 ： UDP_Buf_Recv
功    能 ： UDP数据接收（协议：IP(4 Byte) + Port(2 Byte) + data_len(2 Byte) + data(n Byte)）
参    数 ： S ---- Socket number
            pBuf ---- 数据
            Len ---- 长度
            Addr ---- remote IP
            Port ---- remote port
返 回 值 ： 无
*************************************************/
void UDP_Buf_Recv( uint8_t S, uint8_t *pBuf, uint16_t Len, uint8_t *Addr, uint16_t Port )
{
    uint8_t head[8] = {0};
    uint16_t addr = 0;
    uint16_t data_len = 0;

    if(0 == Len)
    {
        return ;
    }

    addr = Get_Sn_Rx_RD(S);
    W5500_Read_Buf(addr, Sn_RX_BUFFER(S), head, 8);
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
    data_len = head[6];
    data_len = (data_len << 8) + head[7];

    W5500_Read_Buf(addr, Sn_RX_BUFFER(S), pBuf, data_len);  // 读取数据

    addr += data_len;
    Set_Sn_Rx_RD(S, addr);
    Set_Sn_CR(S, Sn_CR_RECV);
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
void UDP_Buf_Send( uint8_t S, const uint8_t *pBuf, uint16_t Len, const uint8_t *Addr, uint16_t Port )
{
    uint16_t addr = 0;

    if(((0 == *Addr) && (0 == *(Addr + 1)) \
        && (0 == *(Addr + 2)) && (0 == *(Addr + 3))) \
            || (0 == Port) || (0 == Len))
    {
        return ;
    }

    /* 写入远程 IP、端口 */
    Set_Sn_DIPR(S, Addr);
    Set_Sn_DPORT(S, Port);

    addr = Get_Sn_Tx_WR(S);

    W5500_Write_Buf(addr, Sn_TX_BUFFER(S), pBuf, Len);  // 写入数据

    addr += Len;
    Set_Sn_Tx_WR(S, addr);
    Set_Sn_CR(S, Sn_CR_SEND);
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
        W5500_Delay_ms(10);
        if(Get_Sn_SR(S) == SOCK_LISTEN)
        {
            return 1;
        }
    }

//    Set_Sn_CR(S, Sn_CR_CLOSE);
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
            Set_Sn_CR(S, Sn_CR_CONNECT);                 // 建立连接

            while(Get_Sn_SR(S) != SOCK_SYNSENT)
            {
                if(Get_Sn_SR(S) == SOCK_ESTABLISHED)
                {
                    return 1;
                }
                else if(Get_Sn_IR(S) & Sn_IR_TIMEOUT)
                {
                    Set_Sn_IR(S, Sn_IR_TIMEOUT);
                    break;
                }
            }
        }
    }

//    Set_Sn_CR(S, Sn_CR_CLOSE);
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
    uint16_t temp_port = 2000;

    Socket_Colse(S);
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
    W5500_Delay_ms(10);
    if(Get_Sn_SR(S) == SOCK_INIT)
    {
        return 1;
    }

    Set_Sn_CR(S, Sn_CR_CLOSE);
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
    W5500_Delay_ms(10);
    if(Get_Sn_SR(S) == SOCK_UDP)
    {
        return 1;
    }

    Set_Sn_CR(S, Sn_CR_CLOSE);
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


/*---------------------------- END OF FILE ----------------------------*/


