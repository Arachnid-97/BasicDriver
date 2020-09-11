#include "w5500_drv.h"
#include <string.h>
#include "w5500.h"
#include "w5500_mode.h"
#include "bsp_uart.h"
//#define NDEBUG              //ȡ������
#include <assert.h>         // ���Կ�


#define _USART_DEBUG

LocalNet_TypeDef    Local_Net;
RemoteNet_TypeDef   Socket_0;           // �����ʹ�õ� Socket�˿ڶ���ṹ��

uint16_t SSIZE[MAX_SOCK_NUM] = {0,0,0,0,0,0,0,0};     // Max Tx buffer
uint16_t RSIZE[MAX_SOCK_NUM] = {0,0,0,0,0,0,0,0};     // Max Rx buffer

/************************************************
�������� �� W5500_Delay_us
��    �� �� ���������ʱ
��    �� �� Count ---- ����
�� �� ֵ �� ��
*************************************************/
static void W5500_Delay_us( uint32_t Count )
{
    while(Count)
    {
        Count--;
    }
}

/************************************************
�������� �� W5500_Delay_ms
��    �� �� ���������ʱ
��    �� �� Count ---- ����
�� �� ֵ �� ��
*************************************************/

#if 1
void W5500_Delay_ms( uint32_t Count )
{
    uint16_t i;

    while(Count--)
    {
        for(i = 1240;i > 0;i--);        // ������ʱ��Ϊ iȡֵ
    }
}

#endif

/************************************************
�������� �� W5500_Data_RW
��    �� �� W5500���ݶ�д
��    �� �� Data ---- ����
�� �� ֵ �� Data ---- ����
*************************************************/
static uint8_t W5500_Data_RW( uint8_t Data )
{
    uint8_t i;

    W5500_SCLK(LOW);

    for(i = 0;i < 8;i++)
    {
		if(Data & 0x80)
		{
			W5500_MOSI(HIGH);
		}
		else
		{
			W5500_MOSI(LOW);
		}
		Data <<= 1;
        W5500_SCLK(HIGH);
        Data |= W5500_MISO;
        W5500_SCLK(LOW);
    }

    return Data;
}

/************************************************
�������� �� W5500_Write_Byte
��    �� �� W5500дһ���ֽ�
��    �� �� Address ---- ��ַ
            Control ---- ����
			Value ---- ����ֵ
�� �� ֵ �� ��
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
�������� �� W5500_Write_Buf
��    �� �� W5500д N���ֽ�
��    �� �� Address ---- ��ַ
            Control ---- ����
			pBuf ---- ����
            Len ---- ����
�� �� ֵ �� ��
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
�������� �� W5500_Read_Byte
��    �� �� W5500��һ���ֽ�
��    �� �� Address ---- ��ַ
            Control ---- ����
�� �� ֵ �� data ---- ����
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
�������� �� W5500_Read_Buf
��    �� �� W5500�� N���ֽ�
��    �� �� Address ---- ��ַ
            Control ---- ����
			pBuf ---- ����
            Len ---- ����
�� �� ֵ �� ��
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
�������� �� W5500_Reset
��    �� �� W5500����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
static void W5500_Reset(void)
{
    W5500_RST(LOW);
    W5500_Delay_us(800);        // ��λʱ���������� 500us
    W5500_RST(HIGH);
    W5500_Delay_us(100);
    while(0 == (Get_PHYCFGR() & LINK));    //�ȴ���̫���������
}

/************************************************
�������� �� Detect_Gateway
��    �� �� ������ط�����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/

#if 0
static void Detect_Gateway(void)
{

}

#endif

/************************************************
�������� �� Load_Net_Parameters
��    �� �� װ���������
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
static void Load_Net_Parameters(void)
{
    /* ���屾�ض� IP��Ϣ */
    uint8_t local_ip[4] = {192,168,1,200};				    /* ���� W5500Ĭ��IP��ַ */
    uint8_t subnet[4] = {255,255,255,0};				    /* ���� W5500Ĭ���������� */
    uint8_t gateway[4] = {192,168,1,1};					    /* ���� W5500Ĭ������ */
    uint8_t dns_server[4] = {114,114,114,114};			    /* ���� W5500Ĭ�� DNS */
    uint8_t local_mac[6] = {0x00,0x08,0xdc,0x11,0x11,0x11}; /* ���� W5500Ĭ�� MAC��ַ */
    uint16_t local_port = 5000;	                       	    /* ���屾�ض˿� */

    /* ����Զ�̶� IP��Ϣ */
    uint8_t remote_ip[4] = {192,168,1,100};				    /* Զ�� IP��ַ */
    uint8_t remote_mac[6] = {0x00,0x06,0x5D,0x18,0x02,0x11};/* Զ�� MAC��ַ */
    uint16_t remote_port = 8000;						    /* Զ�̶˿ں� */

    /* ������Ϣ */
    memcpy(Local_Net.LIp, local_ip, 4);
    memcpy(Local_Net.Sub, subnet, 4);
    memcpy(Local_Net.Gw, gateway, 4);
    memcpy(Local_Net.Dns, dns_server, 4);
    memcpy(Local_Net.LMac, local_mac, 6);
    Local_Net.LPort = local_port;

    memcpy(Socket_0.RIp, remote_ip, 4);
    memcpy(Socket_0.RMac, remote_mac, 6);
    Socket_0.RPort = remote_port;
}

