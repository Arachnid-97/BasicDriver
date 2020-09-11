#include "./AT24Cxx/at24cxx.h"
#include "./AT24Cxx/ee_simulate.h"
#include "bsp.h"
#include "bsp_uart.h"


/* ע��STM32��Ӳ�� I2C��ȷ��һЩ BUG������˵ʹ��ʱ�кܶ಻���������Ҫ��	\
       ���� DMA������жϣ���ʹ�ø�ģʽ���׳��������� */

/* �Ƿ�����ģ�� IIC */
#define USE_SIMULATE_IIC		0

/* ��ʹ��Ӳ�� IIC������ʹ�� DMA���� */
#define IIC_DMA_ENABLE			1

/* EEPROM��д���Ժ� */
#define _EE_TEST				1
#define USE_TEST_ADDR			0x0110


/* ʹ�� 16λ��ַ����ú�,����Ĭ�� 8λ��ַ */
//#define AT24CXX_16BIT_ADDR

/* Uncomment this line to use the default start and end of critical section
   callbacks (it disables then enabled all interrupts) */
#define USE_DEFAULT_CRITICAL_CALLBACK
/* Start and End of critical section: these callbacks should be typically used
   to disable interrupts when entering a critical section of I2C communication
   You may use default callbacks provided into this driver by uncommenting the
   define USE_DEFAULT_CRITICAL_CALLBACK.
   Or you can comment that line and implement these callbacks into your
   application */


#define MAX_TIME_OUT		((uint32_t)0x1000)
#define MAX_LONGTIME_OUT	((uint32_t)(10 * MAX_TIME_OUT))

__IO uint16_t g_EEData_ReadPointer;
__IO uint16_t g_EEData_WritePointer;

static __IO uint32_t AT24C_TimeOut = MAX_TIME_OUT;
static uint8_t s_AT24Cxx_Addr = 0;

/* Public functions ---------------------------------------------------------*/
static uint8_t EE_Test(void);

#if (IIC_DMA_ENABLE && 0 == USE_SIMULATE_IIC)
static DMA_InitTypeDef s_EEDMA_InitStructure;

/* Private functions ---------------------------------------------------------*/
static void IIC_DMA_Config( uint32_t pBuffer, uint32_t BufferSize, uint32_t Direction );

#endif /* IIC_DMA_ENABLE && 0 == USE_SIMULATE_IIC */


/************************************************
�������� �� TimeOut_Callback
��    �� �� �ȴ���ʱ�ص�����
��    �� �� ErrorCode ---- �������
�� �� ֵ �� ����ֵ 0
*************************************************/
static uint8_t TimeOut_Callback( char ErrorCode )
{
    /* �ȴ���ʱ��Ĵ���,���������Ϣ */
    AT24C_DEBUG_PRINTF("IIC �ȴ���ʱ!  EerrorCode = %d\n",ErrorCode);

    return 0;
}

/************************************************
�������� �� EE_DMA_TxWait
��    �� �� DMA������ɵȴ�
��    �� �� ��
�� �� ֵ �� ��
��    ע :  DMAģʽ�£�����������ô˺�����飨���ֽڳ��⣩
*************************************************/
void EE_DMA_TxWait(void)
{

#if IIC_DMA_ENABLE
    /* Wait transfer through DMA to be complete */
    AT24C_TimeOut = MAX_LONGTIME_OUT;
    while (g_EEData_WritePointer > 0)
    {
        if((AT24C_TimeOut--) == 0) {
            TimeOut_Callback(19);
            return;
        }
    }

#endif /* IIC_DMA_ENABLE */
}

/************************************************
�������� �� EE_DMA_RxWait
��    �� �� DMA������ɵȴ�
��    �� �� ��
�� �� ֵ �� ��
��    ע :  DMAģʽ�£�����������ô˺�����飨���ֽڳ��⣩
*************************************************/
void EE_DMA_RxWait(void)
{

#if IIC_DMA_ENABLE
    /* Wait transfer through DMA to be complete */
    AT24C_TimeOut = MAX_LONGTIME_OUT;
    while (g_EEData_ReadPointer > 0)
    {
        if((AT24C_TimeOut--) == 0) {
            TimeOut_Callback(20);
            return;
        }
    }

#endif /* IIC_DMA_ENABLE */
}

