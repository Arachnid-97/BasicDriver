#ifndef __GB2312_FONT_H
#define __GB2312_FONT_H


#include "N76E003.h"

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

#define GB2312_BASE_ADDR			0x00		// ´æ´¢µÄ GB2312»ùµØÖ·
#define GB2312_1616_BYTE_SIZE		32

uint32_t GB2312_Read( const uint8_t *pData );
void Get_GB2312_Code( uint8_t *pBuff, const uint8_t *pCode );

#endif /* __GB2312_FONT_H */


/*---------------------------- END OF FILE ----------------------------*/


