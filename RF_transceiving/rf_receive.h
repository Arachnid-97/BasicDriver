#ifndef __RF_RECEIVE_H
#define __RF_RECEIVE_H


#include "stm8l15x.h"

#define RF_RX_PORT              GPIOB
#define RF_RX_PINS              GPIO_Pin_0
#define RF_SIGNAL               GPIO_ReadInputDataBit(RF_RX_PORT, RF_RX_PINS)

#define EEPROM_RFADDR           0x0000
#define SYSNC_RATIO_MIN         26		// 同步头最小比例
#define SYSNC_RATIO_MAX         36		// 同步头最大比例
#define RF_DATA_NUM				64		// RF数据储存容量(5的倍数)
#define RF_WAIT_TIME			300		// RF接收允许等待的时间
#define RF_CHECK_NUM			6		// RF接收相同的校验值

extern _Bool g_RF_Study_flag;
extern _Bool g_RF_Clear_flag;
extern uint8_t g_RF_Study_Time;
extern uint8_t g_RF_Control;
extern uint8_t g_RF_KeyData;
extern uint8_t g_RF_AddrBuff[3];
extern uint16_t g_RF_Wait_Time;
extern uint8_t g_RF_Count;

void RF_Decode(void);
void RF_Rx_Dispose(void);
void RF_Rx_Config(void);
void RF_Rx_Init(void);


#endif /* __RF_RECEIVE_H */


/*---------------------------- END OF FILE ----------------------------*/


