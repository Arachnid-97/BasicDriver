#ifndef __W5500_DRV_H
#define __W5500_DRV_H


#include "iostm8s003f3.h"

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

#ifndef ENABLE
#define ENABLE              1
#endif /* ENABLE */

#ifndef DISABLE
#define DISABLE             0
#endif /* DISABLE */

#ifndef HIGH
#define HIGH                1
#endif /* HIGH */

#ifndef LOW
#define LOW                 0
#endif /* LOW */

#define	MAX_SOCK_NUM		8	// Maxmium number of socket

#define W5500_SCS(x)        (PD_ODR_ODR1 = x)
#define W5500_INT           PC_IDR_IDR5
#define W5500_RST(x)        (PC_ODR_ODR6 = x)
#define W5500_SCLK(x)       (PC_ODR_ODR7 = x)
#define W5500_MISO          PD_IDR_IDR3
#define W5500_MOSI(x)       (PD_ODR_ODR2 = x)

extern LocalNet_TypeDef    Local_Net;
extern RemoteNet_TypeDef   Socket_0;

extern uint16_t SSIZE[MAX_SOCK_NUM];
extern uint16_t RSIZE[MAX_SOCK_NUM];

void W5500_Delay_ms( uint32_t Count );
void W5500_Write_Byte( uint16_t Address, uint8_t Control, uint8_t Value );
void W5500_Write_Buf( uint16_t Address, uint8_t Control, const uint8_t *pBuf, uint16_t Len );
uint8_t W5500_Read_Byte( uint16_t Address, uint8_t Control );
void W5500_Read_Buf( uint16_t Address, uint8_t Control, uint8_t *pBuf, uint16_t Len );
void W5500_Init(void);


#endif /* __W5500_DRV_H */


/*---------------------------- END OF FILE ----------------------------*/


