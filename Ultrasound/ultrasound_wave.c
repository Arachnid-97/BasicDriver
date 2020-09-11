#include "ultrasound_wave.h"
#include "bsp_time.h"
#include "bsp_uart.h"


//#define _UART_DEBUG

bit g_UT_TimeOut_flag = 0;				// ������ʱ�������־
bit g_UT_Receive_flag = 0;				// ���������ձ�־
bit g_UT_Wait_flag = 0;					// �������ȴ���־
uint8_t g_UT_Time = 0;					// ������ʱ��
uint8_t g_UT_Buf[3] = {0};				// ����������
uint16_t g_UT_Distance = 0x00;			// ������������

/************************************************
�������� �� UT_Send
��    �� �� �������źŷ���
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void UT_Send(void)
{
	g_UT_Time = 3;
	g_UT_TimeOut_flag = 0;
	
    TRIG(HIGH);
    SoftwareDelay_5us(5);
    TRIG(LOW);
		
	while(!ECHO)
	{
		if(!g_UT_Time)
		{
			g_UT_TimeOut_flag = 1;
			break;
		}
	}
	/* ��ʼ��ʱ */
	TIM0_CNTR_CLEAR;
	COUNT_ENABLE;
	
	g_UT_Wait_flag = 1;				// ��־���ȴ���������
}

/************************************************
�������� �� UT_Receive
��    �� �� �������źŽ���
��    �� �� ��
�� �� ֵ �� ��
*************************************************/

#if 1
void UT_Receive(void)
{			
	COUNT_DISABLE;
	g_UT_Distance = TIM0_GetCounter();	// ��ȡʱ�䣨΢�룩
	g_UT_Wait_flag = 0;
	g_UT_Receive_flag = 1;
	
//	g_UT_Distance = g_UT_Distance*(340L >> 1) / 10000;	// ����		
}

#endif

/************************************************
�������� �� UT_Ranging
��    �� �� ���������(һ�廯,���жϲ���)
��    �� �� ��
�� �� ֵ �� distance ---- ���루cm��
*************************************************/

#if 1
uint16_t UT_Ranging(void)
{
	uint8_t UT_distance[3] = {0};
	uint32_t distance = 0;

    TRIG(HIGH);
    SoftwareDelay_5us(5);
    TRIG(LOW);
	
	g_UT_Time = UT_WAIT_TIME;
	
	while(!ECHO)
	{
		if(!g_UT_Time)
			break;
	}
	
	g_UT_Time = UT_WAIT_TIME;
	TIM0_CNTR_CLEAR;
	COUNT_ENABLE;
	
	while(ECHO)
	{
		if(!g_UT_Time)
			break;
	}
	COUNT_DISABLE;
	distance = TIM0_GetCounter();	// ��ȡʱ�䣨΢�룩
	TIM0_CNTR_CLEAR;
		
	distance = distance*(340L >> 1) / 10000;	// ����� cm
		
	g_UT_Buf[0] = (distance / 100) % 10;	// m
	g_UT_Buf[1] = (distance / 10) % 10;		// dm
	g_UT_Buf[2] = (distance) % 10;			// cm	
	
	
//	if(!g_UT_TimeOut_flag)
		UT_distance[0] = g_UT_Buf[0] + '0';	
		UT_distance[1] = g_UT_Buf[1] + '0';			
		UT_distance[2] = g_UT_Buf[2] + '0';				
//	else
//	{		
//		g_UT_Buf[0] = 17;		
//		g_UT_Buf[1] = 17;		
//		g_UT_Buf[2] = 17;		
//		
//		UT_distance[0] = '-';		
//		UT_distance[1] = '-';
//		UT_distance[2] = '-';		
//	}	
	
//	Dig_Display(g_UT_Buf, 3);

	DUBUG_STRING(UT_distance, 3);
	
	DUBUG_PRINTF("cm\r\n");
	
	Lcd_Show_Str(UT_distance, 10, 1, 3);
	
	return distance;
}

#endif

/************************************************
�������� �� UT_Config
��    �� �� ����������
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void UT_Config(void)
{
	IT1 = 1;		// �½���
	IPH &= ~0x04;
	PX1 = 1;		// ���ȼ� 1
	EX1 = 1;		// ʹ���ⲿ�ж� 1
}


/*------------------------------- END OF FILE -------------------------------*/


