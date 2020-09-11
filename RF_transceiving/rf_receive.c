#include "rf_receive.h"
#include "bsp.h"
#include "bsp_time.h"
#include "bsp_eeprom.h"


//#define _ACCURATE_CHECK


/*
    MODE 1:while����������
    MODE 2:�������жϴ�������
	MODE 3:�������жϴ�������
*/
#define _RF_MODE        2       // (1 or 2 or 3)

_Bool g_RF_Study_flag = 0;							// RFѧϰ��־
_Bool g_RF_Clear_flag = 0;							// RF�����־
uint8_t g_RF_Study_Time = 0;						// RFѧϰ״̬ʱ���ʱ
uint8_t g_RF_Control = 0;							// ң�ض�Ӧ���� ---- Ԥ��
uint8_t g_RF_KeyData = 0;							// ������
uint8_t g_RF_AddrBuff[3] = {0};						// RF������ֵַ��1527:20����ַλ( 1data = 1bit)��2262:8����ַλ( 1data = 2bit)��
uint8_t g_RF_Count = 0;								// RF��ͬ���ݼ���
uint16_t g_RF_Wait_Time = 0;						// RF�ȴ�ʱ��

static _Bool s_Lock_flag = 0;
static _Bool s_RF_DecodeOK_flag = 0;				// ������ɱ�־
static _Bool s_RF_ReceiveOK_flag = 0;				// �������ݽ�����ɱ�־
static uint8_t s_RF_Type = 0x08;					// �������� 1257 -- 0x08/ 2262 -- 0x29
static uint8_t s_RF_DataBuff[2][3] = {0};			// RF���ݻ�����
static uint8_t s_RF_KeyBuff[RF_DATA_NUM] = {0};		// ��ѧϰ�İ�����

/************************************************
�������� �� RF_Decode
��    �� �� RF���ݽ���(���� 2262/ 1527)
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void RF_Decode(void)
{

#if (1 == _RF_MODE || 3 == _RF_MODE)
    uint8_t i;
    uint8_t row = 0;
    uint8_t column = 0;
    static uint16_t Low_width = 0;
    static uint16_t High_width = 0;
	static uint8_t Bit_over = 0;

#if (1 == _RF_MODE)
	static uint8_t Elec_level_last = 0;

	if(RF_SIGNAL == 1)
	{
		if(0 == Elec_level_last)
		{
			Elec_level_last = 1;
			Low_width = TIM2_GetCounter();
			TIM2_CNTR_CLEAR
			Bit_over = 0xFF;
		}
	}
	else if(RF_SIGNAL == 0)
	{
		if(1 == Elec_level_last)
		{
			Elec_level_last = 0;
			High_width = TIM2_GetCounter();
			TIM2_CNTR_CLEAR
		}
	}

    g_RF_KeyData = 0;

#elif (3 == _RF_MODE)
	Low_width = TIM2_GetCounter();
	TIM2_CNTR_CLEAR;
	Bit_over = 0xFF;

#endif /* _RF_MODE */


	if(Bit_over)
	{
		Bit_over = 0;

//        High_width = Low_width / 31;                  // ��׼���

//        if((Low_width > 8000) && (Low_width < 11000))
//        if(((Low_width / High_width) >= SYSNC_RATIO_MIN)
//                && (SYSNC_RATIO_MAX <= (Low_width / High_width)))

        if(((High_width*SYSNC_RATIO_MIN) < Low_width)
           && (Low_width < (High_width*SYSNC_RATIO_MAX)))   //ͬ���루����������խ�����31����
        {
//            asm("sim\n");

#ifdef _ACCURATE_CHECK
            for(row = 0;row < 2;row++)
            {

#endif /* _ACCURATE_CHECK */

                for(column = 0;column < 3;column++)
                {
                    for(i = 0;i < 8;i++)
                    {
//                        Low_width = 0;
//                        High_width = 0;
//                        TIM2_CNTR_CLEAR;
//                        COUNT_ENABLE;
                        while(RF_SIGNAL)                // �ߵ�ƽ�ȴ�
                        {
                            if(TIM2->CNTRH >= 0x0F)
                            {
//                                COUNT_DISABLE;
                                TIM2_CNTR_CLEAR;
//                                asm("rim\n");
                                return ;
                            }
                        }
//                        COUNT_DISABLE;
                        High_width = TIM2_GetCounter();
                        TIM2_CNTR_CLEAR;

//                        COUNT_ENABLE;
                        while(!RF_SIGNAL)               // ��͵�ƽ���
                        {
                            if(TIM2->CNTRH >= 0x0F)
                            {
//                                COUNT_DISABLE;
                                TIM2_CNTR_CLEAR;
//                                asm("rim\n");
                                return ;
                            }
                        }
//                        COUNT_DISABLE;
                        Low_width = TIM2_GetCounter();
                        TIM2_CNTR_CLEAR;

//                        if((Low_width > 100) && (500 > Low_width))
                        if(((Low_width << 2) >= High_width) && (High_width >= (Low_width << 1)))
//                        if((Low_width >= (High_width >> 2)) && ((High_width >> 1) >= Low_width))
                        {
                            s_RF_DataBuff[row][column] |= (1 << (7 - i));           //��1
                        }
//                        else if((High_width > 100) && (500 > High_width))
                        else if(((High_width << 1) <= Low_width) && (Low_width <= (High_width << 2)))
//                        else if((High_width <= (Low_width >> 1)) && ((Low_width >> 2) <= High_width))
                        {
                            s_RF_DataBuff[row][column] &= ~(1 << (7 - i));          //��0
                        }
                        else
                        {
//                            COUNT_DISABLE;
                            TIM2_CNTR_CLEAR;
//                            asm("rim\n");
                            return ;                    // �����˳�
                        }
                    }
                }
                s_RF_ReceiveOK_flag = 1;

#ifdef _ACCURATE_CHECK
//				Low_width = 0;
//				High_width = 0;
//                COUNT_ENABLE;
                TIM2_CNTR_CLEAR;
                while(RF_SIGNAL)
                {
                    if(TIM2->CNTRH >= 0x0F)
                    {
//                        COUNT_DISABLE;
                        TIM2_CNTR_CLEAR;
//                        asm("rim\n");
                        return ;
                    }
                }
//                COUNT_DISABLE;
                High_width = TIM2_GetCounter();
                TIM2_CNTR_CLEAR;

//                COUNT_ENABLE;
                while(!RF_SIGNAL)
                {
                    if(TIM2->CNTRH >= 0x76)
                    {
//                        COUNT_DISABLE;
                        TIM2_CNTR_CLEAR;
                        asm("rim\n");
                        return ;
                    }
                }
//                COUNT_DISABLE;
                Low_width = TIM2_GetCounter();
                TIM2_CNTR_CLEAR;

//                if((Low_width > 8000) && (Low_width < 11000))
                if((Low_width < (High_width*SYSNC_RATIO_MIN))
                   || ((High_width*SYSNC_RATIO_MAX) < Low_width))
                {
//                    COUNT_DISABLE;
                    TIM2_CNTR_CLEAR;
                    asm("rim\n");
                    return ;                            // ͬ��������˳�
                }
            }
            if((s_RF_DataBuff[0][0] == s_RF_DataBuff[1][0]) &&
                   (s_RF_DataBuff[0][1] == s_RF_DataBuff[1][1]) &&
                       (s_RF_DataBuff[0][2] == s_RF_DataBuff[1][2]))    //���ݱȽ�
            {
//                LED ^= 1;
                s_RF_DecodeOK_flag = 1;
            }
//            asm("rim\n");

#endif /* _ACCURATE_CHECK */

        }
#if	(3 == _RF_MODE)
		while(RF_SIGNAL)                // �ߵ�ƽ�ȴ�
		{
			if(TIM2->CNTRH >= 0x0F)
			{
				TIM2_CNTR_CLEAR;
				return ;
			}
		}
		High_width = TIM2_GetCounter();
		TIM2_CNTR_CLEAR;

#endif	/* _RF_MODE */

	} // Bit_Over

//    COUNT_DISABLE;
//    TIM2_CNTR_CLEAR;