/************************************************
�������� �� AT24Cxx_Busy_Wait
��    �� �� AT24Cxxæ�ȴ�
��    �� �� ��
�� �� ֵ �� 0 / 1
��    ע :  Ӳ��ģʽ�¶�д�������ô˺������
*************************************************/
static uint8_t AT24Cxx_Busy_Wait(void)
{
    uint8_t temp = 1;
    __IO uint16_t temp_SR1;

#if (0 == USE_SIMULATE_IIC)
    /*!< While the bus is busy */
    AT24C_TimeOut = MAX_LONGTIME_OUT;
    while(I2C_GetFlagStatus(AT24C_I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(0);
    }

    /* Send START condition */
    I2C_GenerateSTART(AT24C_I2Cx, ENABLE);

    /* Test on EV5 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(1);
    }

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(AT24C_I2Cx, s_AT24Cxx_Addr, I2C_Direction_Transmitter);

    /* Wait for ADDR flag to be set (Slave acknowledged his address) */
    AT24C_TimeOut = MAX_LONGTIME_OUT;
    do
    {
        I2C_GenerateSTART(AT24C_I2Cx, ENABLE);
        I2C_Send7bitAddress(AT24C_I2Cx, s_AT24Cxx_Addr, I2C_Direction_Transmitter);

        temp_SR1 = I2C_ReadRegister(AT24C_I2Cx, I2C_Register_SR1);
        if(0 == (AT24C_TimeOut--))
        {
            temp = TimeOut_Callback(2);
            break;
        }
    } while(!(temp_SR1 & 0x0002));

    /* Clear AF flag */
    I2C_ClearFlag(AT24C_I2Cx, I2C_FLAG_AF);

    /* STOP condition */
    I2C_GenerateSTOP(AT24C_I2Cx, ENABLE);

#endif /* USE_SIMULATE_IIC */

    return temp;
}

/************************************************
�������� �� AT24Cxx_Write_Byte
��    �� �� AT24Cxxдһ���ֽ�
��    �� �� Byte ---- ����
			Address ---- ��ַ
�� �� ֵ �� 0 / 1
*************************************************/
uint8_t AT24Cxx_Write_Byte( uint8_t Byte, uint16_t Address )
{
#if USE_SIMULATE_IIC
	return EE_Write_Byte(s_AT24Cxx_Addr, Byte, Address);

#else
    /* While the bus is busy */
    AT24C_TimeOut = MAX_LONGTIME_OUT;
    while(I2C_GetFlagStatus(AT24C_I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(4);
    }

    /* Send START condition */
    I2C_GenerateSTART(AT24C_I2Cx, ENABLE);

    /* Test on EV5 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(5);
    }

    /* Send EEPROM address for write */
    AT24C_TimeOut = MAX_TIME_OUT;
    I2C_Send7bitAddress(AT24C_I2Cx, s_AT24Cxx_Addr, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(6);
    }

#ifndef AT24CXX_16BIT_ADDR

    /* Send the EEPROM's internal address to write to : only one byte Address */
    I2C_SendData(AT24C_I2Cx, (uint8_t)(Address & 0x00FF));

#else

    /* Send the EEPROM's internal address to write to : MSB of the address first */
    I2C_SendData(AT24C_I2Cx, (uint8_t)((Address & 0xFF00) >> 8));

    /* Test on EV8 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(6.5);
    }

    /*!< Send the EEPROM's internal address to write to : LSB of the address */
    I2C_SendData(AT24C_I2Cx, (uint8_t)(Address & 0x00FF));

#endif /* AT24CXX_16BIT_ADDR */

    /* Test on EV8 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(7);
    }

    /* Send the current byte */
    I2C_SendData(AT24C_I2Cx, Byte);

    /* Test on EV8 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(8);
    }

    /* Send STOP condition */
    I2C_GenerateSTOP(AT24C_I2Cx, ENABLE);

    /* If all operations OK, return 1 */
    return 1;

#endif /* USE_SIMULATE_IIC */
}

/************************************************
�������� �� AT24Cxx_Read_Byte
��    �� �� AT24Cxx��һ���ֽ�
��    �� �� Address ---- ��ַ
�� �� ֵ �� temp ---- ����
*************************************************/
uint8_t AT24Cxx_Read_Byte( uint16_t Address )
{
	
#if USE_SIMULATE_IIC
	return EE_Read_Byte(s_AT24Cxx_Addr, Address);
	
#else
    uint8_t temp;

    /*!< While the bus is busy */
    AT24C_TimeOut = MAX_LONGTIME_OUT;
    while(I2C_GetFlagStatus(AT24C_I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(9);
    }

    /*!< Send START condition */
    I2C_GenerateSTART(AT24C_I2Cx, ENABLE);

    /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(10);
    }

    /*!< Send EEPROM address for write */
    I2C_Send7bitAddress(AT24C_I2Cx, s_AT24Cxx_Addr, I2C_Direction_Transmitter);

    /*!< Test on EV6 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(11);
    }

    /* Clear EV6 by setting again the PE bit */
    I2C_Cmd(AT24C_I2Cx, ENABLE);

#ifndef AT24CXX_16BIT_ADDR

    /*!< Send the EEPROM's internal address to read from: Only one byte address */
    I2C_SendData(AT24C_I2Cx, (uint8_t)(Address & 0x00FF));

#else

    /*!< Send the EEPROM's internal address to read from: MSB of the address first */
    I2C_SendData(AT24C_I2Cx, (uint8_t)((Address & 0xFF00) >> 8));

    /*!< Test on EV8 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(11.5);
    }

    /*!< Send the EEPROM's internal address to read from: LSB of the address */
    I2C_SendData(AT24C_I2Cx, (uint8_t)(ReadAddr & 0x00FF));

#endif /*!< AT24CXX_16BIT_ADDR */

    /*!< Test on EV8 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(I2C_GetFlagStatus(AT24C_I2Cx, I2C_FLAG_BTF) == RESET)
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(12);
    }

    /*!< Send STRAT condition a second time */
    I2C_GenerateSTART(AT24C_I2Cx, ENABLE);

    /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(13);
    }

    /*!< Send EEPROM address for read */
    I2C_Send7bitAddress(AT24C_I2Cx, s_AT24Cxx_Addr, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(14);
    }

    /* Disable Acknowledgement */
    I2C_AcknowledgeConfig(AT24C_I2Cx, DISABLE);

    /* Send STOP Condition */
    I2C_GenerateSTOP(AT24C_I2Cx, ENABLE);

    /* Test on EV7 and clear it */
    AT24C_TimeOut = MAX_LONGTIME_OUT;

    while(I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(15);
    }
    /* Read a byte from the EEPROM */
    temp = I2C_ReceiveData(AT24C_I2Cx);

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(AT24C_I2Cx, ENABLE);

    /* If all operations OK, return receive data */
    return temp;
	
#endif /* USE_SIMULATE_IIC */
}

/************************************************
�������� �� AT24Cxx_Page_Program
��    �� �� AT24Cxxҳ���
��    �� �� pBuffer ---- ����
			Address ---- ��ַ
			Len ---- ����
�� �� ֵ �� 0 / 1
*************************************************/
uint8_t AT24Cxx_Page_Program( uint8_t *pBuffer, uint16_t Address, uint16_t Len )
{

#if USE_SIMULATE_IIC
	return EE_Write_Buffer(s_AT24Cxx_Addr, pBuffer, Address, Len);

#else
    /* Set the pointer to the Number of data to be written. This pointer will be used
      by the DMA Transfer Completer interrupt Handler in order to reset the
      variable to 0. User should check on this variable in order to know if the
      DMA transfer has been complete or not. */
    g_EEData_WritePointer = Len;

    /* While the bus is busy */
    AT24C_TimeOut = MAX_LONGTIME_OUT;
    while(I2C_GetFlagStatus(AT24C_I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(4);
    }

    /* Send START condition */
    I2C_GenerateSTART(AT24C_I2Cx, ENABLE);

    /* Test on EV5 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(5);
    }

    /* Send EEPROM address for write */
    AT24C_TimeOut = MAX_TIME_OUT;
    I2C_Send7bitAddress(AT24C_I2Cx, s_AT24Cxx_Addr, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(6);
    }

#ifndef AT24CXX_16BIT_ADDR

    /* Send the EEPROM's internal address to write to : only one byte Address */
    I2C_SendData(AT24C_I2Cx, (uint8_t)(Address & 0x00FF));

#else

    /* Send the EEPROM's internal address to write to : MSB of the address first */
    I2C_SendData(AT24C_I2Cx, (uint8_t)((Address & 0xFF00) >> 8));

    /* Test on EV8 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(6.5);
    }

    /*!< Send the EEPROM's internal address to write to : LSB of the address */
    I2C_SendData(AT24C_I2Cx, (uint8_t)(Address & 0x00FF));

#endif /* AT24CXX_16BIT_ADDR */

    /* Test on EV8 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(7);
    }

#if (0 == IIC_DMA_ENABLE)
    /* While there is data to be written */
    while(Len--)
    {
        /* Send the current byte */
        I2C_SendData(AT24C_I2Cx, *pBuffer);

        /* Point to the next byte to be written */
        pBuffer++;

        /* Test on EV8 and clear it */
        AT24C_TimeOut = MAX_TIME_OUT;
        while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            if(0 == (AT24C_TimeOut--))
                return TimeOut_Callback(8);
        }
    }

    /* Send STOP condition */
    I2C_GenerateSTOP(AT24C_I2Cx, ENABLE);

#else
    /* Configure the DMA Tx Channel with the buffer address and the buffer size */
    IIC_DMA_Config((uint32_t)pBuffer, Len, EE_DIRECTION_TX);

    /* Enable the DMA Tx Channel */
    DMA_Cmd(EE_I2C_DMA_CHANNEL_TX, ENABLE);

#endif /* IIC_DMA_ENABLE */

    /* If all operations OK, return 1 */
    return 1;

#endif /* USE_SIMULATE_IIC */
}

/************************************************
�������� �� AT24Cxx_Write_EEPROM
��    �� �� �� AT24Cxx��д�����ݿ�
��    �� �� pBuffer ---- ����
			Address ---- ��ַ
			Len ---- ����
�� �� ֵ �� ��
*************************************************/
void AT24Cxx_Write_EEPROM( uint8_t *pBuffer, uint16_t Address, uint16_t Len )
{
    uint8_t NumOfPage = 0, NumOfSingle = 0;
    uint8_t Addr = 0, count = 0, temp = 0;

    /* mod�������࣬�� Address�� AT24C_PAGE_SIZE�������������� AddrֵΪ 0 */
    Addr = Address % AT24C_PAGE_SIZE;

    /* ��count������ֵ���պÿ��Զ��뵽ҳ��ַ */
    count = AT24C_PAGE_SIZE - Addr;

    /* �����Ҫд��������ҳ */
    NumOfPage =  Len / AT24C_PAGE_SIZE;

    /* �����ʣ�಻��һҳ���ֽ��� */
    NumOfSingle = Len % AT24C_PAGE_SIZE;

    /* Addr = 0,�� Address�պð�ҳ���� */
    if(Addr == 0)
    {
        /* Len <= AT24C_PAGE_SIZE */
        if(0 == NumOfPage)
        {
            /* ����һҳ or �պ�һҳ */
            AT24Cxx_Page_Program(pBuffer, Address, Len);
            EE_DMA_TxWait();
            AT24Cxx_Busy_Wait();
        }
        else /* Len > AT24C_PAGE_SIZE */
        {
            /* �Ȱ�����ҳ�Ķ�д�� */
            while(NumOfPage--)
            {
                AT24Cxx_Page_Program(pBuffer, Address, AT24C_PAGE_SIZE);
                EE_DMA_TxWait();
                AT24Cxx_Busy_Wait();
                Address += AT24C_PAGE_SIZE;
                pBuffer += AT24C_PAGE_SIZE;
            }
            /* ���ж���Ĳ���һҳ�����ݣ���һҳ����д�� */
            if(NumOfSingle != 0)
            {
                AT24Cxx_Page_Program(pBuffer, Address, NumOfSingle);
                EE_DMA_TxWait();
                AT24Cxx_Busy_Wait();
            }
        }
    }
    /* ����ַ�� AT24C_PAGE_SIZE������  */
    else
    {
        /* Len < AT24C_PAGE_SIZE */
        if(NumOfPage == 0)
        {
            /* ��ǰҳʣ��� count��λ�ñ� NumOfSingleС��һҳд���� */
            if(NumOfSingle > count)
            {
                /* ��д����ǰҳ */
                AT24Cxx_Page_Program(pBuffer, Address, count);
                EE_DMA_TxWait();
                AT24Cxx_Busy_Wait();

                temp = NumOfSingle - count;
                Address += count;
                pBuffer += count;
                /* ��дʣ������� */
                AT24Cxx_Page_Program(pBuffer, Address, temp);
                EE_DMA_TxWait();
                AT24Cxx_Busy_Wait();
            }
            else /* ��ǰҳʣ��� count��λ����д�� NumOfSingle������ */
            {
                AT24Cxx_Page_Program(pBuffer, Address, Len);
                EE_DMA_TxWait();
                AT24Cxx_Busy_Wait();
            }
        }
        else /* Len > AT24C_PAGE_SIZE */
        {
            /* ��ַ���������� count�ֿ������������������ */
            Len -= count;
            NumOfPage =  Len / AT24C_PAGE_SIZE;
            NumOfSingle = Len % AT24C_PAGE_SIZE;

            if(count != 0)
            {
                /* ��д��count�����ݣ�Ϊ��������һ��Ҫд�ĵ�ַ���� */
                AT24Cxx_Page_Program(pBuffer, Address, count);
                EE_DMA_TxWait();
                AT24Cxx_Busy_Wait();
                /* ���������ظ���ַ�������� */
                Address += count;
                pBuffer += count;
            }

            /* ������ҳ��д�� */
            while(NumOfPage--)
            {
                AT24Cxx_Page_Program(pBuffer, Address, AT24C_PAGE_SIZE);
                EE_DMA_TxWait();
                AT24Cxx_Busy_Wait();
                Address +=  AT24C_PAGE_SIZE;
                pBuffer += AT24C_PAGE_SIZE;
            }
            /* ���ж���Ĳ���һҳ�����ݣ�����д��*/
            if(NumOfSingle != 0)
            {
                AT24Cxx_Page_Program(pBuffer, Address, NumOfSingle);
                EE_DMA_TxWait();
                AT24Cxx_Busy_Wait();
            }
        }
    }
}

/************************************************
�������� �� AT24Cxx_Read_EEPROM
��    �� �� �� AT24Cxx�ж�ȡ���ݿ�
��    �� �� pBuffer ---- ����
			Address ---- ��ַ
			Len ---- ����
�� �� ֵ �� 0 / 1
*************************************************/
uint8_t AT24Cxx_Read_EEPROM( uint8_t *pBuffer, uint16_t Address, uint16_t Len )
{

#if USE_SIMULATE_IIC
	return EE_Read_Buffer(s_AT24Cxx_Addr, pBuffer, Address, Len);
	
#else
    /* Set the pointer to the Number of data to be read. This pointer will be used
      by the DMA Transfer Completer interrupt Handler in order to reset the
      variable to 0. User should check on this variable in order to know if the
      DMA transfer has been complete or not. */
    g_EEData_ReadPointer = Len;

    /*!< While the bus is busy */
    AT24C_TimeOut = MAX_LONGTIME_OUT;
    while(I2C_GetFlagStatus(AT24C_I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(9);
    }

    /*!< Send START condition */
    I2C_GenerateSTART(AT24C_I2Cx, ENABLE);

    /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(10);
    }

    /*!< Send EEPROM address for write */
    I2C_Send7bitAddress(AT24C_I2Cx, s_AT24Cxx_Addr, I2C_Direction_Transmitter);

    /*!< Test on EV6 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(11);
    }

    /* Clear EV6 by setting again the PE bit */
    I2C_Cmd(AT24C_I2Cx, ENABLE);

#ifndef AT24CXX_16BIT_ADDR

    /*!< Send the EEPROM's internal address to read from: Only one byte address */
    I2C_SendData(AT24C_I2Cx, (uint8_t)(Address & 0x00FF));

#else

    /*!< Send the EEPROM's internal address to read from: MSB of the address first */
    I2C_SendData(AT24C_I2Cx, (uint8_t)((Address & 0xFF00) >> 8));

    /*!< Test on EV8 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(11.5);
    }

    /*!< Send the EEPROM's internal address to read from: LSB of the address */
    I2C_SendData(AT24C_I2Cx, (uint8_t)(ReadAddr & 0x00FF));

#endif /*!< AT24CXX_16BIT_ADDR */

    /*!< Test on EV8 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(I2C_GetFlagStatus(AT24C_I2Cx, I2C_FLAG_BTF) == RESET)
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(12);
    }

    /*!< Send STRAT condition a second time */
    I2C_GenerateSTART(AT24C_I2Cx, ENABLE);

    /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(13);
    }

    /*!< Send EEPROM address for read */
    I2C_Send7bitAddress(AT24C_I2Cx, s_AT24Cxx_Addr, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(14);
    }

    /* While there is data to be read */
    while(Len)
    {
        if(Len == 1)
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(AT24C_I2Cx, DISABLE);

            /* Call User callback for critical section start (should typically disable interrupts) */
            EE_EnterCriticalSection_UserCallback();

            /* Send STOP Condition */
            I2C_GenerateSTOP(AT24C_I2Cx, ENABLE);

            /* Call User callback for critical section end (should typically re-enable interrupts) */
            EE_ExitCriticalSection_UserCallback();

	#if (0 == IIC_DMA_ENABLE)
        }

        /* Test on EV7 and clear it */
        AT24C_TimeOut = MAX_LONGTIME_OUT;

        while(I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)
        {
            if(0 == (AT24C_TimeOut--))
                return TimeOut_Callback(15);
        }

        /* Read a byte from the EEPROM */
        *pBuffer = I2C_ReceiveData(AT24C_I2Cx);

        /* Point to the next location where the byte read will be saved */
        pBuffer++;

        /* Decrement the read bytes counter */
        Len--;
    }

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(AT24C_I2Cx, ENABLE);

	#else
            /* Wait for the byte to be received */
            AT24C_TimeOut = MAX_LONGTIME_OUT;

            while(I2C_GetFlagStatus(AT24C_I2Cx, I2C_FLAG_RXNE)==0)
            {
                if(0 == (AT24C_TimeOut--))
                    return TimeOut_Callback(16);
            }
            /* Read a byte from the EEPROM */
            *pBuffer = I2C_ReceiveData(AT24C_I2Cx);

            /* Decrement the read bytes counter */
            Len--;

            /* Wait to make sure that STOP control bit has been cleared */
            AT24C_TimeOut = MAX_TIME_OUT;
            while(AT24C_I2Cx->CR1 & I2C_CR1_STOP)
            {
                if(0 == (AT24C_TimeOut--))
                    return TimeOut_Callback(17);
            }

            /*!< Re-Enable Acknowledgement to be ready for another reception */
            I2C_AcknowledgeConfig(AT24C_I2Cx, ENABLE);
        }
        else
        {
            /* Configure the DMA Rx Channel with the buffer address and the buffer size */
            IIC_DMA_Config((uint32_t)pBuffer, Len, EE_DIRECTION_RX);

            /* Inform the DMA that the next End Of Transfer Signal will be the last one */
            I2C_DMALastTransferCmd(AT24C_I2Cx, ENABLE);

            /* Enable the DMA Rx Channel */
            DMA_Cmd(EE_I2C_DMA_CHANNEL_RX, ENABLE);
			
			break;
        }
    }

	#endif /* IIC_DMA_ENABLE */

    /* If all operations OK, return 1 */
    return 1;

