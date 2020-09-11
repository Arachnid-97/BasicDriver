#include "./AT24Cxx/ee_simulate.h"
#include "./AT24Cxx/at24cxx.h"
#include "bsp_iic.h"


/************************************************
函数名称 ： EE_Write_Buffer
功    能 ： EEPROM缓存模拟写入
参    数 ： DeveiceAddr ---- 设备地址
			pBuff ---- 数据
			WordAddr ---- 字地址
			Len ---- 长度
返 回 值 ： 0 / 1
*************************************************/
uint8_t EE_Write_Buffer( uint8_t DeveiceAddr, uint8_t *pBuff, uint16_t WordAddr, uint16_t Len )
{
    uint16_t i,m;

    /*
		本函数采用的是 Page Write模式
		因此，每次写操作只能在同一页
		具体实现看以下代码
    */

    for (i = 0; i < Len; i++)
    {
        /* 当发送第 1个字节或是页面首地址时，需要重新发起启动信号和地址 */
        if ((i == 0) || (Len & (AT24C_PAGE_SIZE - 1)) == 0)
        {
            IIC_Stop();

            /* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms
            	CLK频率为200KHz时，查询次数为30次左右
            */
            for (m = 0; m < 1000; m++)
            {
                IIC_Start();
                Write_IIC_Byte(DeveiceAddr | AT24C_WRITE);
				if(1 == IIC_Wait_Ack())
					break;
            }
            if (m  == 1000)
            {
                goto WriteFail;	/* EEPROM器件写超时 */
            }

            Write_IIC_Byte((uint8_t)(WordAddr & 0x00FF));
			if(0 == IIC_Wait_Ack())
				goto WriteFail;
        }

        Write_IIC_Byte(*(pBuff + i));
		if(0 == IIC_Wait_Ack())
			goto WriteFail;

        WordAddr++;
    }

    IIC_Stop();
    return 1;
	
WriteFail:
	IIC_Stop();
	return 0;
}

/************************************************
函数名称 ： EE_Read_Buffer
功    能 ： EEPROM缓存模拟读出
参    数 ： DeveiceAddr ---- 设备地址
			pBuff ---- 数据
			WordAddr ---- 字地址
			Len ---- 长度
返 回 值 ： 0 / 1
*************************************************/
uint8_t EE_Read_Buffer( uint8_t DeveiceAddr, uint8_t *pBuff, uint16_t WordAddr, uint16_t Len )
{
    uint16_t i;

    /* 采用串行EEPROM随即读取指令序列，连续读取若干字节 */

    IIC_Start();

    Write_IIC_Byte(DeveiceAddr | AT24C_WRITE);
	if(0 == IIC_Wait_Ack())
		goto ReadFail;

    Write_IIC_Byte((uint8_t)(WordAddr & 0x00FF));
	if(0 == IIC_Wait_Ack())
		goto ReadFail;

    IIC_Start();

    Write_IIC_Byte(DeveiceAddr | AT24C_READ);
	if(0 == IIC_Wait_Ack())
		goto ReadFail;

    /* 第9步：循环读取数据 */
    for (i = 0; i < Len; i++)
    {
        pBuff[i] = Read_IIC_Byte();	/* 读1个字节 */

        /* 每读完 1个字节后，需要发送 Ack， 最后一个字节不需要 Ack，发 UnAck */
        if (i != Len - 1)
        {
            IIC_Ack();
        }
        else
        {
            IIC_UnAck();
        }
    }
    IIC_Stop();
    return 1;

ReadFail:
    IIC_Stop();
    return 0;
}

/************************************************
函数名称 ： EE_Write_Byte
功    能 ： EEPROM模拟写一个字节
参    数 ： DeveiceAddr ---- 设备地址
			Data ---- 数据
			WordAddr ---- 字地址
返 回 值 ： 0 / 1
*************************************************/
uint8_t EE_Write_Byte( uint8_t DeveiceAddr, uint8_t Data, uint16_t WordAddr )
{
	if(IIC_Start())
	{
		Write_IIC_Byte(DeveiceAddr | AT24C_WRITE);
		if(0 == IIC_Wait_Ack())
			goto WriteFail;
		
		Write_IIC_Byte((uint8_t)(WordAddr & 0x00FF));
		if(0 == IIC_Wait_Ack())
			goto WriteFail;
		
		Write_IIC_Byte(Data);
		if(0 == IIC_Wait_Ack())
			goto WriteFail;
		
		IIC_Stop();
		return 1;
	}
	
WriteFail:
	IIC_Stop();
	return 0;
}

/************************************************
函数名称 ： EE_Read_Byte
功    能 ： EEPROM模拟读一个字节
参    数 ： DeveiceAddr ---- 设备地址
			WordAddr ---- 字地址
返 回 值 ： data ---- 数据
*************************************************/
uint8_t EE_Read_Byte( uint8_t DeveiceAddr, uint16_t WordAddr )
{
	uint8_t data = 0;
	
	if(IIC_Start())
	{
		Write_IIC_Byte(DeveiceAddr | AT24C_WRITE);
		if(0 == IIC_Wait_Ack())
			goto ReadFail;
		
		Write_IIC_Byte((uint8_t)(WordAddr & 0x00FF));
		if(0 == IIC_Wait_Ack())
			goto ReadFail;
		
		if(IIC_Start())
		{
			Write_IIC_Byte(DeveiceAddr | AT24C_READ);
			if(0 == IIC_Wait_Ack())
				goto ReadFail;
			
			data = Read_IIC_Byte();
			IIC_UnAck();
			
			IIC_Stop();
			return data;
		}
	}
	
ReadFail:
	IIC_Stop();
	return 0;
}

/************************************************
函数名称 ： EE_IIC_Check
功    能 ： 检测 I2C总线设备
参    数 ： Address设备的 I2C总线地址
返 回 值 ： ack ---- 0 / 1
说    明 ： CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
*************************************************/
uint8_t EE_IIC_Check( uint8_t Address )
{
	uint8_t ack = 0;
	
	IIC_Start();

	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	Write_IIC_Byte(Address | 0);
	ack = IIC_Wait_Ack();

	IIC_Stop();
	
	return ack;
}

/************************************************
函数名称 ： EE_IIC_Init
功    能 ： 模拟 IIC初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void EE_IIC_Init(void)
{
	Simulate_IIC_Config();
	
	IIC_Stop();
}


/*---------------------------- END OF FILE ----------------------------*/

