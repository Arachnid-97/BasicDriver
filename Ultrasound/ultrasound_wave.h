#ifndef __UNLTRASOUND_WAVE_H
#define __UNLTRASOUND_WAVE_H


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

#define TRIG(x)        		(P3_2 = x)	// 信号发射
#define ECHO        		P3_3		// 信号接收

#define UT_WAIT_TIME		20

extern bit g_UT_TimeOut_flag;
extern bit g_UT_Receive_flag;
extern bit g_UT_Wait_flag;
extern uint8_t g_UT_Time;
extern uint8_t g_UT_Buf[3];
extern uint16_t g_UT_Distance;

void UT_Send(void);
void UT_Receive(void);
uint16_t UT_Ranging(void);
void UT_Config(void);
	

#endif /* __UNLTRASOUND_WAVE_H */


/*---------------------------- END OF FILE ----------------------------*/