#endif /* USE_SIMULATE_IIC */
}


#if (IIC_DMA_ENABLE && 0 == USE_SIMULATE_IIC)

/************************************************
�������� �� IIC_NVIC_Config
��    �� �� ����Ƕ�������жϿ����� NVIC
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
static void IIC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure and enable I2C DMA TX Channel interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EE_I2C_DMA_TX_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EE_I2C_DMA_PREPRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = EE_I2C_DMA_SUBPRIO;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure and enable I2C DMA RX Channel interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EE_I2C_DMA_RX_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EE_I2C_DMA_PREPRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = EE_I2C_DMA_SUBPRIO;
    NVIC_Init(&NVIC_InitStructure);
}

/************************************************
�������� �� IIC_DMA_Init
��    �� �� ����Ƕ�������жϿ����� NVIC
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
static void IIC_DMA_Init(void)
{
    /* Enable the DMA clock */
    RCC_AHBPeriphClockCmd(EE_I2C_DMA_CLK, ENABLE);

    /* I2C DMA TX and RX channels configuration */
    s_EEDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)EE_I2C_DR_Address;
    s_EEDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;   /* This parameter will be configured durig communication */
    s_EEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;    /* This parameter will be configured durig communication */
    s_EEDMA_InitStructure.DMA_BufferSize = 0xFFFF;            /* This parameter will be configured durig communication */
    s_EEDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    s_EEDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    s_EEDMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
    s_EEDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    s_EEDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    s_EEDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;		// ������ȼ�
    s_EEDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    /* I2C TX DMA Channel configuration */
    DMA_DeInit(EE_I2C_DMA_CHANNEL_TX);
    DMA_Init(EE_I2C_DMA_CHANNEL_TX, &s_EEDMA_InitStructure);

    /* I2C RX DMA Channel configuration */
    DMA_DeInit(EE_I2C_DMA_CHANNEL_RX);
    DMA_Init(EE_I2C_DMA_CHANNEL_RX, &s_EEDMA_InitStructure);

    /* Enable the DMA Channels Interrupts */
    DMA_ITConfig(EE_I2C_DMA_CHANNEL_TX, DMA_IT_TC, ENABLE);
    DMA_ITConfig(EE_I2C_DMA_CHANNEL_RX, DMA_IT_TC, ENABLE);
}

