#include "stdlib.h"
#include "bsp_adc.h"
#include "bsp_uart.h"

#undef Data_Typedef
#include "filter.h"


#define LIMITING					0
#define MEDIAN						0
#define MEAN						0
#define RECURSIVE_MEAN				0
#define MEDIAN_MEAN					0
#define LIMITING_MEAN				0
#define LOW_PASS					1
#define WEIGHTED_RECURSIVE_MEAN		0
#define DEBOUNCE					0
#define LIMITING_DEBOUNCE			0



static void Delay( uint32_t Cnt )
{
    uint16_t x,y;

    for(x = 0; x < Cnt; x++)
        for(y = 800; y > 0; y--);
}

Data_Typedef Get_AD(void)
{
    // user add AD sampling function
    return ADC_Read(POWER_VOLTAGE);
}

#if LIMITING
Data_Typedef Limiting_Filter( Data_Typedef Value, int Range )
{
    Data_Typedef new_value;

    new_value = Get_AD();
    DUBUG_PRINTF("A%f\r\n", new_value);
    if((new_value - Value > Range)
            || (Value - new_value > Range))
    {
        return Value;
    }

    return new_value;
}

#endif /* LIMITING */

#if MEDIAN
Data_Typedef Median_Filter( int Count )
{
    int i,j;
    Data_Typedef temp;
    Data_Typedef *str;

    if(0 == Count % 2)
        Count++;

    str = (Data_Typedef*)malloc(Count * sizeof(Data_Typedef));

    if(str != NULL) {
        for(i = 0; i < Count; i++) {
            temp = Get_AD();
            *(str + i) = temp;
            DUBUG_PRINTF("A%f\r\n", temp);
            Delay(0x10F);
        }
        /* 冒泡排序 */
        for(j = 0; j < Count - 1; j++) {
            for(i = 0; i < Count - j - 1; i++) {
                if(*(str + i) > *(str + i + 1)) {
                    temp = *(str + i);
                    *(str + i) = *(str + i + 1);
                    *(str + i + 1) = temp;
                }
            }
        }
    } else {
        return -1;
    }

    temp = *(str + (Count-1)/2);

    free(str);

    return temp;
}

#endif /* MEDIAN */

#if MEAN
Data_Typedef Mean_Filter( int Count )
{
    int i;
    Data_Typedef temp;
    Data_Typedef sum = 0;

    for(i = 0; i < Count; i++) {
        temp = Get_AD();
        sum += temp;
        DUBUG_PRINTF("A%f\r\n", temp);
        Delay(0x10F);
    }

    return (Data_Typedef)(sum / Count);
}

#endif /* MEAN */

#if RECURSIVE_MEAN
Data_Typedef Recursive_Mean_Filter( int Count )
{
    static int Start = 0;
    static int Num = 0;
    int i;
    Data_Typedef temp;
    Data_Typedef sum = 0;
    Data_Typedef *str;

    if(0 == Start) {
        Start = Count;
        str = (Data_Typedef*)calloc(Count, sizeof(Data_Typedef));
    } else if(Start != Count) {
        Start = Count;
        str = (Data_Typedef*)realloc(str, Count);		// 重新分配
    }

    if(str != NULL) {
        temp = Get_AD();
        *(str + Num++) = temp;
        DUBUG_PRINTF("A%f\r\n", temp);
        // 覆盖最早采样的数据，相当于递推数据 FIFO
        if(Num >= Count)
            Num = 0;

        for(i = 0; i < Count; i++) {
            sum += *(str + i);
        }
    } else {
        return -1;
    }

    return (Data_Typedef)(sum / Count);
}

#endif /* RECURSIVE_MEAN */

#if MEDIAN_MEAN
Data_Typedef Median_Mean_Filter( int Count )
{
    int i,j;
    Data_Typedef temp;
    Data_Typedef sum = 0;
    Data_Typedef *str;

    if(0 == Count % 2)
        Count++;

    str = (Data_Typedef*)malloc(Count * sizeof(Data_Typedef));

    if(str != NULL) {
        for(i = 0; i < Count; i++) {
            temp = Get_AD();
            *(str + i) = temp;
            DUBUG_PRINTF("A%f\r\n", temp);
            Delay(0x10F);
        }
        for(j = 0; j < Count - 1; j++) {
            for(i = 0; i < Count - j - 1; i++) {
                if(*(str + i) > *(str + i + 1)) {
                    temp = *(str + i);
                    *(str + i) = *(str + i + 1);
                    *(str + i + 1) = temp;
                }
            }
        }
    } else {
        return -1;
    }

    /* 去除最大最小极值后求平均 */
    for(i = 1; i < Count - 1; i++)
        sum += *(str + i);

    free(str);

    return (Data_Typedef)(sum / (Count - 2));
}

