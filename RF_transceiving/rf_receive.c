#include "rf_receive.h"
#include "bsp.h"
#include "bsp_time.h"
#include "bsp_eeprom.h"


//#define _ACCURATE_CHECK


/*
    MODE 1:while主函数解码
    MODE 2:上下沿中断触发解码
	MODE 3:上升沿中断触发解码
*/
#define _RF_MODE        2       // (1 or 2 or 3)

_Bool g_RF_Study_flag = 0;							// RF学习标志
_Bool g_RF_Clear_flag = 0;							// RF清码标志
uint8_t g_RF_Study_Time = 0;						// RF学习状态时间计时
uint8_t g_RF_Control = 0;							// 遥控对应控制 ---- 预留
uint8_t g_RF_KeyData = 0;							// 数据码
uint8_t g_RF_AddrBuff[3] = {0};						// RF按键地址值（1527:20个地址位( 1data = 1bit)；2262:8个地址位( 1data = 2bit)）
uint8_t g_RF_Count = 0;								// RF相同数据计数
uint16_t g_RF_Wait_Time = 0;						// RF等待时间

static _Bool s_Lock_flag = 0;
static _Bool s_RF_DecodeOK_flag = 0;				// 解码完成标志
static _Bool s_RF_ReceiveOK_flag = 0;				// 单串数据接收完成标志
static uint8_t s_RF_Type = 0x08;					// 编码类型 1257 -- 0x08/ 2262 -- 0x29
static uint8_t s_RF_DataBuff[2][3] = {0};			// RF数据缓冲区
static uint8_t s_RF_KeyBuff[RF_DATA_NUM] = {0};		// 已学习的按键码

/************************************************
函数名称 ： RF_Decode
功    能 ： RF数据解码(兼容 2262/ 1527)
参    数 ： 无
返 回 值 ： 无
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

//        High_width = Low_width / 31;                  // 基准宽度

//        if((Low_width > 8000) && (Low_width < 11000))
//        if(((Low_width / High_width) >= SYSNC_RATIO_MIN)
//                && (SYSNC_RATIO_MAX <= (Low_width / High_width)))

        if(((High_width*SYSNC_RATIO_MIN) < Low_width)
           && (Low_width < (High_width*SYSNC_RATIO_MAX)))   //同步码（引导码宽度是窄脉冲的31倍）
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
                        while(RF_SIGNAL)                // 高电平等待
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
                        while(!RF_SIGNAL)               // 测低电平宽度
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
                            s_RF_DataBuff[row][column] |= (1 << (7 - i));           //置1
                        }
//                        else if((High_width > 100) && (500 > High_width))
                        else if(((High_width << 1) <= Low_width) && (Low_width <= (High_width << 2)))
//                        else if((High_width <= (Low_width >> 1)) && ((Low_width >> 2) <= High_width))
                        {
                            s_RF_DataBuff[row][column] &= ~(1 << (7 - i));          //置0
                        }
                        else
                        {
//                            COUNT_DISABLE;
                            TIM2_CNTR_CLEAR;
//                            asm("rim\n");
                            return ;                    // 乱码退出
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
                    return ;                            // 同步码错误退出
                }
            }
            if((s_RF_DataBuff[0][0] == s_RF_DataBuff[1][0]) &&
                   (s_RF_DataBuff[0][1] == s_RF_DataBuff[1][1]) &&
                       (s_RF_DataBuff[0][2] == s_RF_DataBuff[1][2]))    //数据比较
            {
//                LED ^= 1;
                s_RF_DecodeOK_flag = 1;
            }
//            asm("rim\n");

#endif /* _ACCURATE_CHECK */

        }
#if	(3 == _RF_MODE)
		while(RF_SIGNAL)                // 高电平等待
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
            if(Elec_level_last)                 // 电平有变化
            {
                Low_width = RF_time;

                if(!Head)
                {
                    if(((Low_width / High_width) >= SYSNC_RATIO_MIN)
                            && (SYSNC_RATIO_MAX <= (Low_width / High_width)))
//                    if(((High_width*SYSNC_RATIO_MIN) <= Low_width)
//                        && (Low_width <= (High_width*SYSNC_RATIO_MAX)))   // 同步码（引导码宽度是窄脉冲的31倍）
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
                    s_RF_DataBuff[Row][Column] = s_RF_DataBuff[Row][Column] << 1;        // 数据左移一位

                    period = High_width + Low_width;
                    if((period >= Period_min) && (Period_max >= period))
                    {
                        if(High_width > Low_width)
//                        if((Low_width > 150) && (Low_width < 600))                           // 窄脉冲判断
                        {
                            s_RF_DataBuff[Row][Column] = s_RF_DataBuff[Row][Column] | 0x01;     // 置一
                        }
                        else
//                        else if((Low_width > 650) && (Low_width < 1200))                    // 宽脉冲判断
                        {
                            s_RF_DataBuff[Row][Column] = s_RF_DataBuff[Row][Column] & 0xfe;     // 置零
                        }
                    }
                    else                                                                // 接收错误
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
                    Data_bit++;                                                     //字节位加一
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
                                   (s_RF_DataBuff[0][2] == s_RF_DataBuff[1][2]))    //数据比较
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
				   (s_RF_DataBuff[0][2] == s_RF_DataBuff[1][2]))    //数据比较
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
函数名称 ： RF_Study_Task
功    能 ： RF学习任务
参    数 ： 无
返 回 值 ： 无
*************************************************/
void RF_Study_Task(void)
{
	static _Bool Overflow = 0;
    uint8_t RF_key_num = 0;
	uint8_t temp_num = 0;

	temp_num = (uint8_t)RF_DATA_NUM / 5;

//    if(g_RF_Study_flag && KEY1 && KEY2 && KEY3)// 学习状态
    {
		g_RF_Study_flag = 0;
		g_RF_Count = 0;

		RF_key_num = s_RF_KeyBuff[0];          // 取已学习的遥控器数量
		if(RF_key_num >= temp_num)
		{
			RF_key_num = 0;                    // 如果遥控器数量超出，覆盖最先学习的
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
			EEPROM_WriteNByte(EEPROM_RFADDR + 1, &s_RF_KeyBuff[1], RF_DATA_NUM);		// 整页刷新替换
		}
		else
		{
			EEPROM_WriteNByte(EEPROM_RFADDR + 5*RF_key_num + 1, &s_RF_KeyBuff[RF_key_num*5 + 1], 5);    // 更新数据到EEPROM
		}
		g_RF_Control = 0;

		RF_key_num++;
		s_RF_KeyBuff[0] = RF_key_num;
//		EEPROM_WriteByte(EEPROM_ADDR, s_RF_KeyBuff[0]);  // 更新数量
    }
}