/************************************************
�������� �� IIC_DMA_Config
��    �� �� ͨѶ�ڼ�� DMA����
��    �� �� Address ---- �����ַ
			BufferSize ---- �������ݴ�С
			Direction ---- EE_DIRECTION_TX or EE_DIRECTION_RX
�� �� ֵ �� ��
*************************************************/
static void IIC_DMA_Config( uint32_t Address, uint32_t BufferSize, uint32_t Direction )
{
    /* Initialize the DMA with the new parameters */
    if (Direction == EE_DIRECTION_TX)
    {
        /* Configure the DMA Tx Channel with the buffer address and the buffer size */
        s_EEDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Address;
        s_EEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        s_EEDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;
        DMA_Init(EE_I2C_DMA_CHANNEL_TX, &s_EEDMA_InitStructure);
    }
    else
    {
        /* Configure the DMA Rx Channel with the buffer address and the buffer size */
        s_EEDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Address;
        s_EEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        s_EEDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;
        DMA_Init(EE_I2C_DMA_CHANNEL_RX, &s_EEDMA_InitStructure);
    }
}

#endif /* IIC_DMA_ENABLE && 0 == USE_SIMULATE_IIC */

/************************************************
�������� �� AT24Cxx_Config
��    �� �� AT24Cxx����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void AT24Cxx_Config(void)
{

#if USE_SIMULATE_IIC
	EE_IIC_Init();

#else
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    /* AT24C_I2Cx IO Periph clock enable */
    AT24C_IO_APBxClock_FUN(AT24C_SCL_CLK | AT24C_SDA_CLK, ENABLE);

    /* AT24C_I2Cx Periph clock enable */
    AT24C_I2C_APBxClock_FUN(AT24C_I2C_CLK, ENABLE);

    /* Configure AT24C_I2Cx pins: SCL, SDA */
    /* Confugure SCL and SDA pins as Alternate Function Open Drain Output */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;

    GPIO_InitStructure.GPIO_Pin = AT24C_SCL_PINS;
    GPIO_Init(AT24C_SCL_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = AT24C_SDA_PINS;
    GPIO_Init(AT24C_SDA_PORT, &GPIO_InitStructure);

#if IIC_DMA_ENABLE
    /* DMA selective data processing */
    IIC_NVIC_Config();
    IIC_DMA_Init();

#endif /* IIC_DMA_ENABLE */

    /* AT24C_I2Cx configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS7;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
    I2C_Init(AT24C_I2Cx, &I2C_InitStructure);

    /* Enable AT24C_I2Cx */
    I2C_Cmd(AT24C_I2Cx, ENABLE);

#if IIC_DMA_ENABLE
    /* Enable the AT24C_I2Cx peripheral DMA requests */
    I2C_DMACmd(AT24C_I2Cx, ENABLE);

#endif /* IIC_DMA_ENABLE */

#endif /* USE_SIMULATE_IIC */
}

