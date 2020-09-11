#ifndef __NRF24L01_H
#define __NRF24L01_H


#include "reg52.h"

#ifndef ENABLE
#define ENABLE              1
#endif /* ENABLE */

#ifndef DISABLE
#define DISABLE             0
#endif /* DISABLE */

#ifndef HIGH
#define HIGH                1
#endif /* HIGH */

#ifndef LOW
#define LOW                 0
#endif /* LOW */

/* ----- NRF24L01�Ĵ����������� ----- */
#define READ_REG        0x00  //�����üĴ���,��5λΪ�Ĵ�����ַ
#define WRITE_REG       0x20  //д���üĴ���,��5λΪ�Ĵ�����ַ
#define RD_RX_PLOAD     0x61  //��RX��Ч����,1~32�ֽ�
#define WR_TX_PLOAD     0xA0  //дTX��Ч����,1~32�ֽ�
#define FLUSH_TX        0xE1  //���TX FIFO�Ĵ���.����ģʽ����
#define FLUSH_RX        0xE2  //���RX FIFO�Ĵ���.����ģʽ����
#define REUSE_TX_PL     0xE3  //����ʹ����һ������,CEΪ��,���ݰ������Ϸ���.
#define NOP             0xFF  //�ղ���,����������״̬�Ĵ���	 
/* ----- END ----- */

/* ----- NRF24L01�Ĵ�����ַ ----- */
#define CONFIG          0x00  // ���üĴ�����ַ                             
#define EN_AA           0x01  // ʹ���Զ�Ӧ���� 
#define EN_RXADDR       0x02  // ���յ�ַ����
#define SETUP_AW        0x03  // ���õ�ַ���(��������ͨ��)
#define SETUP_RETR      0x04  // �����Զ��ط�
#define RF_CH           0x05  // RFͨ��
#define RF_SETUP        0x06  // RF�Ĵ���	
#define STATUS          0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ��Ĵ���
#define CD              0x09  // �ز����Ĵ���
#define RX_ADDR_P0      0x0A  // ����ͨ��0���յ�ַ
#define RX_ADDR_P1      0x0B  // ����ͨ��1���յ�ַ
#define RX_ADDR_P2      0x0C  // ����ͨ��2���յ�ַ
#define RX_ADDR_P3      0x0D  // ����ͨ��3���յ�ַ
#define RX_ADDR_P4      0x0E  // ����ͨ��4���յ�ַ
#define RX_ADDR_P5      0x0F  // ����ͨ��5���յ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ��������ͨ��0��Ч���ݿ��(1~32�ֽ�) 
#define RX_PW_P1        0x12  // ��������ͨ��1��Ч���ݿ��(1~32�ֽ�) 
#define RX_PW_P2        0x13  // ��������ͨ��2��Ч���ݿ��(1~32�ֽ�) 
#define RX_PW_P3        0x14  // ��������ͨ��3��Ч���ݿ��(1~32�ֽ�) 
#define RX_PW_P4        0x15  // ��������ͨ��4��Ч���ݿ��(1~32�ֽ�)
#define RX_PW_P5        0x16  // ��������ͨ��5��Ч���ݿ��(1~32�ֽ�)
#define FIFO_STATUS     0x17  // FIFO״̬�Ĵ���
/* ----- END ----- */

/* ----- STATUS�Ĵ��� bitλ���� ----- */
#define TX_MAX  	0x10  	  // �ﵽ����ʹ����ж�
#define TX_OK   	0x20  	  // TX��������ж�
#define RX_OK   	0x40  	  // ���յ������ж�
/* ----- END ----- */

/* ----- 24L01���ͽ������ݿ�ȶ��� ----- */
#define NRF_ADDR_WIDTH  5     // 5�ֽڵ�ַ���
#define TX_PLOAD_WIDTH  32    // 32�ֽ���Ч���ݿ�ȣ�������Է�������Ч���ݿ��һ�£�����Է��ղ�����
#define RX_PLOAD_WIDTH  32    // 32�ֽ���Ч���ݿ��
/* ----- END ----- */

#define NRF_CE(x)     P1_0 = x
#define NRF_CSN(x)    P3_7 = x
#define NRF_MISO   	  P2_0
#define NRF_MOSI(x)   P2_2 = x
#define NRF_SCK(x)    P3_6 = x
#define NRF_IRQ       P3_3

void NRF24L_Write_Buf( const uint8_t *pBuf, uint8_t Len );
void NRF24L_Read_Buf( uint8_t *pBuf, uint8_t Len );
uint8_t NRF24L_SendPacket( uint8_t *pData, uint8_t Const );
bit NRF24L_ReceivePacket( uint8_t *pData, uint8_t Const );
void NRF24L_Config(void);
void NRF24L_Init(void);


#endif /* __NRF24L01_H */


/*------------------------------- END OF FILE -------------------------------*/


