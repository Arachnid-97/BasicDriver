#include "./AT24Cxx/ee_simulate.h"
#include "./AT24Cxx/at24cxx.h"
#include "bsp_iic.h"


/************************************************
�������� �� EE_Write_Buffer
��    �� �� EEPROM����ģ��д��
��    �� �� DeveiceAddr ---- �豸��ַ
			pBuff ---- ����
			WordAddr ---- �ֵ�ַ
			Len ---- ����
�� �� ֵ �� 0 / 1
*************************************************/
uint8_t EE_Write_Buffer( uint8_t DeveiceAddr, uint8_t *pBuff, uint16_t WordAddr, uint16_t Len )
{
    uint16_t i,m;

    /*
		���������õ��� Page Writeģʽ
		��ˣ�ÿ��д����ֻ����ͬһҳ
		����ʵ�ֿ����´���
    */

    for (i = 0; i < Len; i++)
    {
        /* �����͵� 1���ֽڻ���ҳ���׵�ַʱ����Ҫ���·��������źź͵�ַ */
        if ((i == 0) || (Len & (AT24C_PAGE_SIZE - 1)) == 0)
        {
            IIC_Stop();

            /* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ����, һ��С�� 10ms
            	CLKƵ��Ϊ200KHzʱ����ѯ����Ϊ30������
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
                goto WriteFail;	/* EEPROM����д��ʱ */
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
�������� �� EE_Read_Buffer
��    �� �� EEPROM����ģ�����
��    �� �� DeveiceAddr ---- �豸��ַ
			pBuff ---- ����
			WordAddr ---- �ֵ�ַ
			Len ---- ����
�� �� ֵ �� 0 / 1
*************************************************/
uint8_t EE_Read_Buffer( uint8_t DeveiceAddr, uint8_t *pBuff, uint16_t WordAddr, uint16_t Len )
{
    uint16_t i;

    /* ���ô���EEPROM�漴��ȡָ�����У�������ȡ�����ֽ� */

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

    /* ��9����ѭ����ȡ���� */
    for (i = 0; i < Len; i++)
    {
        pBuff[i] = Read_IIC_Byte();	/* ��1���ֽ� */

        /* ÿ���� 1���ֽں���Ҫ���� Ack�� ���һ���ֽڲ���Ҫ Ack���� UnAck */
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
�������� �� EE_Write_Byte
��    �� �� EEPROMģ��дһ���ֽ�
��    �� �� DeveiceAddr ---- �豸��ַ
			Data ---- ����
			WordAddr ---- �ֵ�ַ
�� �� ֵ �� 0 / 1
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
�������� �� EE_Read_Byte
��    �� �� EEPROMģ���һ���ֽ�
��    �� �� DeveiceAddr ---- �豸��ַ
			WordAddr ---- �ֵ�ַ
�� �� ֵ �� data ---- ����
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
�������� �� EE_IIC_Check
��    �� �� ��� I2C�����豸
��    �� �� Address�豸�� I2C���ߵ�ַ
�� �� ֵ �� ack ---- 0 / 1
˵    �� �� CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
*************************************************/
uint8_t EE_IIC_Check( uint8_t Address )
{
	uint8_t ack = 0;
	
	IIC_Start();

	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	Write_IIC_Byte(Address | 0);
	ack = IIC_Wait_Ack();

	IIC_Stop();
	
	return ack;
}

/************************************************
�������� �� EE_IIC_Init
��    �� �� ģ�� IIC��ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void EE_IIC_Init(void)
{
	Simulate_IIC_Config();
	
	IIC_Stop();
}


/*---------------------------- END OF FILE ----------------------------*/