/************************************************
�������� �� AT24Cxx_Init
��    �� �� AT24Cxx��ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void AT24Cxx_Init(void)
{
    AT24Cxx_Config();

    /* depending on the EEPROM Address selected in the at24cxx.h file */
#ifdef EEPROM_BLOCK0_ADDRESS
    /*!< Select the EEPROM Block0 to write on */
    s_AT24Cxx_Addr = EEPROM_BLOCK0_ADDRESS;
#endif

#ifdef EEPROM_BLOCK1_ADDRESS
    /*!< Select the EEPROM Block1 to write on */
    s_AT24Cxx_Addr = EEPROM_BLOCK1_ADDRESS;
#endif

#ifdef EEPROM_BLOCK2_ADDRESS
    /*!< Select the EEPROM Block2 to write on */
    s_AT24Cxx_Addr = EEPROM_BLOCK2_ADDRESS;
#endif

#ifdef EEPROM_BLOCK3_ADDRESS
    /*!< Select the EEPROM Block3 to write on */
    s_AT24Cxx_Addr = EEPROM_BLOCK3_ADDRESS;
#endif

#if USE_SIMULATE_IIC
	if(0 == EE_IIC_Check(s_AT24Cxx_Addr))
	{
		AT24C_DEBUG_PRINTF("The EEPROM device could not be found\n");
	}
	
