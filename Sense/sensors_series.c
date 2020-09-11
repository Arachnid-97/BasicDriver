#include "sensors_series.h"
#include "bsp_time.h"
#include "bsp_uart.h"


DHT11_Data_TypeDef DHT11_Data;

bit g_DHT11_Read_flag = 0;			// 数据读取标志

/************************************************
函数名称 ： Read_Byte
功    能 ： 读数据
参    数 ： 无
返 回 值 ： temp ---- 数据
*************************************************/
static uint8_t Read_Byte(void)
{
	uint8_t i, temp=0;

	for(i=0;i<8;i++)    
	{	 
		/*每bit以50us低电平标置开始，轮询直到从机发出 的50us 低电平 结束*/  
		while(0 == DHT11_PIN);

		/*DHT11 以26~28us的高电平表示“0”，以70us高电平表示“1”，
		 *通过检测 x us后的电平即可区别这两个状 ，x 即下面的延时 
		 */
		Delay_30us(1); //延时x us这个延时需要大于数据 0持续的时间即可	   	  

		if(DHT11_PIN)/* x us后仍为高电平表示数据“1” */
		{
			/* 等待数据1的高电平结束 */
			while(DHT11_PIN);

			temp|=(uint8_t)(0x01<<(7-i));  //把第7-i位置1，MSB先行 
		}
		else	 // x us后为低电平表示数据“0”
		{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //把第7-i位置0，MSB先行
		}
	}
	return temp;
}

/************************************************
函数名称 ： Read_DHT11
功    能 ： 温湿度读取
参    数 ： 无
返 回 值 ： 0 / 1
*************************************************/
uint8_t Read_DHT11( DHT11_Data_TypeDef *DHT11_Data )
{
	P03_Quasi_Mode;
	
	/* 输出模式 */
//	P03_PushPull_Mode;
	/* 主机拉低 */
	clr_P03;
	/* 延时至少 18ms */
	Delay_30us(610);

	/* 总线拉高 主机延时 30us左右 */
	set_P03; 
	/* 延时 30us */
	Delay_30us(1);

	/* 主机设为输入 判断从机响应信号 */ 
//	P03_Input_Mode;

	/* 判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行 */   
	if(0 == DHT11_PIN)
	{
		/* 轮询直到从机发出的 80us低电平响应信号结束 */  
		while(0 == DHT11_PIN);

		/* 轮询直到从机发出的 80us高电平标置信号结束 */
		while(1 == DHT11_PIN);

		/* 开始接收数据 */   
		DHT11_Data->humi_int = Read_Byte();

		DHT11_Data->humi_deci = Read_Byte();

		DHT11_Data->temp_int = Read_Byte();

		DHT11_Data->temp_deci = Read_Byte();

		DHT11_Data->check_sum = Read_Byte();

		/* 读取结束，引脚改为输出模式 */
//		P03_PushPull_Mode;
		/* 主机拉高 */
		set_P03;

		/* 检查读取的数据是否正确 */
		if(DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int+ DHT11_Data->temp_deci)
			return 1;
		else 
			return 0;
	}
	else
	{		
		return 0;
	}   
}

/*---------------------------- END OF FILE ----------------------------*/


