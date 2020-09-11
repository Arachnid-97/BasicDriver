#ifndef __RF_SEND_H
#define __RF_SEND_H


#include "iostm8s003f3.h"

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

#define RF_LEVEL_COMM(x)    (PD_ODR_ODR1 = x)
#define RF_SYNC_CODE		93
#define	RF_HIGH_LEVEL_TIME  9
#define RF_LOW_LEVEL_TIME	3


void RF_Tx_Dispose( const uint8_t *RtnSig, uint16_t RtnLen, uint8_t Count );
void RF_Tx_Config(void);


#endif /* __RF_SEND_H */


/*---------------------------- END OF FILE ----------------------------*/


