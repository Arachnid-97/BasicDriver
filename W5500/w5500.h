#ifndef __W5500_H
#define __W5500_H


#include <stdio.h>


typedef struct
{
   /* Write function */
   void (*write_byte_fn)(uint16_t, uint8_t, uint8_t);
   void (*write_buf_fn)(uint16_t, uint8_t, const uint8_t*, uint16_t);

   /* Read function */
   uint8_t (*read_byte_fn)(uint16_t, uint8_t);
   void (*read_buf_fn)(uint16_t, uint8_t, uint8_t*, uint16_t);
} W5500_Drv_Hooks;


void W5500_InitHooks(W5500_Drv_Hooks* Hooks);
W5500_Drv_Hooks* Get_W5500_Hooks(void);

void Set_MR( uint8_t Value );
void Set_GAR( const uint8_t *pData );
void Get_GAR( uint8_t *pData );
void Set_SUBR( const uint8_t *pData );
void Get_SUBR( uint8_t *pData );
void Set_SHAR( const uint8_t *pData );
void Get_SHAR( uint8_t *pData );
void Set_SIPR( const uint8_t *pData );
void Get_SIPR( uint8_t *pData );
uint8_t Get_IR(void);
void Clean_IR( uint8_t Mask );
void Set_RTR( uint16_t Value );
void Set_RCR( uint8_t Value );
uint8_t Get_PHYCFGR(void);
uint8_t Get_ETH_Link(void);

void Set_Sn_MR( uint8_t S, uint8_t Value );
uint8_t Get_Sn_CR( uint8_t S );
void Set_Sn_CR( uint8_t S, uint8_t Value );
uint8_t Get_Sn_IR( uint8_t S );
void Set_Sn_IR( uint8_t S, uint8_t Value );
uint8_t Get_Sn_SR( uint8_t S );
void Set_Sn_PORT( uint8_t S, uint16_t Value );
void Set_Sn_DHAR( uint8_t S, const uint8_t *pValue );
void Set_Sn_DIPR( uint8_t S, const uint8_t *pValue );
void Set_Sn_DPORT( uint8_t S, uint16_t Value );
void Set_Sn_MSSR( uint8_t S, uint16_t Value );
void Set_Sn_TTL( uint8_t S, uint8_t Value );
uint16_t Get_Sn_Tx_FSR( uint8_t S );
uint16_t Get_Sn_Rx_RSR( uint8_t S );
void Set_Sn_Tx_WR( uint8_t S, uint16_t Value );
uint16_t Get_Sn_Tx_WR( uint8_t S );
void Set_Sn_Rx_RD( uint8_t S, uint16_t Value );
uint16_t Get_Sn_Rx_RD( uint8_t S );


#endif /* __W5500_H */


/*---------------------------- END OF FILE ----------------------------*/


