#ifndef __SENSORS_SERIES_H
#define __SENSORS_SERIES_H


#include "N76E003.h"
#include "SFR_Macro.h"
#include "Function_define.h"

#define DHT11_PIN			P03

typedef struct
{
	uint8_t  humi_int;		// 湿度的整数部分
	uint8_t  humi_deci;	 	// 湿度的小数部分
	uint8_t  temp_int;	 	// 温度的整数部分
	uint8_t  temp_deci;	 	// 温度的小数部分
	uint8_t  check_sum;	 	// 校验和
		                 
}DHT11_Data_TypeDef;

extern DHT11_Data_TypeDef DHT11_Data;

extern bit g_DHT11_Read_flag;

uint8_t Read_DHT11( DHT11_Data_TypeDef *DHT11_Data );


#endif	/* __SENSORS_SERIES_H */


/*---------------------------- END OF FILE ----------------------------*/

