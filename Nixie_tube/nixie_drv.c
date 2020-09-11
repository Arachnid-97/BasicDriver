#include "nixie_drv.h"
#include "bsp_time.h"


/*
	说明：共阴数码管
	     a
	    ——
	   |    | b
	 f |  g | 
	    ——
	   |    | c
	 e |  d | 
	    ——  。 dp  
*/

/* 0、1、2、3、4、5、6、7、8、9、A、b、C、d、E、F、‘-’的显示码 */
static code uint8_t DIG_Code[18] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
									  0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x40};

/************************************************
函数名称 ： Dig_Display
功    能 ： 数码管动态显示
参    数 ： pData ---- 数据
			Num ---- 显示个数					
返 回 值 ： 无
*************************************************/
void Dig_Display( const uint8_t *pData, uint8_t Num )
{
	uint8_t i = 0;
	
	for(i = 0; i < Num; i++)
	{
		DIG1(LOW);
		DIG2(LOW);
		DIG3(LOW);
		DIG4(LOW);
		
		switch(Num - i - 1)
		{
			case 0:
					DIG1(HIGH);
					break;
			case 1:
					DIG2(HIGH);
					break;
			case 2:
					DIG3(HIGH);
					break;
			case 3:
					DIG4(HIGH);
					break;
			default:
					break;
		}
		NIXIE_IO(DIG_Code[*pData++]);        	//发送段码
		SoftwareDelay_5us(100);
		NIXIE_IO(0x00);						    //消隐
	}
}

/************************************************
函数名称 ： Nixie_Tube_Config
功    能 ： 数码管显示配置
参    数 ： 无					
返 回 值 ： 无
*************************************************/
void Nixie_Tube_Config(void)
{
	DIG1(HIGH);
	DIG2(HIGH);
	DIG3(HIGH);
	DIG4(HIGH);
	
	NIXIE_IO(0xFF);
}


/*---------------------------- END OF FILE ----------------------------*/