/************************************************
�������� �� W5500_Config
��    �� �� W5500����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
static void W5500_Config(void)
{
    /* W5500_SCS */
    PD_DDR_DDR1 = 1;
    PD_CR1_C11 = 1;
    PD_CR2_C21 = 0;


    /* W5500_INT */
    PC_DDR_DDR5 = 0;
    PC_CR1_C15 = 1;
    PC_CR2_C25 = 0;


    /* W5500_RST */
    PC_DDR_DDR6 = 1;
    PC_CR1_C16 = 1;
    PC_CR2_C26 = 0;


    /* W5500_SCLK */
    PC_DDR_DDR7 = 1;
    PC_CR1_C17 = 1;
    PC_CR2_C27 = 1;


    /* W5500_MISO */
    PD_DDR_DDR3 = 0;
    PD_CR1_C13 = 1;
    PD_CR2_C23 = 0;


    /* W5500_MOSI */
    PD_DDR_DDR2 = 1;
    PD_CR1_C12 = 1;
    PD_CR2_C22 = 1;

}

/************************************************
�������� �� W5500_Init
��    �� �� W5500��ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void W5500_Init(void)
{
    uint8_t temp[4] = {0};
    uint8_t txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};   // tx buffer set K bits
    uint8_t rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};   // rx buffet set K bits


    W5500_Config();
    W5500_Reset();                                      // Ӳ��λ W5500
    Load_Net_Parameters();

    Set_GAR(Local_Net.Gw);                              // ��������(Gateway)��IP��ַ
    Set_SUBR(Local_Net.Sub);                            // ������������(MASK)ֵ
    Set_SHAR(Local_Net.LMac);                           // ����Դ MAC��ַ
    Set_SIPR(Local_Net.LIp);                            // ����Դ IP��ַ

#ifdef _USART_DEBUG
    Get_SIPR (temp);
    UART1_SendString(temp, 4);
//    printf("IP : %d.%d.%d.%d\r\n", temp[0],temp[1],temp[2],temp[3]);

    Get_SUBR(temp);
    UART1_SendString(temp, 4);
//    printf("MASK : %d.%d.%d.%d\r\n", temp[0],temp[1],temp[2],temp[3]);

    Get_GAR(temp);
    UART1_SendString(temp, 4);
//    printf("Gateway : %d.%d.%d.%d\r\n", temp[0],temp[1],temp[2],temp[3]);

#endif /* _USART_DEBUG */

    Socket_Buf_Init(txsize, rxsize);                    // ���÷��ͻ������ͽ��ջ������Ĵ�С
    Set_RTR(0x07d0);                                    // ��������ʱ�䣬ÿһ��λ��ֵΪ100΢�룬Ĭ��Ϊ2000(200ms)
    Set_RCR(8);                                         // �������Դ�����Ĭ��Ϊ8�Σ�����ط��Ĵ��������趨ֵ,�������ʱ�ж�(��صĶ˿��жϼĴ����е�Sn_IR ��ʱλ(TIMEOUT)�á�1��)

//    if(Socket_Config(SOCK_TCP_C_PORT, TCP_SERVER, Local_Net.LPort) == TCP_CLIENT );
//    {
//
//    }
}


/*---------------------------- END OF FILE ----------------------------*/


