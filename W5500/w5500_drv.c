#include "w5500_drv.h"
#include <assert.h>
#include "w5500.h"
#include "w5500_mode.h"
#include "bsp_uart.h"

#include "FreeRTOS.h"
#include "task.h"


#define USING_HARDWARE_SPI  0

#define WAIT_TIME		2

#define W5500_STACK_SIZE          256                      // 任务堆栈大小
#define W5500_TASK_PRIORITY       5                        // 任务优先级

static W5500_Drv_Hooks w5500_drv;

static void prvW5500_App_Task(void *pvParameters);


/************************************************
函数名称 ： W5500_Delay_us
功    能 ： 软件毫秒延时
参    数 ： Count ---- 次数
返 回 值 ： 无
*************************************************/
static void W5500_Delay_us( uint32_t Count )
{
    while(Count)
    {
        Count--;
    }

    // vTaskDelay(Count);
}

/************************************************
函数名称 ： W5500_Delay_ms
功    能 ： 软件毫秒延时
参    数 ： Count ---- 次数
返 回 值 ： 无
*************************************************/

#if 1
void W5500_Delay_ms( uint32_t Count )
{
    uint16_t i;

    while(Count--)
    {
        for(i = 1240; i > 0; i--);        // 根据震荡时间为 i取值
    }

    // vTaskDelay(Count / portTICK_RATE_MS);
}

#endif

#if USING_HARDWARE_SPI
static uint8_t TimeOut_Callback(char ErrorCode)
{
	/* 等待超时后的处理,输出错误信息 */
	DEBUG_PRINTF("SPI waiting overtime! EerrorCode = %d\n", ErrorCode);

	return 0;
}
#endif

