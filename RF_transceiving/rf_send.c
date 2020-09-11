#include "rf_send.h"
#include "bsp_time.h"


/************************************************
�������� �� RF_Tx_Dispose
��    �� �� RF�źŷ���
��    �� �� RtnSig ---- ����
            RtnLen ---- ����
            Count ---- ���ʹ��� 
�� �� ֵ �� ��
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
	    Delay_100us(RF_SYNC_CODE);											//ͬ���뷢��
	    for(l = 0;l < RtnLen;l++)
	    {
			temp = RtnSig[i];
	        for(n = 0;n < 8;n++)							    			//bitת��ƽ
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
�������� �� RF_Tx_Config
��    �� �� RF���ͳ�ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void RF_Tx_Config(void)
{
	RF_LEVEL_COMM(LOW);
}

/*---------------------------- END OF FILE ----------------------------*/