#elif (2 == _RF_MODE)
    uint32_t period = 0;
    static uint8_t Row = 0;
    static uint8_t Column = 0;
    static uint8_t Data_bit = 0;
    static uint8_t Elec_level_last = 0;
    static uint16_t Head = 0;
    static uint16_t Low_width = 0;
    static uint16_t High_width = 0;
    static uint32_t RF_time = 0;
    static uint32_t Period_min = 0;
    static uint32_t Period_max = 0;

    if(!s_RF_DecodeOK_flag)
    {
        RF_time = TIM2_GetCounter();
        TIM2_CNTR_CLEAR;
//        COUNT_ENABLE;

        if(RF_SIGNAL)
        {
            if(Elec_level_last)                 // ��ƽ�б仯
            {
                Low_width = RF_time;

                if(!Head)
                {
                    if(((Low_width / High_width) >= SYSNC_RATIO_MIN)
                            && (SYSNC_RATIO_MAX <= (Low_width / High_width)))
//                    if(((High_width*SYSNC_RATIO_MIN) <= Low_width)
//                        && (Low_width <= (High_width*SYSNC_RATIO_MAX)))   // ͬ���루����������խ�����31����
//                    if((RF_time >= 8000)&&(RF_time <= 11000))
                    {
                        Period_min = RF_time / 9;
                        Period_max = RF_time / 7;
                        Data_bit = 0;
                        Head = 1;
                    }
                }
                else
                {
                    Column = (Data_bit >> 3);
                    s_RF_DataBuff[Row][Column] = s_RF_DataBuff[Row][Column] << 1;        // ��������һλ

                    period = High_width + Low_width;
                    if((period >= Period_min) && (Period_max >= period))
                    {
                        if(High_width > Low_width)
//                        if((Low_width > 150) && (Low_width < 600))                           // խ�����ж�
                        {
                            s_RF_DataBuff[Row][Column] = s_RF_DataBuff[Row][Column] | 0x01;     // ��һ
                        }
                        else
//                        else if((Low_width > 650) && (Low_width < 1200))                    // �������ж�
                        {
                            s_RF_DataBuff[Row][Column] = s_RF_DataBuff[Row][Column] & 0xfe;     // ����
                        }
                    }
                    else                                                                // ���մ���
                    {
                        Period_min = 0;
                        Period_max = 0;
                        Data_bit = 0;
                        Head = 0;
                        Column = 0;
                        Row = 0;
//                        COUNT_DISABLE;
                        return ;
                    }
                    Data_bit++;                                                     //�ֽ�λ��һ
                    if(24 == Data_bit)
                    {
                        Period_min = 0;
                        Period_max = 0;
                        Data_bit = 0;
                        Head = 0;
                        Column = 0;
                        s_RF_ReceiveOK_flag = 1;

#ifdef _ACCURATE_CHECK
                        if((s_RF_DataBuff[0][0] == s_RF_DataBuff[1][0]) &&
                               (s_RF_DataBuff[0][1] == s_RF_DataBuff[1][1]) &&
                                   (s_RF_DataBuff[0][2] == s_RF_DataBuff[1][2]))    //���ݱȽ�
                        {
                            s_RF_DecodeOK_flag = 1;
                        }

                        if(Row > 0)
                        {
                            Row = 0;
                        }
                        else
                        {
                            Row++;
                        }

#endif /* _ACCURATE_CHECK */

//                        COUNT_DISABLE;
                    }

                }
            }
            Elec_level_last = 0;
        }
        else
        {
            if(!Elec_level_last)
            {
                if((RF_time >= 40) && (RF_time <= 1270))
                {
                    High_width = RF_time;
                }
            }
            Elec_level_last = 1;
        }
    }

#endif /* _RF_MODE */

#ifndef _ACCURATE_CHECK
	if(s_RF_ReceiveOK_flag)
	{
		s_RF_ReceiveOK_flag = 0;

		if((s_RF_DataBuff[0][0] == s_RF_DataBuff[1][0]) &&
			   (s_RF_DataBuff[0][1] == s_RF_DataBuff[1][1]) &&
				   (s_RF_DataBuff[0][2] == s_RF_DataBuff[1][2]))    //���ݱȽ�
		{
			s_RF_DecodeOK_flag = 1;

			if(s_Lock_flag)
			{
				if(!g_RF_Wait_Time)
				{
					s_Lock_flag = 0;
				}
				g_RF_Wait_Time = RF_WAIT_TIME;
			}
			if(g_RF_Count < 100)
			{
				g_RF_Count++;
				if(g_RF_Count >= RF_CHECK_NUM)
				{
					g_RF_Study_Time = 3;
				}
			}
		}
		else
		{
			s_RF_DataBuff[1][0] = s_RF_DataBuff[0][0];
			s_RF_DataBuff[1][1] = s_RF_DataBuff[0][1];
			s_RF_DataBuff[1][2] = s_RF_DataBuff[0][2];
			g_RF_Count = 0;
		}
	}