#endif /* USE_SIMULATE_IIC */
	
	EE_Test();
}

/************************************************
�������� �� EE_Test
��    �� �� EEPROM���Ժ���
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
static uint8_t EE_Test(void)
{
	
#if _EE_TEST
	uint8_t I2c_Buf_Write[256] = {0};
	uint8_t I2c_Buf_Read[256] = {0};
	uint16_t i;

	/* ���ֽڶ�д���� */
	AT24C_DEBUG_PRINTF("���ֽڶ�д����\n");
	if(AT24Cxx_Write_Byte(0xBB, USE_TEST_ADDR + 0x200))
	{
		AT24Cxx_Busy_Wait();

#if USE_SIMULATE_IIC
		Delay(0xFFF);		// ģ���д��Ҫ�ȴ�һ��ʱ�䣬��������

#endif /* USE_SIMULATE_IIC */
		
		AT24C_DEBUG_PRINTF("data:0x%02X\n",AT24Cxx_Read_Byte(USE_TEST_ADDR + 0x200));
		AT24Cxx_Busy_Wait();
	}

#if 1
	/* ���ֽڶ�д���� */
	AT24C_DEBUG_PRINTF("ҳ��д����\n");

	for (i = 0;i < 256;i++) // ��仺��
	{
		I2c_Buf_Write[i] = i;
	}
	
	I2c_Buf_Write[0] = 0xAA;

	// �� I2c_Buf_Write��˳�����������д�� EERPOM��
	AT24Cxx_Write_EEPROM(I2c_Buf_Write, USE_TEST_ADDR, 256);
	
	// д�뺯���Ѿ�������busy�����
