#include "bsp_spi.h"


#define WAIT_TIME		2

/*
 * MODE 0:CPOL=0，CPHA=0：此时空闲态时，SCLK处于低电平，数据采样是在第 1个边沿，	\
 * 			也就是 SCLK由低电平到高电平的跳变，所以数据采样是在上升沿，数据发送是在下降沿。

 * MODE 1:CPOL=0，CPHA=1：此时空闲态时，SCLK处于低电平，数据发送是在第 2个边沿，	\
 * 			也就是 SCLK由高电平到低电平的跳变，所以数据采样是在下降沿，数据发送是在上升沿。

 * MODE 2:CPOL=1，CPHA=0：此时空闲态时，SCLK处于高电平，数据采集是在第 1个边沿，	\
 * 			也就是 SCLK由高电平到低电平的跳变，所以数据采集是在下降沿，数据发送是在上升沿。

 * MODE 3:CPOL=1，CPHA=1：此时空闲态时，SCLK处于高电平，数据发送是在第 2个边沿，	\
 * 			也就是 SCLK由低电平到高电平的跳变，所以数据采集是在上升沿，数据发送是在下降沿。
*/
#define _CPOL     1
#define _CPHA     1

/************************************************
函数名称 ： SPI_Delay_us
功    能 ： 软件毫秒延时
参    数 ： Count ---- 次数
返 回 值 ： 无
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
函数名称 ： Write_SPI_Byte
功    能 ： SPI写读一个字节
参    数 ： Byte ---- 数据
返 回 值 ： Byte ---- 数据
*************************************************/
uint8_t Write_SPI_Byte( uint8_t Byte )
{
	uint8_t i;
	
	for(i = 0;i < 8;i++)
	{
		SPI_SCK(LOW);
		SPI_Delay_us(WAIT_TIME);       // 空等待
		
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
		SPI_Delay_us(WAIT_TIME);       // 空等待
		SPI_SCK(HIGH);
		SPI_Delay_us(WAIT_TIME);       // 空等待
		Byte |= SPI_MISO;
	}
	
	SPI_SCK(LOW);
	
	return Byte;
}

/************************************************
函数名称 ： Read_SPI_Byte
功    能 ： SPI只读一个字节
参    数 ： 无
返 回 值 ： temp ---- 数据
*************************************************/
uint8_t Read_SPI_Byte(void)
{
	uint8_t i;
	uint8_t temp = 0;
	
	for(i = 0;i < 8;i++)
	{
		SPI_SCK(LOW);
		SPI_Delay_us(WAIT_TIME);       // 空等待
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
		SPI_Delay_us(WAIT_TIME);       // 空等待
	}

	SPI_SCK(LOW);

	return temp;
}

#elif (0 == _CPOL && 1 == _CPHA) /* ----- MODE 1 ----- */
/************************************************
函数名称 ： Write_SPI_Byte
功    能 ： SPI写读一个字节
参    数 ： Byte ---- 数据
返 回 值 ： Byte ---- 数据
*************************************************/
uint8_t Write_SPI_Byte( uint8_t Byte )
{
	uint8_t i;

	SPI_SCK(LOW);
	
	for(i = 0;i < 8;i++)
	{
		SPI_SCK(HIGH);
		SPI_Delay_us(WAIT_TIME);       // 空等待
		
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
		SPI_Delay_us(WAIT_TIME);       // 空等待
		SPI_SCK(LOW);
		SPI_Delay_us(WAIT_TIME);       // 空等待
		Byte |= SPI_MISO;
	}
	
	return Byte;
}

/************************************************
函数名称 ： Read_SPI_Byte
功    能 ： SPI只读一个字节
参    数 ： 无
返 回 值 ： temp ---- 数据
*************************************************/
uint8_t Read_SPI_Byte(void)
{
	uint8_t i;
	uint8_t temp = 0;

	SPI_SCK(LOW);
	
	for(i = 0;i < 8;i++)
	{
		SPI_SCK(HIGH);
		SPI_Delay_us(WAIT_TIME);       // 空等待
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
		SPI_Delay_us(WAIT_TIME);       // 空等待
	}
	
	return temp;
}

#elif (1 == _CPOL && 0 == _CPHA) /* ----- MODE 2 ----- */
/************************************************
函数名称 ： Write_SPI_Byte
功    能 ： SPI写读一个字节
参    数 ： Byte ---- 数据
返 回 值 ： Byte ---- 数据
*************************************************/
uint8_t Write_SPI_Byte( uint8_t Byte )
{
	uint8_t i;
	
	for(i = 0;i < 8;i++)
	{
		SPI_SCK(HIGH);
		SPI_Delay_us(WAIT_TIME);       // 空等待
		
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
		SPI_Delay_us(WAIT_TIME);       // 空等待
		SPI_SCK(LOW);
		SPI_Delay_us(WAIT_TIME);       // 空等待
		Byte |= SPI_MISO;
	}
	
	SPI_SCK(HIGH);
	
	return Byte;
}

/************************************************
函数名称 ： Read_SPI_Byte
功    能 ： SPI只读一个字节
参    数 ： 无
返 回 值 ： temp ---- 数据
*************************************************/
uint8_t Read_SPI_Byte(void)
{
	uint8_t i;
	uint8_t temp = 0;
	
	for(i = 0;i < 8;i++)
	{
		SPI_SCK(HIGH);
		SPI_Delay_us(WAIT_TIME);       // 空等待
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
		SPI_Delay_us(WAIT_TIME);       // 空等待
	}

	SPI_SCK(HIGH);

	return temp;
}

#elif (1 == _CPOL && 1 == _CPHA) /* ----- MODE 3 ----- */
/************************************************
函数名称 ： Write_SPI_Byte
功    能 ： SPI写读一个字节
参    数 ： Byte ---- 数据
返 回 值 ： Byte ---- 数据
*************************************************/
uint8_t Write_SPI_Byte( uint8_t Byte )
{
	uint8_t i;

	SPI_SCK(HIGH);
	
	for(i = 0;i < 8;i++)
	{
		SPI_SCK(LOW);
		SPI_Delay_us(WAIT_TIME);       // 空等待
		
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
		SPI_Delay_us(WAIT_TIME);       // 空等待
		SPI_SCK(HIGH);
		SPI_Delay_us(WAIT_TIME);       // 空等待
		Byte |= SPI_MISO;
	}
	
	return Byte;
}

/************************************************
函数名称 ： Read_SPI_Byte
功    能 ： SPI只读一个字节
参    数 ： 无
返 回 值 ： temp ---- 数据
*************************************************/
uint8_t Read_SPI_Byte(void)
{
	uint8_t i;
	uint8_t temp = 0;

	SPI_SCK(HIGH);
	
	for(i = 0;i < 8;i++)
	{
		SPI_SCK(LOW);
		SPI_Delay_us(WAIT_TIME);       // 空等待
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
		SPI_Delay_us(WAIT_TIME);       // 空等待
	}
	
	return temp;
}

#endif /* SPI MODE */

/************************************************
函数名称 ： Simulate_SPI_Config
功    能 ： 模拟 SPI IO配置
参    数 ： 无
返 回 值 ： 无
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


