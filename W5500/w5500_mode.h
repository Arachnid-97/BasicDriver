#ifndef __W5500_MODE_H
#define __W5500_MODE_H


#include <stdio.h>

/* 端口的运行模式 */
#define TCP_SERVER		0x01	// TCP服务器模式
#define TCP_CLIENT		0x02	// TCP客户端模式
#define UDP_MODE		0x03	// UDP(广播)模式
#define MACRAW_MODE     0x04    // 以太网 MAC通信

/* Socket端口选择，可按自己的习惯定义 */
#define SOCK_TCP_C_PORT           0         // TCP client
#define SOCK_TCP_S_PORT           1         // TCP server
#define SOCK_UDP_PORT             2			// UDP
#define SOCK_MACRAW_PORT          0         // MACRAW 模式只能为 0

#define	MAX_SOCK_NUM		8	// Maxmium number of socket

/* 本地网络结构体 */
typedef struct
{
	uint8_t Gw[4];			/* 网关 */
	uint8_t Sub[4];			/* 子网掩码 */
	uint8_t LIp[4];			/* local IP - 本地IP地址 */
    uint16_t LPort;         /* local Port - 本地端口 */
	uint8_t LMac[6];		/* local MAC - 本地 MAC地址 */
    uint8_t Dns[4];			/* DNS服务器地址 */
}LocalNet_TypeDef;

/* 远程网络结构体 */
typedef struct
{
	uint8_t RIp[4];			/* remote IP - 远程 IP地址 */
	uint16_t RPort;		    /* remote Port - 远程端端口*/
	uint8_t RMac[6];		/* remote MAC - 远程 MAC地址 */
}RemoteNet_TypeDef;


void W5500_TCP_Server(void);
void W5500_TCP_Client(void);
void W5500_UDP_Deal(void);
void Socket_Disconnect( uint8_t S );
void Socket_Colse( uint8_t S );
uint16_t TCP_Buf_Recv( uint8_t S, uint8_t *pBuf, uint16_t Len );
uint16_t TCP_Buf_Send( uint8_t S, const uint8_t *pBuf, uint16_t Len );
uint16_t UDP_Buf_Recv( uint8_t S, uint8_t *pBuf, uint16_t Len, uint8_t *Addr, uint16_t Port );
uint16_t UDP_Buf_Send( uint8_t S, const uint8_t *pBuf, uint16_t Len, const uint8_t *Addr, uint16_t Port );
_Bool Socket_Listen( uint8_t S );
_Bool Socket_Connect( uint8_t S, uint8_t *Address, uint16_t Port );
_Bool Socket_TCP( uint8_t S, uint16_t Port );
_Bool Socket_UDP( uint8_t S, uint16_t Port );
_Bool Socket_MACRAW( uint8_t S, uint16_t Port );
uint8_t Socket_Config( uint8_t S, uint8_t Protocol, uint16_t Port );
void Socket_Buf_Init( uint8_t *Tx_size, uint8_t *Rx_size );
void W5500_Net_Config(void);


#endif /* __W5500_MODE_H */


/*---------------------------- END OF FILE ----------------------------*/


