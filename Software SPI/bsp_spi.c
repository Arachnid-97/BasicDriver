#include "bsp_spi.h"


#define WAIT_TIME		2

/*
 * MODE 0:CPOL=0��CPHA=0����ʱ����̬ʱ��SCLK���ڵ͵�ƽ�����ݲ������ڵ� 1�����أ�	\
 * 			Ҳ���� SCLK�ɵ͵�ƽ���ߵ�ƽ�����䣬�������ݲ������������أ����ݷ��������½��ء�

 * MODE 1:CPOL=0��CPHA=1����ʱ����̬ʱ��SCLK���ڵ͵�ƽ�����ݷ������ڵ� 2�����أ�	\
 * 			Ҳ���� SCLK�ɸߵ�ƽ���͵�ƽ�����䣬�������ݲ��������½��أ����ݷ������������ء�

 * MODE 2:CPOL=1��CPHA=0����ʱ����̬ʱ��SCLK���ڸߵ�ƽ�����ݲɼ����ڵ� 1�����أ�	\
 * 			Ҳ���� SCLK�ɸߵ�ƽ���͵�ƽ�����䣬�������ݲɼ������½��أ����ݷ������������ء�

 * MODE 3:CPOL=1��CPHA=1����ʱ����̬ʱ��SCLK���ڸߵ�ƽ�����ݷ������ڵ� 2�����أ�	\
 * 			Ҳ���� SCLK�ɵ͵�ƽ���ߵ�ƽ�����䣬�������ݲɼ����������أ����ݷ��������½��ء�
*/
#define _CPOL     1
#define _CPHA     1

/************************************************
�������� �� SPI_Delay_us
��    �� �� ���������ʱ
��    �� �� Count ---- ����
�� �� ֵ �� ��
*************************************************/
static void SPI_Delay_us( uint32_t Count )
{
    while(Count)
    {
        Count--;
    }
}

#if (0 == _CPOL && 0 == _CPHA) /* ----- MODE 0 ----- */
/************************************************
�������� �� Write_SPI_Byte
��    �� �� SPIд��һ���ֽ�
��    �� �� Byte ---- ����
�� �� ֵ �� Byte ---- ����
*************************************************/
uint8_t Write_SPI_Byte( uint8_t Byte )
{
	uint8_t i;
	
	for(i = 0;i < 8;i++)
	{
		SPI_SCK(LOW);
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
		
	#if 0
        SPI_MOSI((Byte & 0x80) >> 7);
		
	#else
		if(Byte & 0x80)
		{
			SPI_MOSI(HIGH);
		}
		else
		{
			SPI_MOSI(LOW);
		}
		
	#endif
		Byte <<= 1;
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
		SPI_SCK(HIGH);
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
		Byte |= SPI_MISO;
	}
	
	SPI_SCK(LOW);
	
	return Byte;
}

/************************************************
�������� �� Read_SPI_Byte
��    �� �� SPIֻ��һ���ֽ�
��    �� �� ��
�� �� ֵ �� temp ---- ����
*************************************************/
uint8_t Read_SPI_Byte(void)
{
	uint8_t i;
	uint8_t temp = 0;
	
	for(i = 0;i < 8;i++)
	{
		SPI_SCK(LOW);
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
		temp <<= 1;
		
	#if 1
		temp |= SPI_MISO;
		
	#else
		if(SPI_MISO)
		{
			temp++;
		}
		
	#endif
		SPI_SCK(HIGH);
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
	}

	SPI_SCK(LOW);

	return temp;
}

#elif (0 == _CPOL && 1 == _CPHA) /* ----- MODE 1 ----- */
/************************************************
�������� �� Write_SPI_Byte
��    �� �� SPIд��һ���ֽ�
��    �� �� Byte ---- ����
�� �� ֵ �� Byte ---- ����
*************************************************/
uint8_t Write_SPI_Byte( uint8_t Byte )
{
	uint8_t i;

	SPI_SCK(LOW);
	
	for(i = 0;i < 8;i++)
	{
		SPI_SCK(HIGH);
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
		
	#if 0
        SPI_MOSI((Byte & 0x80) >> 7);
		
	#else
		if(Byte & 0x80)
		{
			SPI_MOSI(HIGH);
		}
		else
		{
			SPI_MOSI(LOW);
		}
		
	#endif
		Byte <<= 1;
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
		SPI_SCK(LOW);
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
		Byte |= SPI_MISO;
	}
	
	return Byte;
}