#endif /* MEDIAN_MEAN */

#if LIMITING_MEAN
Data_Typedef Limiting_Mean_Filter( int Range, int Count )
{
    static int Start = 0;
    static int Num = 0;
    static Data_Typedef Value = 0;
    int i;
    Data_Typedef new_value;
    Data_Typedef sum = 0;
    Data_Typedef *str;

    if(0 == Start) {
        Start = Count;
        Value = Get_AD();
        str = (Data_Typedef*)calloc(Count, sizeof(Data_Typedef));
    } else if(Start != Count) {
        Start = Count;
        Value = Get_AD();
        str = (Data_Typedef*)realloc(str, Count);		// 重新分配
    }

    if(str != NULL) {
        new_value = Get_AD();
        if((new_value - Value > Range)
                || (Value - new_value > Range))
        {
            *(str + Num++) = Value;
        } else {
            *(str + Num++) = new_value;
            Value = new_value;
        }
        DUBUG_PRINTF("A%f\r\n", Value);

        if(Num >= Count)
            Num = 0;

        for(i = 0; i < Count; i++) {
            sum += *(str + i);
        }
    } else {
        return -1;
    }

    return (Data_Typedef)(sum / Count);
}

#endif /* LIMITING_MEAN */

#if LOW_PASS
Data_Typedef Low_Pass_Filter( Data_Typedef Value, float Factor )
{
    Data_Typedef new_value;

    new_value = Get_AD();
    DUBUG_PRINTF("A%f\r\n", new_value);
    new_value = (Data_Typedef)(new_value * Factor + (1.0f - Factor) * Value);

    return new_value;
}

#endif /* LOW_PASS */

#if WEIGHTED_RECURSIVE_MEAN
Data_Typedef Weighted_Recursive_Mean_Filter( char *Weight_factor, int Count )
{
    static int Sum_coe;
    static int Start = 0;
    static int Num = 0;
    int i;
    Data_Typedef temp;
    Data_Typedef sum = 0;
    Data_Typedef *str;

    if(0 == Start) {
        Start = Count;
        str = (Data_Typedef*)calloc(Count, sizeof(Data_Typedef));
        for(i = 0; i < Count; i++) {
            Sum_coe += *(Weight_factor + i);
        }
    } else if(Start != Count) {
        Start = Count;
        str = (Data_Typedef*)realloc(str, Count);		// 重新分配
        for(i = 0; i < Count; i++) {
            Sum_coe += *(Weight_factor + i);
        }
    }

    if(str != NULL) {
        temp = Get_AD();
        *(str + Num++) = temp;
        DUBUG_PRINTF("A%f\r\n", temp);
        // 覆盖最早采样的数据，相当于递推数据 FIFO
        if(Num >= Count)
            Num = 0;

        for(i = 0; i < Count; i++) {
            sum += *(str + i) * *(Weight_factor + i);
        }
    } else {
        return -1;
    }

    return (Data_Typedef)(sum / Sum_coe);
}

#endif /* WEIGHTED_RECURSIVE_MEAN */

#if DEBOUNCE
Data_Typedef Debounce_Filter( Data_Typedef Value, int Count )
{
    static int Num = 0;
    Data_Typedef new_value;

    new_value = Get_AD();
	DUBUG_PRINTF("A%f\r\n", new_value);
    if(Value != new_value) {
        Num++;
        if(Num > Count) {
            Num = 0;
            Value = new_value;
        }
    }
    else
        Num = 0;

    return Value;
}

#endif /* DEBOUNCE */

#if LIMITING_DEBOUNCE
Data_Typedef Limiter_Debounce_Filter( Data_Typedef Value, int Range, int Count )
{
    static int Num = 0;
    Data_Typedef new_value;

    new_value = Get_AD();
    DUBUG_PRINTF("A%f\r\n", new_value);
    if((new_value - Value > Range)
            || (Value - new_value > Range))
    {
        new_value = Value;
    }

	if(Value != new_value) {
        Num++;
        if(Num > Count) {
            Num = 0;
            Value = new_value;
        }
    }
    else
        Num = 0;

    return Value;
}

#endif /* LIMITING_DEBOUNCE */