/************************************************
函数名称 ： RF_Clear_Task
功    能 ： RF清码任务
参    数 ： 无
返 回 值 ： 无
*************************************************/
void RF_Clear_Task(void)
{
	uint8_t n;
	uint8_t temp_num = 0;

//	if(g_RF_Clear_flag && KEY1 && KEY2 && KEY3)		   // 清码处理
	{
		g_RF_Clear_flag = 0;
		temp_num = (uint8_t)RF_DATA_NUM / 5;

		for(n = 0;n < temp_num;n++)
		{
			s_RF_KeyBuff[1 + n] = 0xff;
		}
		EEPROM_ErasePage(EEPROM_RFADDR);				   // 擦除数据

		s_RF_KeyBuff[0] = 0;
//		EEPROM_WriteByte(EEPROM_STARTADDR, s_RF_KeyBuff[0]);// 清零按键数
	}
}


/************************************************
函数名称 ： RF_Rx_Dispose
功    能 ： RF接收状态处理
参    数 ： 无
返 回 值 ： 无
*************************************************/
void RF_Rx_Dispose(void)
{
    uint8_t n;
	uint8_t j, k, q;
	uint8_t temp_num = 0;

	temp_num = (uint8_t)RF_DATA_NUM / 5;			// 遥控器数量


	if(!g_RF_Study_Time && g_RF_Study_flag)
	{
		g_RF_Study_flag = 0;
		g_RF_Count = 0;
	}


    if(s_RF_DecodeOK_flag)
    {
		if(!s_Lock_flag)
//		if(!g_RF_Wait_Time)                				 // 连按判断(防止连续触发)
		{
			s_Lock_flag = 1;

#if 1
            for(j = 0;j < 3;j++)                    // 判断 2262与 1527
            {
                for(k = 0;k < 4;k++)
                {
                    if(2 == ((s_RF_DataBuff[0][j] >> (k*2)) & 3))
                    {
                        j = 152;                    // 有 10数据则为1527（10在 2262为无效码）
                        break;
                    }
                }
                if(152 == j)
                {
                    break;
                }
            }

            /* 获取地址码 */
            g_RF_AddrBuff[0] = s_RF_DataBuff[1][0];
            g_RF_AddrBuff[1] = s_RF_DataBuff[1][1];

            if(152 == j)                            // ev1527
            {
                g_RF_KeyData = s_RF_DataBuff[1][2] & 0x0F;              // 提取码键
                g_RF_AddrBuff[2] = s_RF_DataBuff[1][2] >> 4;            // 提取后 4个地址码
                s_RF_Type = 0x08;
            }
            else                                   // pt2262
            {
                for(q = 0;q < 4;q++)
                {
                    if(3 == ((s_RF_DataBuff[1][2] >> (q*2)) & 3))      //（11为 1，00为 0）
                    {
                        g_RF_KeyData |= 1 << q;     // 提取码键
                    }
                }
                g_RF_AddrBuff[2] = 0x00;           // 2262无后 4位地址,全为 0
                s_RF_Type = 0x29;
            }

#endif

            for(n = 0;n < temp_num;n++)                  // 查找已学习按键
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
函数名称 ： RF_Rx_Config
功    能 ： RF接收 IO配置(PB0)
参    数 ： 无
返 回 值 ： 无
*************************************************/
void RF_Rx_Config(void)
{

#if (2 == _RF_MODE)
    GPIO_Init(RF_RX_PORT, RF_RX_PINS, GPIO_Mode_In_FL_IT);
    EXTI_SetPortSensitivity(EXTI_Port_B, EXTI_Trigger_Rising_Falling);  // 上下沿触发

#elif (3 == _RF_MODE)
    GPIO_Init(RF_RX_PORT, RF_RX_PINS, GPIO_Mode_In_FL_IT);
    EXTI_SetPortSensitivity(EXTI_Port_B, EXTI_Trigger_Rising);          // 上升沿触发

#endif

#if (1 != _RF_MODE)
    EXTI_SelectPort(EXTI_Port_B);                                       // 选用端口 B中断
    EXTI_SetHalfPortSelection(EXTI_HalfPort_B_LSB, ENABLE);             // 使能 PB[3:0]用于 EXTIB中断

    ITC_SetSoftwarePriority(EXTIB_IRQn, ITC_PriorityLevel_3);           // 级别 3

#endif
}

/************************************************
函数名称 ： RF_Rx_Init
功    能 ： RF接收初始化
参    数 ： 无
返 回 值 ： 无
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
//		EEPROM_WriteByte(EEPROM_RFADDR, s_RF_KeyBuff[0]);// 清零按键数
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


