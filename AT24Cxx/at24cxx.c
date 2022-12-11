#include "./AT24Cxx/at24cxx.h"
#include "./AT24Cxx/ee_simulate.h"
#include "bsp.h"
#include "bsp_uart.h"


/* 注：STM32的硬件 I2C的确有一些 BUG，或者说使用时有很多不方便满足的要求，	\
       比如 DMA加最高中断（不使用该模式容易出现死机） */

/* 是否启用模拟 IIC \
    note: 只适用 7-bit data word address(即标准 I2C) */
#define USE_SIMULATE_IIC		0

/* 若使用硬件 IIC，建议使用 DMA功能 */
#define IIC_DMA_ENABLE			1

/* EEPROM读写测试宏 */
#define _EE_TEST				1
#define USER_TEST_ADDR			68


/* Uncomment this line to use the default start and end of critical section
   callbacks (it disables then enabled all interrupts) */
#define USE_DEFAULT_CRITICAL_CALLBACK
/* Start and End of critical section: these callbacks should be typically used
   to disable interrupts when entering a critical section of I2C communication
   You may use default callbacks provided into this driver by uncommenting the
   define USE_DEFAULT_CRITICAL_CALLBACK.
   Or you can comment that line and implement these callbacks into your
   application */

#define _AT24C_DEBUG        1
#define AT24C_DEBUG_PRINTF(fmt,arg...)      do{\
                                            if(_AT24C_DEBUG)\
                                                printf("<<-EEPROM-DEBUG->> < %s >[%d]\n"fmt"\n",__FILE__,__LINE__, ##arg);\
                                            }while(0)

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


__attribute__((weak)) void Delay(uint32_t Cnt)
{
    uint8_t i, j;

    while (Cnt--)
    {
        for (i = 2; i > 0; i--)
            for (j = 43; j > 0; j--)
                continue;
    }
}

/************************************************
函数名称 ： TimeOut_Callback
功    能 ： 等待超时回调函数
参    数 ： ErrorCode ---- 错误代号
返 回 值 ： 错误值 0
*************************************************/
static uint8_t TimeOut_Callback( char ErrorCode )
{
    /* 等待超时后的处理,输出错误信息 */
    AT24C_DEBUG_PRINTF("IIC 等待超时!  EerrorCode = %d\n",ErrorCode);

    return 0;
}

/************************************************
函数名称 ： EE_DMA_TxWait
功    能 ： DMA发送完成等待
参    数 ： 无
返 回 值 ： 无
备    注 :  DMA模式下，发送完需调用此函数检查（单字节除外）
*************************************************/
void EE_DMA_TxWait(void)
{

#if IIC_DMA_ENABLE
    /* Wait transfer through DMA to be complete */
    AT24C_TimeOut = MAX_LONGTIME_OUT;
    while (g_EEData_WritePointer > 0)
    {
        Delay(3);
        if((AT24C_TimeOut--) == 0) {
            TimeOut_Callback(19);
            return;
        }
    }

#endif /* IIC_DMA_ENABLE */
}

/************************************************
函数名称 ： EE_DMA_RxWait
功    能 ： DMA接收完成等待
参    数 ： 无
返 回 值 ： 无
备    注 :  DMA模式下，接收完需调用此函数检查（单字节除外）
*************************************************/
void EE_DMA_RxWait(void)
{

#if IIC_DMA_ENABLE
    /* Wait transfer through DMA to be complete */
    AT24C_TimeOut = MAX_LONGTIME_OUT;
    while (g_EEData_ReadPointer > 0)
    {
        Delay(3);
        if((AT24C_TimeOut--) == 0) {
            TimeOut_Callback(20);
            return;
        }
    }

#endif /* IIC_DMA_ENABLE */
}

