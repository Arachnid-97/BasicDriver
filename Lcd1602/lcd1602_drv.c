#include "lcd1602_drv.h"


/* 不使用 LCD置 0，可以绕过驱动 LCD，而不必改动原程序 */
#define ENBALE_LCD1602		0

#define WAIT_TIME			0

/************************************************
函数名称 ： LCD_Delay_us
功    能 ： 软件毫秒延时
参    数 ： Count ---- 次数
返 回 值 ： 无
*************************************************/
static void LCD_Delay_us( uint32_t Count )
{
	while(Count)
	{
		Count--;
	}
}

/************************************************
函数名称 ： Read_Busy
功    能 ： Lcd1602读写忙检测
参    数 ： 无
返 回 值 ： 无
*************************************************/
static void Read_Busy(void)         
{
    LCD1602_DB(0xFF);		// 准双向口在读外部状态前，要先锁存为‘1’
	LCD_Delay_us(WAIT_TIME);
    LCD1602_RS(LOW);
    LCD1602_RW(HIGH);
	LCD_Delay_us(WAIT_TIME);
	LCD1602_EN(HIGH);
	while(LCD1602_D7_READ)
		;					// 等待读写操作允许
	LCD1602_EN(LOW);    	// 释放总线
}

/************************************************
函数名称 ： Lcd1602_Write_Cmd
功    能 ： Lcd1602写命令
参    数 ： Cmd ---- 命令
返 回 值 ： 无
*************************************************/
void Lcd1602_Write_Cmd( uint8_t Cmd )    
{
	
#if ENBALE_LCD1602
    Read_Busy();
    LCD1602_RS(LOW);
    LCD1602_RW(LOW);
    LCD1602_DB(Cmd);
	LCD_Delay_us(WAIT_TIME);
    LCD1602_EN(HIGH);
    LCD1602_EN(LOW);
	
#endif /* ENBALE_LCD1602 */
}

/************************************************
函数名称 ： Lcd1602_Write_Data
功    能 ： Lcd1602写数据
参    数 ： Data ---- 数据
返 回 值 ： 无
*************************************************/
static void Lcd1602_Write_Data( uint8_t Data ) 
{
	Read_Busy();
	LCD1602_RS(HIGH);
	LCD1602_RW(LOW);
	LCD1602_DB(Data);
	LCD_Delay_us(WAIT_TIME);
	LCD1602_EN(HIGH);
	LCD1602_EN(LOW);
}

/************************************************
函数名称 ： Lcd_Coord
功    能 ： Lcd1602坐标显示
参    数 ： X ---- X轴
			Y ---- Y轴
返 回 值 ： 无
*************************************************/
static void Lcd_Coord( uint8_t X, uint8_t Y ) 
{
    uint8_t addr = 0;
    if(!Y)
        addr = 0x00 + X;
    else
        addr = 0x40 + X;
    
    Lcd1602_Write_Cmd(addr | 0x80);
}

/************************************************
函数名称 ： Lcd_Show_Str
功    能 ： Lcd1602显示
参    数 ： Str ---- 数据
			X ---- X轴
			Y ---- Y轴
			Len --- 长度
返 回 值 ： 无
*************************************************/
void Lcd_Show_Str( const uint8_t *Str, uint8_t X, uint8_t Y, uint8_t Len )
{

#if ENBALE_LCD1602
    Lcd_Coord(X,Y);							//当前字符的坐标
	
    while(Len--)
    {
        Lcd1602_Write_Data(*Str++);
    }
	
#endif /* ENBALE_LCD1602 */
}

/************************************************
函数名称 ： Lcd_Printf
功    能 ： Lcd1602字符串显示
参    数 ： Str ---- 数据
			X ---- X轴
			Y ---- Y轴
返 回 值 ： 无
*************************************************/
void Lcd_Printf( const uint8_t *Str, uint8_t X, uint8_t Y )
{
	
#if ENBALE_LCD1602
    Lcd_Coord(X,Y);							//当前字符的坐标
    while(*Str != '\0')
    {
        Lcd1602_Write_Data(*Str++);
    }
	
#endif /* ENBALE_LCD1602 */
}

/************************************************
函数名称 ： Lcd1602_Config
功    能 ： Lcd1602配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
void Lcd1602_Config(void)
{
	P1_0 = 0;						// 关闭数码管
	P1_1 = 0;
	P1_2 = 0;
	P1_3 = 0;
}

/************************************************
函数名称 ： Lcd1602_Init
功    能 ： Lcd1602初始化
参    数 ： D ---- 显示（开 or 关）
			C ---- 光标显示
			B ---- 光标闪烁
			N ---- 地址移动
			S ---- 屏幕移动
返 回 值 ： 无
*************************************************/
void Lcd1602_Init( uint8_t D, uint8_t C, uint8_t B, uint8_t N, uint8_t S )
{
	uint8_t disp = 0x08;
	uint8_t offset = 0x04;
	
	Lcd1602_Config();
	
	disp |= ((D << 2) | (C << 1) | (B << 0));
	offset |= ((N << 1) | (S << 0));
	
    Lcd1602_Write_Cmd(0x38);    	//打开，5*8,8位数据
    Lcd1602_Write_Cmd(disp);
    Lcd1602_Write_Cmd(offset);
    Lcd1602_Write_Cmd(LCD_CLEAR);   //清屏   	
}


/*---------------------------- END OF FILE ----------------------------*/


