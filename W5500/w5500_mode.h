#ifndef __W5500_MODE_H
#define __W5500_MODE_H


#include "iostm8s003f3.h"

/* 端口的运行模式 */
#define TCP_SERVER		0x01	// TCP服务器模式
#define TCP_CLIENT		0x02	// TCP客户端模式
#define UDP_MODE		0x03	// UDP(广播)模式
#define MACRAW_MODE     0x04    // 以太网 MAC通信

/* Socket端口选择，可按自己的习惯定义 */
#define SOCK_TCP_C_PORT           0         // TCP client
#define SOCK_TCP_S_PORT           1         // TCP server
#define SOCK_UDP_PORT             2
#define SOCK_MACRAW_PORT          0         // MACRAW 模式只能为 0

void W5500_TCP_Server(void);
void W5500_TCP_Client(void);
void W5500_UDP_Deal(void);
void Socket_Disconnect( uint8_t S );
void Socket_Colse( uint8_t S );
void TCP_Buf_Recv( uint8_t S, uint8_t *pBuf, uint16_t Len );
void TCP_Buf_Send( uint8_t S, const uint8_t *pBuf, uint16_t Len );
void UDP_Buf_Recv( uint8_t S, uint8_t *pBuf, uint16_t Len, uint8_t *Addr, uint16_t Port );
void UDP_Buf_Send( uint8_t S, const uint8_t *pBuf, uint16_t Len, const uint8_t *Addr, uint16_t Port );
_Bool Socket_Listen( uint8_t S );
_Bool Socket_Connect( uint8_t S, uint8_t *Address, uint16_t Port );
_Bool Socket_TCP( uint8_t S, uint16_t Port );
_Bool Socket_UDP( uint8_t S, uint16_t Port );
_Bool Socket_MACRAW( uint8_t S, uint16_t Port );
uint8_t Socket_Config( uint8_t S, uint8_t Protocol, uint16_t Port );


#endif /* __W5500_MODE_H */


/*---------------------------- END OF FILE ----------------------------*/


