#ifndef __NIXIE_DRV_H
#define __NIXIE_DRV_H


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

#define DIG1(x)				(P1_0 = x)
#define DIG2(x)				(P1_1 = x)
#define DIG3(x)				(P1_2 = x)
#define DIG4(x)				(P1_3 = x)
#define NIXIE_IO(x)			(P0 = x)

void Dig_Display( const uint8_t *pData, uint8_t Num );
void Nixie_Tube_Config(void);


#endif /* __NIXIE_DRV_H */


/*---------------------------- END OF FILE ----------------------------*/