//	EE_DMA_RxWait();
//	AT24Cxx_Busy_Wait();

#if USE_SIMULATE_IIC
	Delay(0xFFF);			// ģ���д��Ҫ�ȴ�һ��ʱ�䣬��������

#endif /* USE_SIMULATE_IIC */
	
	// �� EEPROM��������˳�򱣳ֵ� I2c_Buf_Read��
	AT24Cxx_Read_EEPROM(I2c_Buf_Read, USE_TEST_ADDR, 256);
		
	EE_DMA_RxWait();
	AT24Cxx_Busy_Wait();
   
	// �� I2c_Buf_Read�е�����ͨ�����ڴ�ӡ
	for (i = 0;i < 256;i++)
	{
		if(I2c_Buf_Read[i] != I2c_Buf_Write[i])
		{
			AT24C_DEBUG_PRINTF("0x%02X , i = %d\n", I2c_Buf_Read[i], i);
			AT24C_DEBUG_PRINTF("����:I2C EEPROMд������������ݲ�һ��\n\r");
			return 0;
		}
		printf("0x%02X ", I2c_Buf_Read[i]);
		if(i%11 == 10 || i == 255)
			printf("\n\r");    
	}
	AT24C_DEBUG_PRINTF("I2C(AT24C02)��д���Գɹ�\n\r");

