#include "rf_send.h"
#include "bsp_time.h"


/************************************************
函数名称 ： RF_Tx_Dispose
功    能 ： RF信号发送
参    数 ： RtnSig ---- 数据
            RtnLen ---- 长度
            Count ---- 发送次数 
返 回 值 ： 无
*************************************************/
void RF_Tx_Dispose( const uint8_t *RtnSig, uint16_t RtnLen, uint8_t Count )
{
    uint8_t n,l;
    uint16_t i = 0;
	uint8_t temp;

	while(Count--)
	{	
	    RF_LEVEL_COMM(HIGH);
	    Delay_100us(RF_LOW_LEVEL_TIME);
	    RF_LEVEL_COMM(LOW);
	    Delay_100us(RF_SYNC_CODE);											//同步码发送
	    for(l = 0;l < RtnLen;l++)
	    {
			temp = RtnSig[i];
	        for(n = 0;n < 8;n++)							    			//bit转电平
	        {
	            if(temp & 0x80)												// 1
	            {
	                RF_LEVEL_COMM(HIGH); Delay_100us(RF_HIGH_LEVEL_TIME);
	                RF_LEVEL_COMM(LOW); Delay_100us(RF_LOW_LEVEL_TIME);
	            }
	            else														// 0
	            {
	                RF_LEVEL_COMM(HIGH); Delay_100us(RF_LOW_LEVEL_TIME);
	                RF_LEVEL_COMM(LOW); Delay_100us(RF_HIGH_LEVEL_TIME);
	            }
	            temp <<= 1;
	        }
	        i++;
	    }
		RF_LEVEL_COMM(LOW);
	}
}

/************************************************
函数名称 ： RF_Tx_Config
功    能 ： RF发送初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void RF_Tx_Config(void)
{
	RF_LEVEL_COMM(LOW);
}

/*---------------------------- END OF FILE ----------------------------*/