/************************************************
�������� �� Read_SPI_Byte
��    �� �� SPIֻ��һ���ֽ�
��    �� �� ��
�� �� ֵ �� temp ---- ����
*************************************************/
uint8_t Read_SPI_Byte(void)
{
	uint8_t i;
	uint8_t temp = 0;

	SPI_SCK(LOW);
	
	for(i = 0;i < 8;i++)
	{
		SPI_SCK(HIGH);
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
		temp <<= 1;
		
	#if 1
		temp |= SPI_MISO;
		
	#else
		if(SPI_MISO)
		{
			temp++;
		}
		
	#endif
		SPI_SCK(LOW);
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
	}
	
	return temp;
}

#elif (1 == _CPOL && 0 == _CPHA) /* ----- MODE 2 ----- */
/************************************************
�������� �� Write_SPI_Byte
��    �� �� SPIд��һ���ֽ�
��    �� �� Byte ---- ����
�� �� ֵ �� Byte ---- ����
*************************************************/
uint8_t Write_SPI_Byte( uint8_t Byte )
{
	uint8_t i;
	
	for(i = 0;i < 8;i++)
	{
		SPI_SCK(HIGH);
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
		
	#if 0
        SPI_MOSI((Byte & 0x80) >> 7);
		
	#else
		if(Byte & 0x80)
		{
			SPI_MOSI(HIGH);
		}
		else
		{
			SPI_MOSI(LOW);
		}
		
	#endif
		Byte <<= 1;
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
		SPI_SCK(LOW);
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
		Byte |= SPI_MISO;
	}
	
	SPI_SCK(HIGH);
	
	return Byte;
}

/************************************************
�������� �� Read_SPI_Byte
��    �� �� SPIֻ��һ���ֽ�
��    �� �� ��
�� �� ֵ �� temp ---- ����
*************************************************/
uint8_t Read_SPI_Byte(void)
{
	uint8_t i;
	uint8_t temp = 0;
	
	for(i = 0;i < 8;i++)
	{
		SPI_SCK(HIGH);
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
		temp <<= 1;
		
	#if 1
		temp |= SPI_MISO;
		
	#else
		if(SPI_MISO)
		{
			temp++;
		}
		
	#endif
		SPI_SCK(LOW);
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
	}

	SPI_SCK(HIGH);

	return temp;
}

#elif (1 == _CPOL && 1 == _CPHA) /* ----- MODE 3 ----- */
/************************************************
�������� �� Write_SPI_Byte
��    �� �� SPIд��һ���ֽ�
��    �� �� Byte ---- ����
�� �� ֵ �� Byte ---- ����
*************************************************/
uint8_t Write_SPI_Byte( uint8_t Byte )
{
	uint8_t i;

	SPI_SCK(HIGH);
	
	for(i = 0;i < 8;i++)
	{
		SPI_SCK(LOW);
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
		
	#if 0
        SPI_MOSI((Byte & 0x80) >> 7);
		
	#else
		if(Byte & 0x80)
		{
			SPI_MOSI(HIGH);
		}
		else
		{
			SPI_MOSI(LOW);
		}
		
	#endif
		Byte <<= 1;
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
		SPI_SCK(HIGH);
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
		Byte |= SPI_MISO;
	}
	
	return Byte;
}

/************************************************
�������� �� Read_SPI_Byte
��    �� �� SPIֻ��һ���ֽ�
��    �� �� ��
�� �� ֵ �� temp ---- ����
*************************************************/
uint8_t Read_SPI_Byte(void)
{
	uint8_t i;
	uint8_t temp = 0;

	SPI_SCK(HIGH);
	
	for(i = 0;i < 8;i++)
	{
		SPI_SCK(LOW);
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
		temp <<= 1;
		
	#if 1
		temp |= SPI_MISO;
		
	#else
		if(SPI_MISO)
		{
			temp++;
		}
		
	#endif
		SPI_SCK(HIGH);
		SPI_Delay_us(WAIT_TIME);       // �յȴ�
	}
	
	return temp;
}

#endif /* SPI MODE */

/************************************************
�������� �� Simulate_SPI_Config
��    �� �� ģ�� SPI IO����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Simulate_SPI_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	SL_SPI_SCK_APBxClock_FUN(SL_SPI_SCK_CLK, ENABLE);
	SL_SPI_MOSI_APBxClock_FUN(SL_SPI_MOSI_CLK, ENABLE);
	SL_SPI_MISO_APBxClock_FUN(SL_SPI_MISO_CLK, ENABLE);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/* SCK */
	GPIO_InitStructure.GPIO_Pin = SL_SPI_SCK_PINS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SL_SPI_SCK_PORT, &GPIO_InitStructure);
	
	/* MISO */
	GPIO_InitStructure.GPIO_Pin = SL_SPI_MISO_PINS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SL_SPI_MISO_PORT, &GPIO_InitStructure);
	
	/* MOSI */
	GPIO_InitStructure.GPIO_Pin = SL_SPI_MOSI_PINS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SL_SPI_MOSI_PORT, &GPIO_InitStructure);
}


/*---------------------------- END OF FILE ----------------------------*/


