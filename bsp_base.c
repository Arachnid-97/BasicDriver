#include "bsp_base.h"
#include "bsp_uart.h"


/* ѡ�����Ӧ�ĺ��� */
#define _Power
#define _HexToChar
#define _CharToHex
#define _ClrArray
#define _CheckDigit
#define _Data_Extract

/************************************************
�������� �� Power
��    �� �� ������
��    �� �� Base ---- ����
			Index ---- ָ��
�� �� ֵ �� power ---- ��
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
�������� �� HexToChar
��    �� �� ʮ������ת�ַ���
��    �� �� Dest ---- ���������
			Array ---- ��ת��������
			Length ---- ת������
�� �� ֵ �� i ---- ����
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
�������� �� CharToHex
��    �� �� �ַ���תʮ������
��    �� �� Dest ---- ���������
			Array ---- ��ת��������
			Length ---- ת������
�� �� ֵ �� i ---- ����
*************************************************/

#ifdef _CharToHex
uint16_t CharToHex( uint8_t *Dest, const uint8_t *Array, uint16_t Length )
{
	uint8_t s1,s2;
	uint16_t l,i = 0;	

	l = Length / 2;
	for(i = 0;i < l;i++)								//�ַ���תʮ������
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
�������� �� ClrArray
��    �� �� ���ArrayBuf
��    �� �� Array ---- Ҫ���������
			BufCnt ---- �������
�� �� ֵ �� ��
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
�������� �� CheckDigit
��    �� �� У��������
��    �� �� pData ---- ��������
			Check_L ---- ����
�� �� ֵ �� check_code ---- ���ֽ�У����
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
�������� �� Data_Extract
��    �� �� ��Ϣ��ȡ
��    �� �� Getbuf ---- ����Ĳ���
			Outbuf ---- ���������
			Cmpack ---- У�������
			Caput ---- ��ȡ���
			End ---- �������
			Len ---- ���볤��
�� �� ֵ �� n ---- ���ݳ���
*************************************************/

#ifdef _Data_Extract
uint16_t Data_Extract( uint8_t *Getbuf, uint8_t *Outbuf, char *Cmpack, char Caput, char End, uint16_t Len )
{
	uint8_t num = 0;
	uint16_t j = 0;
	uint16_t q,n,i = 0;

	do
	{
		for(q = 0;Getbuf[i + q] == Cmpack[q];q++)		//�ж�AT��������
		{
			if(Cmpack[q] == Caput)						//Ϊ�� : ��ʱ��ʼ��ȡ������Ϣ
			{
				i += (q + 1);
				for(n = 0;n < 256;n++)
				{
					if(Getbuf[i + n] == ' ')			//�ո���ж�
					{
						num++;
					}
					else if(Getbuf[i + n] == End)		//�жϸ���ָ�����
					{
						return n - num;
					}
					Outbuf[j++] = Getbuf[i + n];
				}
			}
		}
		i++;
	}while(i < Len);
	return 0;											//��Ϣ��ƥ��
}

#endif /* _Data_Extract */


/*---------------------------- END OF FILE ----------------------------*/


