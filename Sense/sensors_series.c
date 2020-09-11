#include "sensors_series.h"
#include "bsp_time.h"
#include "bsp_uart.h"


DHT11_Data_TypeDef DHT11_Data;

bit g_DHT11_Read_flag = 0;			// ���ݶ�ȡ��־

/************************************************
�������� �� Read_Byte
��    �� �� ������
��    �� �� ��
�� �� ֵ �� temp ---- ����
*************************************************/
static uint8_t Read_Byte(void)
{
	uint8_t i, temp=0;

	for(i=0;i<8;i++)    
	{	 
		/*ÿbit��50us�͵�ƽ���ÿ�ʼ����ѯֱ���ӻ����� ��50us �͵�ƽ ����*/  
		while(0 == DHT11_PIN);

		/*DHT11 ��26~28us�ĸߵ�ƽ��ʾ��0������70us�ߵ�ƽ��ʾ��1����
		 *ͨ����� x us��ĵ�ƽ��������������״ ��x ���������ʱ 
		 */
		Delay_30us(1); //��ʱx us�����ʱ��Ҫ�������� 0������ʱ�伴��	   	  

		if(DHT11_PIN)/* x us����Ϊ�ߵ�ƽ��ʾ���ݡ�1�� */
		{
			/* �ȴ�����1�ĸߵ�ƽ���� */
			while(DHT11_PIN);

			temp|=(uint8_t)(0x01<<(7-i));  //�ѵ�7-iλ��1��MSB���� 
		}
		else	 // x us��Ϊ�͵�ƽ��ʾ���ݡ�0��
		{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //�ѵ�7-iλ��0��MSB����
		}
	}
	return temp;
}

/************************************************
�������� �� Read_DHT11
��    �� �� ��ʪ�ȶ�ȡ
��    �� �� ��
�� �� ֵ �� 0 / 1
*************************************************/
uint8_t Read_DHT11( DHT11_Data_TypeDef *DHT11_Data )
{
	P03_Quasi_Mode;
	
	/* ���ģʽ */
//	P03_PushPull_Mode;
	/* �������� */
	clr_P03;
	/* ��ʱ���� 18ms */
	Delay_30us(610);

	/* �������� ������ʱ 30us���� */
	set_P03; 
	/* ��ʱ 30us */
	Delay_30us(1);

	/* ������Ϊ���� �жϴӻ���Ӧ�ź� */ 
//	P03_Input_Mode;

	/* �жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ���������� */   
	if(0 == DHT11_PIN)
	{
		/* ��ѯֱ���ӻ������� 80us�͵�ƽ��Ӧ�źŽ��� */  
		while(0 == DHT11_PIN);

		/* ��ѯֱ���ӻ������� 80us�ߵ�ƽ�����źŽ��� */
		while(1 == DHT11_PIN);

		/* ��ʼ�������� */   
		DHT11_Data->humi_int = Read_Byte();

		DHT11_Data->humi_deci = Read_Byte();

		DHT11_Data->temp_int = Read_Byte();

		DHT11_Data->temp_deci = Read_Byte();

		DHT11_Data->check_sum = Read_Byte();

		/* ��ȡ���������Ÿ�Ϊ���ģʽ */
//		P03_PushPull_Mode;
		/* �������� */
		set_P03;

		/* ����ȡ�������Ƿ���ȷ */
		if(DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int+ DHT11_Data->temp_deci)
			return 1;
		else 
			return 0;
	}
	else
	{		
		return 0;
	}   
}

/*---------------------------- END OF FILE ----------------------------*/