#endif /* _ACCURATE_CHECK */

}

/************************************************
�������� �� RF_Study_Task
��    �� �� RFѧϰ����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void RF_Study_Task(void)
{
	static _Bool Overflow = 0;
    uint8_t RF_key_num = 0;
	uint8_t temp_num = 0;

	temp_num = (uint8_t)RF_DATA_NUM / 5;

//    if(g_RF_Study_flag && KEY1 && KEY2 && KEY3)// ѧϰ״̬
    {
		g_RF_Study_flag = 0;
		g_RF_Count = 0;

		RF_key_num = s_RF_KeyBuff[0];          // ȡ��ѧϰ��ң��������
		if(RF_key_num >= temp_num)
		{
			RF_key_num = 0;                    // ���ң����������������������ѧϰ��
			Overflow = 1;
		}
		s_RF_KeyBuff[RF_key_num*5 + 1] = s_RF_Type;
		s_RF_KeyBuff[RF_key_num*5 + 2] = s_RF_DataBuff[1][0];
		s_RF_KeyBuff[RF_key_num*5 + 3] = s_RF_DataBuff[1][1];
		s_RF_KeyBuff[RF_key_num*5 + 4] = s_RF_DataBuff[1][2];
		s_RF_KeyBuff[RF_key_num*5 + 5] = g_RF_Control;

		if(Overflow)
		{
			EEPROM_ErasePage(EEPROM_RFADDR);
			EEPROM_WriteNByte(EEPROM_RFADDR + 1, &s_RF_KeyBuff[1], RF_DATA_NUM);		// ��ҳˢ���滻
		}
		else
		{
			EEPROM_WriteNByte(EEPROM_RFADDR + 5*RF_key_num + 1, &s_RF_KeyBuff[RF_key_num*5 + 1], 5);    // �������ݵ�EEPROM
		}
		g_RF_Control = 0;

		RF_key_num++;
		s_RF_KeyBuff[0] = RF_key_num;
//		EEPROM_WriteByte(EEPROM_ADDR, s_RF_KeyBuff[0]);  // ��������
    }
}

/************************************************
�������� �� RF_Clear_Task
��    �� �� RF��������
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void RF_Clear_Task(void)
{
	uint8_t n;
	uint8_t temp_num = 0;

//	if(g_RF_Clear_flag && KEY1 && KEY2 && KEY3)		   // ���봦��
	{
		g_RF_Clear_flag = 0;
		temp_num = (uint8_t)RF_DATA_NUM / 5;

		for(n = 0;n < temp_num;n++)
		{
			s_RF_KeyBuff[1 + n] = 0xff;
		}
		EEPROM_ErasePage(EEPROM_RFADDR);				   // ��������

		s_RF_KeyBuff[0] = 0;
//		EEPROM_WriteByte(EEPROM_STARTADDR, s_RF_KeyBuff[0]);// ���㰴����
	}
}


/************************************************
�������� �� RF_Rx_Dispose
��    �� �� RF����״̬����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void RF_Rx_Dispose(void)
{
    uint8_t n;
	uint8_t j, k, q;
	uint8_t temp_num = 0;

	temp_num = (uint8_t)RF_DATA_NUM / 5;			// ң��������


	if(!g_RF_Study_Time && g_RF_Study_flag)
	{
		g_RF_Study_flag = 0;
		g_RF_Count = 0;
	}


    if(s_RF_DecodeOK_flag)
    {
		if(!s_Lock_flag)
//		if(!g_RF_Wait_Time)                				 // �����ж�(��ֹ��������)
		{
			s_Lock_flag = 1;

#if 1
            for(j = 0;j < 3;j++)                    // �ж� 2262�� 1527
            {
                for(k = 0;k < 4;k++)
                {
                    if(2 == ((s_RF_DataBuff[0][j] >> (k*2)) & 3))
                    {
                        j = 152;                    // �� 10������Ϊ1527��10�� 2262Ϊ��Ч�룩
                        break;
                    }
                }
                if(152 == j)
                {
                    break;
                }
            }

            /* ��ȡ��ַ�� */
            g_RF_AddrBuff[0] = s_RF_DataBuff[1][0];
            g_RF_AddrBuff[1] = s_RF_DataBuff[1][1];

            if(152 == j)                            // ev1527
            {
                g_RF_KeyData = s_RF_DataBuff[1][2] & 0x0F;              // ��ȡ���
                g_RF_AddrBuff[2] = s_RF_DataBuff[1][2] >> 4;            // ��ȡ�� 4����ַ��
                s_RF_Type = 0x08;
            }
            else                                   // pt2262
            {
                for(q = 0;q < 4;q++)
                {
                    if(3 == ((s_RF_DataBuff[1][2] >> (q*2)) & 3))      //��11Ϊ 1��00Ϊ 0��
                    {
                        g_RF_KeyData |= 1 << q;     // ��ȡ���
                    }
                }
                g_RF_AddrBuff[2] = 0x00;           // 2262�޺� 4λ��ַ,ȫΪ 0
                s_RF_Type = 0x29;
            }