#endif
	
#endif /* _EE_TEST */
	
	return 1;
}


#if (0 == USE_SIMULATE_IIC && IIC_DMA_ENABLE)

/************************************************************************/
/*            STM32F10x I2C DMA Interrupt Handlers                      */
/************************************************************************/

/**
  * @brief  This function handles the DMA Tx Channel interrupt Handler.
  * @param  None
  * @retval None
  */
void EE_I2C_DMA_TX_IRQHandler(void)
{
    /* Check if the DMA transfer is complete */
    if(DMA_GetFlagStatus(EE_I2C_DMA_FLAG_TX_TC) != RESET)
    {
        /* Disable the DMA Tx Channel and Clear all its Flags */
        DMA_Cmd(EE_I2C_DMA_CHANNEL_TX, DISABLE);
        DMA_ClearFlag(EE_I2C_DMA_FLAG_TX_GL);

        /*!< Wait till all data have been physically transferred on the bus */
        AT24C_TimeOut = MAX_LONGTIME_OUT;
        while(!I2C_GetFlagStatus(AT24C_I2Cx, I2C_FLAG_BTF))
        {
            if((AT24C_TimeOut--) == 0) TimeOut_Callback(3);
        }

        /*!< Send STOP condition */
        I2C_GenerateSTOP(AT24C_I2Cx, ENABLE);

        /* Perform a read on SR1 and SR2 register to clear eventualaly pending flags */
        (void)AT24C_I2Cx->SR1;
        (void)AT24C_I2Cx->SR2;

        /* Reset the variable holding the number of data to be written */
        g_EEData_WritePointer = 0;
    }
}

/**
  * @brief  This function handles the DMA Rx Channel interrupt Handler.
  * @param  None
  * @retval None
  */
void EE_I2C_DMA_RX_IRQHandler(void)
{
    /* Check if the DMA transfer is complete */
    if(DMA_GetFlagStatus(EE_I2C_DMA_FLAG_RX_TC) != RESET)
    {
        /*!< Send STOP Condition */
        I2C_GenerateSTOP(AT24C_I2Cx, ENABLE);

        /* Disable the DMA Rx Channel and Clear all its Flags */
        DMA_Cmd(EE_I2C_DMA_CHANNEL_RX, DISABLE);
        DMA_ClearFlag(EE_I2C_DMA_FLAG_RX_GL);

        /* Reset the variable holding the number of data to be read */
        g_EEData_ReadPointer = 0;
    }
}

#endif /* 0 == USE_SIMULATE_IIC && IIC_DMA_ENABLE */


#ifdef USE_DEFAULT_CRITICAL_CALLBACK
/**
  * @brief  Start critical section: these callbacks should be typically used
  *         to disable interrupts when entering a critical section of I2C communication
  *         You may use default callbacks provided into this driver by uncommenting the
  *         define USE_DEFAULT_CRITICAL_CALLBACK.
  *         Or you can comment that line and implement these callbacks into your
  *         application.
  * @param  None.
  * @retval None.
  */
void EE_EnterCriticalSection_UserCallback(void)
{
    __disable_irq();
}

/**
  * @brief  Start and End of critical section: these callbacks should be typically used
  *         to re-enable interrupts when exiting a critical section of I2C communication
  *         You may use default callbacks provided into this driver by uncommenting the
  *         define USE_DEFAULT_CRITICAL_CALLBACK.
  *         Or you can comment that line and implement these callbacks into your
  *         application.
  * @param  None.
  * @retval None.
  */
void EE_ExitCriticalSection_UserCallback(void)
{
    __enable_irq();
}
#endif /* USE_DEFAULT_CRITICAL_CALLBACK */


/*---------------------------- END OF FILE ----------------------------*/


