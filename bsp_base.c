#include "bsp_base.h"
#include "bsp_uart.h"


/* 选择打开相应的函数 */
#define _Power
#define _HexToChar
#define _CharToHex
#define _ClrArray
#define _CheckDigit
#define _Data_Extract

/************************************************
函数名称 ： Power
功    能 ： 幂运算
参    数 ： Base ---- 底数
			Index ---- 指数
返 回 值 ： power ---- 幂
*************************************************/

#ifdef _Power
uint32_t Power( uint16_t Base, uint8_t Index )								
{
	uint8_t y;
	uint32_t power = 1;

	for(y = 0;y < Index;y++)
	{
		power *= Base;
	}
	return power;
}

#endif /* _Power */

/************************************************
函数名称 ： HexToChar
功    能 ： 十六进制转字符串
参    数 ： Dest ---- 输出缓冲区
			Array ---- 需转换的数组
			Length ---- 转换长度
返 回 值 ： i ---- 长度
*************************************************/

#ifdef _HexToChar
uint16_t HexToChar( uint8_t *Dest, uint8_t *Array, uint16_t Length )
{
	uint8_t ddh = 0,ddl = 0;
	uint16_t i = 1;

	while(Length--)
	{
		ddh =	Array[i-1] >> 4;
		ddl = Array[i-1] &= ~0xF0;
		if((ddh >= 0) && (ddh <= 9))
		{
			ddh += 0x30;
		}
		else if((ddh >= 10) && (ddh <= 15))	
		{
			ddh += 0x37;
		}
		
		if((ddl >= 0)&&(ddl <= 9))
		{
			ddl += 0x30;
		}
		else if((ddl >= 10)&&(ddl <= 15))	
		{
			ddl += 0x37;
		}
		
		Dest[2*i - 2] = ddh;
		Dest[2*i - 1] = ddl;				
		i++;	
	}	
	return i;
}

#endif /* _HexToChar */

/************************************************
函数名称 ： CharToHex
功    能 ： 字符串转十六进制
参    数 ： Dest ---- 输出缓冲区
			Array ---- 需转换的数组
			Length ---- 转换长度
返 回 值 ： i ---- 长度
*************************************************/

#ifdef _CharToHex
uint16_t CharToHex( uint8_t *Dest, const uint8_t *Array, uint16_t Length )
{
	uint8_t s1,s2;
	uint16_t l,i = 0;	

	l = Length / 2;
	for(i = 0;i < l;i++)								//字符串转十六进制
	{
		s1 = Array[2*i] - 0x30;					
		if(s1 > 9)	
		{
			s1 -= 7;
		}
		
		s2 = Array[2*i + 1] - 0x30;
		if(s2 > 9)	
		{
			s2 -= 7;
		}
		
		Dest[i] = s1*16 + s2;
	}
	return i;
}

#endif /* _CharToHex */

/************************************************
函数名称 ： ClrArray
功    能 ： 清空ArrayBuf
参    数 ： Array ---- 要清除的数据
			BufCnt ---- 清除长度
返 回 值 ： 无
*************************************************/

#ifdef _ClrArray
void ClrArray( uint8_t *Array, short BufCnt )	
{
	short i = 0;

	for(i = 0;i < BufCnt;i++)
	{
		Array[i] = 0;
	}
}

#endif /* _ClrArray */

/************************************************
函数名称 ： CheckDigit
功    能 ： 校验码运算
参    数 ： pData ---- 检验码组
			Check_L ---- 长度
返 回 值 ： check_code ---- 单字节校验码
*************************************************/

#ifdef _CheckDigit
uint8_t CheckDigit( const uint8_t *pData, uint16_t Check_L )
{
	uint8_t check_code = 0;
	uint16_t i;

	for(i = 0;i < Check_L;i++)
	{
		check_code += *(pData + i);
	}
	check_code = ~check_code;
	return check_code;
}

#endif /* _CheckDigit */

/************************************************
函数名称 ： Data_Extract
功    能 ： 信息提取
参    数 ： Getbuf ---- 输入的参数
			Outbuf ---- 输出的数据
			Cmpack ---- 校验的数据
			Caput ---- 提取标记
			End ---- 结束标记
			Len ---- 输入长度
返 回 值 ： n ---- 数据长度
*************************************************/

#ifdef _Data_Extract
uint16_t Data_Extract( uint8_t *Getbuf, uint8_t *Outbuf, char *Cmpack, char Caput, char End, uint16_t Len )
{
	uint8_t num = 0;
	uint16_t j = 0;
	uint16_t q,n,i = 0;

	do
	{
		for(q = 0;Getbuf[i + q] == Cmpack[q];q++)		//判断AT数据类型
		{
			if(Cmpack[q] == Caput)						//为“ : ”时开始提取数据信息
			{
				i += (q + 1);
				for(n = 0;n < 256;n++)
				{
					if(Getbuf[i + n] == ' ')			//空格符判断
					{
						num++;
					}
					else if(Getbuf[i + n] == End)		//判断该条指令结束
					{
						return n - num;
					}
					Outbuf[j++] = Getbuf[i + n];
				}
			}
		}
		i++;
	}while(i < Len);
	return 0;											//信息不匹配
}

#endif /* _Data_Extract */


/*---------------------------- END OF FILE ----------------------------*/


