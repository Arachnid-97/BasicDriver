#include "bsp_iic.h"


#define WAIT_TIME		3

/************************************************
函数名称 ： IIC_Delay_us
功    能 ： 软件毫秒延时
参    数 ： Count ---- 次数
返 回 值 ： 无
*************************************************/
static void IIC_Delay_us( uint32_t Count )
{
    while(Count)
    {
        Count--;
    }
}

/************************************************
函数名称 ： IIC_Start
功    能 ： IIC写启动
参    数 ： 无
返 回 值 ： 0 / 1
*************************************************/
uint8_t IIC_Start(void)
{
	IIC_SCL(HIGH);
	IIC_SDA(HIGH);
    IIC_Delay_us(WAIT_TIME);

    if(!IIC_SDA_READ)              // 回检一次电平状态
    {
        return 0;
    }
	IIC_SDA(LOW);
    IIC_Delay_us(WAIT_TIME);

    if(IIC_SDA_READ)               // 回检一次电平状态
    {
        return 0;
    }
	IIC_SCL(LOW);

    return 1;
}

/************************************************
函数名称 ： IIC_Stop
功    能 ： IIC写停止
参    数 ： 无
返 回 值 ： 无
*************************************************/
void IIC_Stop(void)
{
	IIC_SCL(LOW);
	IIC_SDA(LOW);
    IIC_Delay_us(WAIT_TIME);

    IIC_SCL(HIGH);
    IIC_Delay_us(WAIT_TIME);

	IIC_SDA(HIGH);
}

/************************************************
函数名称 ： IIC_Ack
功    能 ： Ack应答
参    数 ： 无
返 回 值 ： 无
*************************************************/
void IIC_Ack(void)
{
	IIC_SDA(LOW);
    IIC_Delay_us(WAIT_TIME);

    IIC_SCL(HIGH);
    IIC_Delay_us(WAIT_TIME);

	IIC_SCL(LOW);
	IIC_Delay_us(WAIT_TIME);
	
    IIC_SDA(HIGH);
}

/************************************************
函数名称 ： IIC_UnAck
功    能 ： Ack不应答
参    数 ： 无
返 回 值 ： 无
*************************************************/
void IIC_UnAck(void)
{
	IIC_SCL(LOW);
	IIC_SDA(HIGH);
    IIC_Delay_us(WAIT_TIME);

    IIC_SCL(HIGH);
    IIC_Delay_us(WAIT_TIME);

	IIC_SCL(LOW);
}

/************************************************
函数名称 ： IIC_Wait_Ack
功    能 ： ACK等待
参    数 ： 无
返 回 值 ： 0 / 1
*************************************************/
uint8_t IIC_Wait_Ack(void)
{
    uint8_t time = 80;

    IIC_SCL(LOW);
    IIC_Delay_us(WAIT_TIME);

    IIC_SDA(HIGH);
    IIC_Delay_us(WAIT_TIME);

	IIC_SCL(HIGH);
    IIC_Delay_us(WAIT_TIME);

    while(IIC_SDA_READ)
    {
        time--;
        if(!time)
        {
            IIC_Stop();
            return 0;
        }
    }
	IIC_SCL(LOW);

    return 1;
}

/************************************************
函数名称 ： Write_IIC_Byte
功    能 ： IIC写一个字节
参    数 ： Byte ---- 数据
返 回 值 ： 无
*************************************************/
void Write_IIC_Byte( uint8_t Byte )
{
	uint8_t i;

	IIC_SCL(LOW);
	IIC_Delay_us(WAIT_TIME);

	for(i = 0;i < 8;i++)
	{
        IIC_SDA((BitAction)((Byte & 0x80) >> 7));
        Byte <<= 1;
        IIC_Delay_us(WAIT_TIME);

		IIC_SCL(HIGH);
        IIC_Delay_us(WAIT_TIME);

		IIC_SCL(LOW);
        IIC_Delay_us(WAIT_TIME);
    }
}

/************************************************
函数名称 ： Read_IIC_Byte
功    能 ： IIC读一个字节
参    数 ： 无
返 回 值 ： temp ---- 数据
*************************************************/
uint8_t Read_IIC_Byte(void)
{
	uint8_t i;
	uint8_t temp = 0;

	IIC_SDA(HIGH);
	IIC_Delay_us(WAIT_TIME);

	for(i = 0;i < 8;i++)
	{
		temp <<= 1;

		IIC_SCL(LOW);
		IIC_Delay_us(WAIT_TIME);

		IIC_SCL(HIGH);
        IIC_Delay_us(WAIT_TIME);

	#if 0
		if(IIC_SDA_READ)
		{
			temp++;
		}
	
	#else
		temp |= IIC_SDA_READ;
		
	#endif
    }
	IIC_SCL(LOW);
	IIC_Delay_us(WAIT_TIME);
	
	return temp;
}

/************************************************
函数名称 ： Simulate_IIC_Config
功    能 ： 模拟 IIC IO配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
void Simulate_IIC_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	SL_IIC_SCL_APBxClock_FUN(SL_IIC_SCL_CLK, ENABLE);
	SL_IIC_SDA_APBxClock_FUN(SL_IIC_SDA_CLK, ENABLE);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/* SCL */
	GPIO_InitStructure.GPIO_Pin = SL_IIC_SCL_PINS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(SL_IIC_SCL_PORT, &GPIO_InitStructure);
	
	/* SDA */
	GPIO_InitStructure.GPIO_Pin = SL_IIC_SDA_PINS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(SL_IIC_SDA_PORT, &GPIO_InitStructure);
}


/*---------------------------- END OF FILE ----------------------------*/


