#ifndef __EE_SIMULATE_H
#define __EE_SIMULATE_H


#include "stm32f4xx.h"


//#ifndef ENABLE
//#define ENABLE              1
//#endif /* ENABLE */

//#ifndef DISABLE
//#define DISABLE             0
//#endif /* DISABLE */

#ifndef HIGH
#define HIGH                1
#endif /* HIGH */

#ifndef LOW
#define LOW                 0
#endif /* LOW */

uint8_t EE_Write_Buffer( uint8_t DeveiceAddr, uint8_t *pBuff, uint16_t WordAddr, uint16_t Len );
uint8_t EE_Read_Buffer( uint8_t DeveiceAddr, uint8_t *pBuff, uint16_t WordAddr, uint16_t Len );
uint8_t EE_Write_Byte( uint8_t DeveiceAddr, uint8_t Data, uint16_t WordAddr );
uint8_t EE_Read_Byte( uint8_t DeveiceAddr, uint16_t WordAddr );
uint8_t EE_IIC_Check( uint8_t Address );
void EE_IIC_Init(void);


#endif	/* __EE_SIMULATE_H */


/*---------------------------- END OF FILE ----------------------------*/