#endif

            for(n = 0;n < temp_num;n++)                  // ������ѧϰ����
            {
                if((s_RF_DataBuff[1][0] == s_RF_KeyBuff[n*5 + 2]) &&
                       (s_RF_DataBuff[1][1] == s_RF_KeyBuff[n*5 + 3]) &&
                           (s_RF_DataBuff[1][2] == s_RF_KeyBuff[n*5 + 4]))
                {
                    switch(s_RF_KeyBuff[n*5 + 5])
                    {
                        case 1:

                                break;
                        case 2:

                                break;
                        case 3:

                                break;
                        default:
                                break;
                    }
                }
            }
            GPIO_ToggleBits(LED_PORT, LED_PINS);
        }
//		RF_Study_Task();
        s_RF_DecodeOK_flag = 0;
    }
//	RF_Clear_Task();
}

/************************************************
�������� �� RF_Rx_Config
��    �� �� RF���� IO����(PB0)
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void RF_Rx_Config(void)
{

#if (2 == _RF_MODE)
    GPIO_Init(RF_RX_PORT, RF_RX_PINS, GPIO_Mode_In_FL_IT);
    EXTI_SetPortSensitivity(EXTI_Port_B, EXTI_Trigger_Rising_Falling);  // �����ش���

#elif (3 == _RF_MODE)
    GPIO_Init(RF_RX_PORT, RF_RX_PINS, GPIO_Mode_In_FL_IT);
    EXTI_SetPortSensitivity(EXTI_Port_B, EXTI_Trigger_Rising);          // �����ش���

#endif

#if (1 != _RF_MODE)
    EXTI_SelectPort(EXTI_Port_B);                                       // ѡ�ö˿� B�ж�
    EXTI_SetHalfPortSelection(EXTI_HalfPort_B_LSB, ENABLE);             // ʹ�� PB[3:0]���� EXTIB�ж�

    ITC_SetSoftwarePriority(EXTIB_IRQn, ITC_PriorityLevel_3);           // ���� 3

#endif
}

/************************************************
�������� �� RF_Rx_Init
��    �� �� RF���ճ�ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void RF_Rx_Init(void)
{
	uint8_t i;
	uint8_t num = 0;

    RF_Rx_Config();
//    EEPROM_ReadNByte(EEPROM_RFADDR, s_RF_KeyBuff, RF_DATA_NUM);
//	if(0xFF == s_RF_KeyBuff[0])
//	{
//		s_RF_KeyBuff[0] = 0;
//		EEPROM_WriteByte(EEPROM_RFADDR, s_RF_KeyBuff[0]);// ���㰴����
//	}

    for(i = 1;i < RF_DATA_NUM;i += 5)
	{
		if(s_RF_KeyBuff[i] != 0x00)
		{
			num++;
		}
	}
	s_RF_KeyBuff[0] = num;
}


/*---------------------------- END OF FILE ----------------------------*/