/************************************************
函数名称 ： AT24Cxx_Busy_Wait
功    能 ： AT24Cxx忙等待
参    数 ： 无
返 回 值 ： 0 / 1
备    注 :  硬件模式下读写后必需调用此函数检查
*************************************************/
static uint8_t AT24Cxx_Busy_Wait(void)
{
    uint8_t temp = 1;

#if (0 == USE_SIMULATE_IIC)
    __IO uint16_t temp_SR1;

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
函数名称 ： AT24Cxx_Write_Byte
功    能 ： AT24Cxx写一个字节
参    数 ： Byte ---- 数据
			Address ---- 地址
返 回 值 ： 0 / 1
*************************************************/
uint8_t AT24Cxx_Write_Byte( uint8_t Byte, uint16_t Address )
{
#ifndef AT24CXX_16BIT_ADDR
    if(AT24C_MEMORY_CAPACITY > 256)
        s_AT24Cxx_Addr = EEPROM_BLOCK0_ADDRESS | ((Address % AT24C_MEMORY_CAPACITY) >= 256 ? (AT24C_MEMORY_CAPACITY / 256) & 0xFE : 0);
#endif /* AT24CXX_16BIT_ADDR */

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
函数名称 ： AT24Cxx_Read_Byte
功    能 ： AT24Cxx读一个字节
参    数 ： Address ---- 地址
返 回 值 ： temp ---- 数据
*************************************************/
uint8_t AT24Cxx_Read_Byte( uint16_t Address )
{
#ifndef AT24CXX_16BIT_ADDR
    if(AT24C_MEMORY_CAPACITY > 256)
        s_AT24Cxx_Addr = EEPROM_BLOCK0_ADDRESS | ((Address % AT24C_MEMORY_CAPACITY) >= 256 ? (AT24C_MEMORY_CAPACITY / 256) & 0xFE : 0);
#endif /* AT24CXX_16BIT_ADDR */

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
    I2C_SendData(AT24C_I2Cx, (uint8_t)(Address & 0x00FF));

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
函数名称 ： AT24Cxx_Page_Program
功    能 ： AT24Cxx页编程
参    数 ： pBuffer ---- 数据
			Address ---- 地址
			Len ---- 长度
返 回 值 ： 0 / 1
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
    DMA_Cmd(EE_I2C_DMA_STREAM_TX, ENABLE);

#endif /* IIC_DMA_ENABLE */

    /* If all operations OK, return 1 */
    return 1;

#endif /* USE_SIMULATE_IIC */
}

/************************************************
函数名称 ： AT24Cxx_Write_EEPROM
功    能 ： 从 AT24Cxx中写入数据块
参    数 ： pBuffer ---- 数据
			Address ---- 地址
			Len ---- 长度
返 回 值 ： 无
*************************************************/
void AT24Cxx_Write_EEPROM( uint8_t *pBuffer, uint16_t Address, uint16_t Len )
{
    uint8_t NumOfPage = 0, NumOfSingle = 0;
    uint8_t Addr = 0, count = 0, temp = 0;

    /* mod运算求余，若 Address是 AT24C_PAGE_SIZE整数倍，运算结果 Addr值为 0 */
    Addr = Address % AT24C_PAGE_SIZE;

    /* 差count个数据值，刚好可以对齐到页地址 */
    count = AT24C_PAGE_SIZE - Addr;

    /* 计算出要写多少整数页 */
    NumOfPage =  Len / AT24C_PAGE_SIZE;

    /* 计算出剩余不满一页的字节数 */
    NumOfSingle = Len % AT24C_PAGE_SIZE;

    /* Addr = 0,则 Address刚好按页对齐 */
    if(Addr == 0)
    {
    #ifndef AT24CXX_16BIT_ADDR
        if(AT24C_MEMORY_CAPACITY > 256)
            s_AT24Cxx_Addr = EEPROM_BLOCK0_ADDRESS | ((Address % AT24C_MEMORY_CAPACITY) >= 256 ? (AT24C_MEMORY_CAPACITY / 256) & 0xFE : 0);
    #endif /* AT24CXX_16BIT_ADDR */

        /* Len <= AT24C_PAGE_SIZE */
        if(0 == NumOfPage)
        {
            /* 不到一页 or 刚好一页 */
            AT24Cxx_Page_Program(pBuffer, Address, Len);
            EE_DMA_TxWait();
            AT24Cxx_Busy_Wait();
        }
        else /* Len > AT24C_PAGE_SIZE */
        {
            /* 先把整数页的都写了 */
            while(NumOfPage--)
            {
                AT24Cxx_Page_Program(pBuffer, Address, AT24C_PAGE_SIZE);
                EE_DMA_TxWait();
                AT24Cxx_Busy_Wait();
                Address += AT24C_PAGE_SIZE;
                pBuffer += AT24C_PAGE_SIZE;

            #ifndef AT24CXX_16BIT_ADDR
                if(AT24C_MEMORY_CAPACITY > 256)
                    s_AT24Cxx_Addr = EEPROM_BLOCK0_ADDRESS | ((Address % AT24C_MEMORY_CAPACITY) >= 256 ? (AT24C_MEMORY_CAPACITY / 256) & 0xFE : 0);
            #endif /* AT24CXX_16BIT_ADDR */
            }
            /* 若有多余的不满一页的数据，下一页把它写完 */
            if(NumOfSingle != 0)
            {
                AT24Cxx_Page_Program(pBuffer, Address, NumOfSingle);
                EE_DMA_TxWait();
                AT24Cxx_Busy_Wait();
            }
        }
    }
    /* 若地址与 AT24C_PAGE_SIZE不对齐  */
    else
    {
    #ifndef AT24CXX_16BIT_ADDR
        if(AT24C_MEMORY_CAPACITY > 256)
            s_AT24Cxx_Addr = EEPROM_BLOCK0_ADDRESS | ((Address % AT24C_MEMORY_CAPACITY) >= 256 ? (AT24C_MEMORY_CAPACITY / 256) & 0xFE : 0);
    #endif /* AT24CXX_16BIT_ADDR */

        /* Len < AT24C_PAGE_SIZE */
        if(NumOfPage == 0)
        {
            /* 当前页剩余的 count个位置比 NumOfSingle小，一页写不完 */
            if(NumOfSingle > count)
            {
                /* 先写满当前页 */
                AT24Cxx_Page_Program(pBuffer, Address, count);
                EE_DMA_TxWait();
                AT24Cxx_Busy_Wait();

                temp = NumOfSingle - count;
                Address += count;
                pBuffer += count;

            #ifndef AT24CXX_16BIT_ADDR
                if(AT24C_MEMORY_CAPACITY > 256)
                    s_AT24Cxx_Addr = EEPROM_BLOCK0_ADDRESS | ((Address % AT24C_MEMORY_CAPACITY) >= 256 ? (AT24C_MEMORY_CAPACITY / 256) & 0xFE : 0);
            #endif /* AT24CXX_16BIT_ADDR */

                /* 再写剩余的数据 */
                AT24Cxx_Page_Program(pBuffer, Address, temp);
                EE_DMA_TxWait();
                AT24Cxx_Busy_Wait();
            }
            else /* 当前页剩余的 count个位置能写完 NumOfSingle个数据 */
            {
                AT24Cxx_Page_Program(pBuffer, Address, Len);
                EE_DMA_TxWait();
                AT24Cxx_Busy_Wait();
            }
        }
        else /* Len > AT24C_PAGE_SIZE */
        {
            /* 地址不对齐多出的 count分开处理，不加入这个运算 */
            Len -= count;
            NumOfPage =  Len / AT24C_PAGE_SIZE;
            NumOfSingle = Len % AT24C_PAGE_SIZE;

            if(count != 0)
            {
                /* 先写完count个数据，为的是让下一次要写的地址对齐 */
                AT24Cxx_Page_Program(pBuffer, Address, count);
                EE_DMA_TxWait();
                AT24Cxx_Busy_Wait();
                /* 接下来就重复地址对齐的情况 */
                Address += count;
                pBuffer += count;
            }

        #ifndef AT24CXX_16BIT_ADDR
            if(AT24C_MEMORY_CAPACITY > 256)
                s_AT24Cxx_Addr = EEPROM_BLOCK0_ADDRESS | ((Address % AT24C_MEMORY_CAPACITY) >= 256 ? (AT24C_MEMORY_CAPACITY / 256) & 0xFE : 0);
        #endif /* AT24CXX_16BIT_ADDR */

            /* 把整数页都写了 */
            while(NumOfPage--)
            {
                AT24Cxx_Page_Program(pBuffer, Address, AT24C_PAGE_SIZE);
                EE_DMA_TxWait();
                AT24Cxx_Busy_Wait();
                Address +=  AT24C_PAGE_SIZE;
                pBuffer += AT24C_PAGE_SIZE;

            #ifndef AT24CXX_16BIT_ADDR
                if(AT24C_MEMORY_CAPACITY > 256)
                    s_AT24Cxx_Addr = EEPROM_BLOCK0_ADDRESS | ((Address % AT24C_MEMORY_CAPACITY) >= 256 ? (AT24C_MEMORY_CAPACITY / 256) & 0xFE : 0);
            #endif /* AT24CXX_16BIT_ADDR */
            }
            /* 若有多余的不满一页的数据，把它写完*/
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
函数名称 ： AT24Cxx_Read_EEPROM
功    能 ： 从 AT24Cxx中读取数据块
参    数 ： pBuffer ---- 数据
			Address ---- 地址
			Len ---- 长度
返 回 值 ： 0 / 1
*************************************************/
uint8_t AT24Cxx_Read_EEPROM( uint8_t *pBuffer, uint16_t Address, uint16_t Len )
{
#ifndef AT24CXX_16BIT_ADDR
    if(AT24C_MEMORY_CAPACITY > 256)
        s_AT24Cxx_Addr = EEPROM_BLOCK0_ADDRESS | ((Address % AT24C_MEMORY_CAPACITY) >= 256 ? (AT24C_MEMORY_CAPACITY / 256) & 0xFE : 0);
#endif /* AT24CXX_16BIT_ADDR */

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
    I2C_SendData(AT24C_I2Cx, (uint8_t)(Address & 0x00FF));

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
            while(I2C_ReadRegister(AT24C_I2Cx, I2C_Register_CR1) & I2C_CR1_STOP)
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
            DMA_Cmd(EE_I2C_DMA_STREAM_RX, ENABLE);
			
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
函数名称 ： IIC_NVIC_Config
功    能 ： 配置嵌套向量中断控制器 NVIC
参    数 ： 无
返 回 值 ： 无
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
函数名称 ： IIC_DMA_Init
功    能 ： 配置嵌套向量中断控制器 NVIC
参    数 ： 无
返 回 值 ： 无
*************************************************/
static void IIC_DMA_Init(void)
{
    /* Enable the DMA clock */
    RCC_AHB1PeriphClockCmd(EE_I2C_DMA_CLK, ENABLE);

    /* I2C DMA TX and RX configuration */    
    /* Disable the I2C Tx DMA stream */
    DMA_Cmd(EE_I2C_DMA_STREAM_TX, DISABLE);
    
    /* Disable the I2C Rx DMA stream */
    DMA_Cmd(EE_I2C_DMA_STREAM_RX, DISABLE);
    
    /* Initialize the DMA_Channel member */
    s_EEDMA_InitStructure.DMA_Channel = EE_I2C_DMA_CHANNEL;    
    /* Initialize the DMA_PeripheralBaseAddr member */
    s_EEDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)EE_I2C_DR_Address;    
    /* Initialize the DMA_PeripheralInc member */         
    s_EEDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    
    /* Initialize the DMA_MemoryInc member */
    s_EEDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;    
    /* Initialize the DMA_PeripheralDataSize member */
    s_EEDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;    
    /* Initialize the DMA_MemoryDataSize member */
    s_EEDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    
    /* Initialize the DMA_Mode member */
    s_EEDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;    
    /* Initialize the DMA_Priority member */
    s_EEDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;    
    /* Initialize the DMA_FIFOMode member */
    s_EEDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;    
    /* Initialize the DMA_FIFOThreshold member */
    s_EEDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;    
    /* Initialize the DMA_MemoryBurst member */
    s_EEDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    
    /* Initialize the DMA_PeripheralBurst member */
    s_EEDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    /* This parameter will be configured durig communication */
    s_EEDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0;   
    s_EEDMA_InitStructure.DMA_BufferSize = 0xFFFF;

    /* I2C TX DMA Channel configuration */
    s_EEDMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_DeInit(EE_I2C_DMA_STREAM_TX);
    DMA_Init(EE_I2C_DMA_STREAM_TX, &s_EEDMA_InitStructure);

    /* I2C RX DMA Channel configuration */
    s_EEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_DeInit(EE_I2C_DMA_STREAM_RX);
    DMA_Init(EE_I2C_DMA_STREAM_RX, &s_EEDMA_InitStructure);

    /* Clear any pending flag on Tx Stream  */
    DMA_ClearFlag(EE_I2C_DMA_STREAM_TX, EE_I2C_DMA_FLAG_TX_TC);
                                        
    /* Clear any pending flag on Rx Stream  */
    DMA_ClearFlag(EE_I2C_DMA_STREAM_RX, EE_I2C_DMA_FLAG_RX_TC);

    /* Enable the DMA Channels Interrupts */
    DMA_ITConfig(EE_I2C_DMA_STREAM_TX, DMA_IT_TC, ENABLE);
    DMA_ITConfig(EE_I2C_DMA_STREAM_RX, DMA_IT_TC, ENABLE);
}

/************************************************
函数名称 ： IIC_DMA_Config
功    能 ： 通讯期间的 DMA配置
参    数 ： Address ---- 缓冲地址
			BufferSize ---- 缓冲数据大小
			Direction ---- EE_DIRECTION_TX or EE_DIRECTION_RX
返 回 值 ： 无
*************************************************/
static void IIC_DMA_Config( uint32_t Address, uint32_t BufferSize, uint32_t Direction )
{
    /* Initialize the DMA with the new parameters */
    if (Direction == EE_DIRECTION_TX)
    {
        /* Configure the DMA Tx Channel with the buffer address and the buffer size */
        s_EEDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Address;
        s_EEDMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
        s_EEDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;
        DMA_Init(EE_I2C_DMA_STREAM_TX, &s_EEDMA_InitStructure);
    }
    else
    {
        /* Configure the DMA Rx Channel with the buffer address and the buffer size */
        s_EEDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Address;
        s_EEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
        s_EEDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;
        DMA_Init(EE_I2C_DMA_STREAM_RX, &s_EEDMA_InitStructure);
    }
}

#endif /* IIC_DMA_ENABLE && 0 == USE_SIMULATE_IIC */

/************************************************
函数名称 ： AT24Cxx_Config
功    能 ： AT24Cxx配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
void AT24Cxx_Config(void)
{

#if USE_SIMULATE_IIC
	EE_IIC_Init();

#else
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    /* AT24C_I2Cx IO Periph clock enable */
    AT24C_IO_CLOCK_FUN(AT24C_SCL_CLK | AT24C_SDA_CLK, ENABLE);

    /* AT24C_I2Cx Periph clock enable */
    AT24C_I2C_CLOCK_FUN(AT24C_I2C_CLK, ENABLE);

    /* Configure AT24C_I2Cx pins: SCL, SDA */
    /* Connect PXx to I2C_SCL */
    GPIO_PinAFConfig(AT24C_SCL_PORT, AT24C_SCL_SOURCE, AT24C_I2C_GPIO_AF_MAP);

    /* Connect PXx to I2C_SDA */
    GPIO_PinAFConfig(AT24C_SDA_PORT, AT24C_SDA_SOURCE, AT24C_I2C_GPIO_AF_MAP);

    /* Confugure SCL and SDA pins as Alternate Function */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

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
    I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;

#ifndef I2C_10BITS_ADDRESS
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
#else
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
#endif /* I2C_10BITS_ADDRESS */

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
函数名称 ： AT24Cxx_Init
功    能 ： AT24Cxx初始化
参    数 ： 无
返 回 值 ： 无
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
        return;
	}
	
#endif /* USE_SIMULATE_IIC */
	
	EE_Test();
}

/************************************************
函数名称 ： EE_Test
功    能 ： EEPROM测试函数
参    数 ： 无
返 回 值 ： 无
*************************************************/
static uint8_t EE_Test(void)
{
	
#if _EE_TEST
	uint8_t I2c_Buf_Write[AT24C_MEMORY_CAPACITY] = {0};
	uint8_t I2c_Buf_Read[AT24C_MEMORY_CAPACITY] = {0};
	uint16_t i;

	/* 单字节读写测试 */
	AT24C_DEBUG_PRINTF("单字节读写测试\n");
	if(AT24Cxx_Write_Byte(0xBB, USER_TEST_ADDR + 0x200))
	{
		AT24Cxx_Busy_Wait();

#if USE_SIMULATE_IIC
		Delay(0xFFF);		// 模拟读写需要等待一段时间，否则会出错

#endif /* USE_SIMULATE_IIC */
		
		AT24C_DEBUG_PRINTF("data:0x%02X\n", AT24Cxx_Read_Byte(USER_TEST_ADDR + 0x200));
		AT24Cxx_Busy_Wait();
	}

#if 1
	/* 页读写测试 */
	AT24C_DEBUG_PRINTF("页读写测试\n");

	for (i = 0; i < AT24C_MEMORY_CAPACITY; i++) // 填充缓冲
	{
		I2c_Buf_Write[i] = i;
        if(i >= 256)
            I2c_Buf_Write[i] = I2c_Buf_Write[AT24C_MEMORY_CAPACITY - i];
	}
	
	I2c_Buf_Write[0] = 0xAA;

	// 将 I2c_Buf_Write中顺序递增的数据写入 EERPOM中
	AT24Cxx_Write_EEPROM(I2c_Buf_Write, USER_TEST_ADDR, AT24C_MEMORY_CAPACITY);
	
	// 写入函数已经包含了busy检查了
//	EE_DMA_RxWait();
//	AT24Cxx_Busy_Wait();

#if USE_SIMULATE_IIC
	Delay(0xFFF);			// 模拟读写需要等待一段时间，否则会出错

#endif /* USE_SIMULATE_IIC */
	
	// 将 EEPROM读出数据顺序保持到 I2c_Buf_Read中
	AT24Cxx_Read_EEPROM(I2c_Buf_Read, USER_TEST_ADDR, AT24C_MEMORY_CAPACITY);

	EE_DMA_RxWait();
	AT24Cxx_Busy_Wait();
   
	// 将 I2c_Buf_Read中的数据通过串口打印
	for (i = 0;i < AT24C_MEMORY_CAPACITY;i++)
	{
		if(I2c_Buf_Read[i] != I2c_Buf_Write[i])
		{
			AT24C_DEBUG_PRINTF("0x%02X , i = %d\n", I2c_Buf_Read[i], i);
			AT24C_DEBUG_PRINTF("错误:I2C EEPROM写入与读出的数据不一致\n");
			return 0;
		}
		printf("0x%02X ", I2c_Buf_Read[i]);
		if(i%11 == 10 || i == AT24C_MEMORY_CAPACITY - 1)
			printf("\n");    
	}
	AT24C_DEBUG_PRINTF("I2C(AT24C04)读写测试成功\n");

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
    if(DMA_GetITStatus(EE_I2C_DMA_STREAM_TX, EE_I2C_DMA_IT_FLAG_TX) != RESET)
    {
        /* Disable the DMA Tx Channel and Clear all its Flags */
        DMA_Cmd(EE_I2C_DMA_STREAM_TX, DISABLE);
        DMA_ClearITPendingBit(EE_I2C_DMA_STREAM_TX, EE_I2C_DMA_IT_FLAG_TX);

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
    if(DMA_GetITStatus(EE_I2C_DMA_STREAM_RX, EE_I2C_DMA_IT_FLAG_RX) != RESET)
    {
        /*!< Send STOP Condition */
        I2C_GenerateSTOP(AT24C_I2Cx, ENABLE);

        /* Disable the DMA Rx Channel and Clear all its Flags */
        DMA_Cmd(EE_I2C_DMA_STREAM_RX, DISABLE);
        DMA_ClearITPendingBit(EE_I2C_DMA_STREAM_RX, EE_I2C_DMA_IT_FLAG_RX);

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