/************************************************
函数名称 ： W5500_Data_RW
功    能 ： W5500数据读写
参    数 ： Data ---- 数据
返 回 值 ： Data ---- 数据
*************************************************/
static uint8_t W5500_Data_RW( uint8_t Data )
{
    uint8_t i;

#if USING_HARDWARE_SPI
	__IO uint32_t W25Q_TimeOut = 0x1000;

	/* Wait for W25Q_SPIx Tx buffer empty */
	while (SPI_I2S_GetFlagStatus(SPI4, SPI_I2S_FLAG_TXE) == RESET)
	{
		if (0 == (W25Q_TimeOut--))
			return TimeOut_Callback(0);
	}

	/* Send byte through the W25Q_SPIx peripheral */
	SPI_I2S_SendData(SPI4, Data);

	W25Q_TimeOut = 0x1000;

	/* Wait for W25Q_SPIx data reception */
	while (SPI_I2S_GetFlagStatus(SPI4, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if (0 == (W25Q_TimeOut--))
			return TimeOut_Callback(1);
	}

	/* Return the byte read from the W25Q_SPIx bus */
	Data = SPI_I2S_ReceiveData(SPI4);

#else

#if 1   /* SPI Mode == 0 */
    for(i = 0;i < 8;i++)
    {
        W5500_SCLK(LOW);
        W5500_Delay_us(WAIT_TIME);
		if(Data & 0x80)
		{
			W5500_MOSI(HIGH);
		}
		else
		{
			W5500_MOSI(LOW);
		}
		Data <<= 1;
        W5500_Delay_us(WAIT_TIME);
        W5500_SCLK(HIGH);
        W5500_Delay_us(WAIT_TIME);
        Data |= W5500_MISO;
    }

    W5500_SCLK(LOW);
#else   /* SPI Mode == 3 */
    W5500_SCLK(HIGH);

    for(i = 0;i < 8;i++)
    {
        W5500_SCLK(LOW);
        W5500_Delay_us(WAIT_TIME);
		if(Data & 0x80)
		{
			W5500_MOSI(HIGH);
		}
		else
		{
			W5500_MOSI(LOW);
		}
		Data <<= 1;
        W5500_Delay_us(WAIT_TIME);
        W5500_SCLK(HIGH);
        W5500_Delay_us(WAIT_TIME);
        Data |= W5500_MISO;
    }
#endif

#endif /* USING_HARDWARE_SPI */

    return Data;
}

/************************************************
函数名称 ： W5500_Write_Byte
功    能 ： W5500写一个字节
参    数 ： Address ---- 地址
            Control ---- 控制
			Value ---- 输入值
返 回 值 ： 无
*************************************************/
void W5500_Write_Byte( uint16_t Address, uint8_t Control, uint8_t Value )
{
    W5500_SCS(LOW);
	W5500_Data_RW((Address & 0xFF00) >> 8);
	W5500_Data_RW(Address & 0x00FF);
	W5500_Data_RW(Control | 0x04);
	W5500_Data_RW(Value);
    W5500_SCS(HIGH);
}

/************************************************
函数名称 ： W5500_Write_Buf
功    能 ： W5500写 N个字节
参    数 ： Address ---- 地址
            Control ---- 控制
			pBuf ---- 数据
            Len ---- 长度
返 回 值 ： 无
*************************************************/
void W5500_Write_Buf( uint16_t Address, uint8_t Control, const uint8_t *pBuf, uint16_t Len )
{
    assert(Len > 0);

    W5500_SCS(LOW);
	W5500_Data_RW((Address & 0xFF00) >> 8);
	W5500_Data_RW(Address & 0x00FF);
	W5500_Data_RW(Control | 0x04);
    while(Len--)
    {
        W5500_Data_RW(*pBuf++);
    }
    W5500_SCS(HIGH);
}

/************************************************
函数名称 ： W5500_Read_Byte
功    能 ： W5500读一个字节
参    数 ： Address ---- 地址
            Control ---- 控制
返 回 值 ： data ---- 数据
*************************************************/
uint8_t W5500_Read_Byte( uint16_t Address, uint8_t Control )
{
    uint8_t data = 0;

    W5500_SCS(LOW);
	W5500_Data_RW((Address & 0xFF00) >> 8);
	W5500_Data_RW(Address & 0x00FF);
	W5500_Data_RW(Control & ~0x04);
	data = W5500_Data_RW(0xFF);
    W5500_SCS(HIGH);

    return data;
}

/************************************************
函数名称 ： W5500_Read_Buf
功    能 ： W5500读 N个字节
参    数 ： Address ---- 地址
            Control ---- 控制
			pBuf ---- 数据
            Len ---- 长度
返 回 值 ： 无
*************************************************/
void W5500_Read_Buf( uint16_t Address, uint8_t Control, uint8_t *pBuf, uint16_t Len )
{
    assert(Len > 0);

    W5500_SCS(LOW);
	W5500_Data_RW((Address & 0xFF00) >> 8);
	W5500_Data_RW(Address & 0x00FF);
	W5500_Data_RW(Control & ~0x04);
    while(Len--)
    {
        *pBuf = W5500_Data_RW(0xFF);
        pBuf++;
    }
    W5500_SCS(HIGH);
}

/************************************************
函数名称 ： W5500_Reset
功    能 ： W5500重置
参    数 ： 无
返 回 值 ： 无
*************************************************/
static void W5500_Reset(void)
{
    W5500_RST(LOW);
    W5500_Delay_us(800);        // 复位时钟周期至少 500us
    W5500_RST(HIGH);
    W5500_Delay_ms(1600);
    while(0 == Get_ETH_Link());    //等待以太网连接完成
}

/************************************************
函数名称 ： W5500_Config
功    能 ： W5500配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
static void W5500_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    /* W5500_SCS */
	W5500_SCS_Clock_FUN(W5500_SCS_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = W5500_SCS_PINS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(W5500_SCS_PORT, &GPIO_InitStructure);

    W5500_SCS(HIGH);

    /* W5500_INT */
	W5500_INT_Clock_FUN(W5500_INT_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = W5500_INT_PINS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(W5500_INT_PORT, &GPIO_InitStructure);

    /* W5500_RST */
	W5500_RST_Clock_FUN(W5500_RST_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = W5500_RST_PINS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(W5500_RST_PORT, &GPIO_InitStructure);

#if (0 == USING_HARDWARE_SPI)
    /* W5500_SCLK */
	GPIO_InitStructure.GPIO_Pin = W5500_SCLK_PINS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(W5500_SCLK_PORT, &GPIO_InitStructure);

    /* W5500_MISO */
	GPIO_InitStructure.GPIO_Pin = W5500_MISO_PINS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(W5500_MISO_PORT, &GPIO_InitStructure);

    /* W5500_MOSI */
	GPIO_InitStructure.GPIO_Pin = W5500_MOSI_PINS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(W5500_MOSI_PORT, &GPIO_InitStructure);
#else
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4, ENABLE);
	GPIO_PinAFConfig(W5500_SCLK_PORT, W5500_SCLK_SOURCE, GPIO_AF_SPI4);
	GPIO_PinAFConfig(W5500_MISO_PORT, W5500_MISO_SOURCE, GPIO_AF_SPI4);
	GPIO_PinAFConfig(W5500_MOSI_PORT, W5500_MOSI_SOURCE, GPIO_AF_SPI4);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

    /* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = W5500_SCLK_PINS;
	GPIO_Init(W5500_SCLK_PORT, &GPIO_InitStructure);

	/* SPI MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin = W5500_MISO_PINS;
	GPIO_Init(W5500_MISO_PORT, &GPIO_InitStructure);

    /* SPI MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin = W5500_MOSI_PINS;
	GPIO_Init(W5500_MOSI_PORT, &GPIO_InitStructure);

	/* W25Q_SPIx configuration */
    SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI4, &SPI_InitStructure);

	/* Disable W25Q_SPIx CRC calculation */
	SPI_CalculateCRC(SPI4, DISABLE);

	/* Enable W25Q_SPIx */
	SPI_Cmd(SPI4, ENABLE);
#endif
}

/************************************************
函数名称 ： W5500_Init
功    能 ： W5500初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void W5500_Init(void)
{
    uint8_t txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};   // tx buffer set K bits
    uint8_t rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};   // rx buffet set K bits

    w5500_drv.write_byte_fn = W5500_Write_Byte;
    w5500_drv.write_buf_fn = W5500_Write_Buf;
    w5500_drv.read_byte_fn = W5500_Read_Byte;
    w5500_drv.read_buf_fn = W5500_Read_Buf;
    W5500_InitHooks(&w5500_drv);

    W5500_GPIO_Config();
    W5500_Reset();                                      // 硬复位 W5500

    W5500_Net_Config();

    Socket_Buf_Init(txsize, rxsize);                    // 设置发送缓冲区和接收缓冲区的大小
    Set_RTR(0x07d0);                                    // 设置重试时间，每一单位数值为100微秒，默认为2000(200ms)
    Set_RCR(3);                                         // 设置重试次数，默认为8次，如果重发的次数超过设定值,则产生超时中断(相关的端口中断寄存器中的Sn_IR 超时位(TIMEOUT)置“1”)

    BaseType_t xReturn;

    xReturn = xTaskCreate(prvW5500_App_Task, "prvW5500_App_Task", W5500_STACK_SIZE, NULL, W5500_TASK_PRIORITY, NULL);
    if (pdPASS != xReturn)
    {
        return; //创建任务失败
    }
}

/************************************************
函数名称 ： prvCANSend_Task
功    能 ： CAN发送应用任务程序
参    数 ： pvParameters
返 回 值 ： 无
*************************************************/
static void prvW5500_App_Task(void *pvParameters)
{
    while(1)
    {
		W5500_TCP_Client();
		// W5500_TCP_Server();
        // W5500_UDP_Deal();
        vTaskDelay(300 / portTICK_RATE_MS);
        // W5500_Delay_ms(1600);
    }
}


/*---------------------------- END OF FILE ----------------------------*/


