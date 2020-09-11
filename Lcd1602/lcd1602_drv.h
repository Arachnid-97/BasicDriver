#ifndef __LCD1602_DRV_H
#define __LCD1602_DRV_H


#include "reg52.h"

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

#define LCD1602_DB(x)		(P0 = x)
//#define LCD1602_D0			P0_0
//#define LCD1602_D1			P0_1
//#define LCD1602_D2			P0_2
//#define LCD1602_D3			P0_3
//#define LCD1602_D4			P0_4
//#define LCD1602_D5			P0_5
//#define LCD1602_D6			P0_6
//#define LCD1602_D7			P0_7
#define LCD1602_D7_READ		P0_7


#define LCD1602_RW(x)  		(P2_5 = x)
#define LCD1602_RS(x) 		(P2_6 = x)
#define LCD1602_EN(x)  		(P2_7 = x)

#define LCD_CLEAR			0x01

void Lcd1602_Write_Cmd( uint8_t Cmd );
void Lcd_Printf( const uint8_t *Str, uint8_t X, uint8_t Y );
void Lcd_Show_Str( const uint8_t *Str, uint8_t X, uint8_t Y, uint8_t Len );
void Lcd1602_Config(void); 
void Lcd1602_Init( uint8_t D, uint8_t C, uint8_t B, uint8_t N, uint8_t S );


#endif /* __LCD1602_DRV_H */


/*---------------------------- END OF FILE ----------------------------*/


