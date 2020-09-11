#ifndef __NRF24L01_H
#define __NRF24L01_H


#include "stm32f10x.h"

//#ifndef ENABLE
//#define ENABLE              1
//#endif /* ENABLE */

//#ifndef DISABLE
//#define DISABLE             0
//#endif /* DISABLE */

#ifndef HIGH
#define HIGH                1
#endif /* HIGH */

#ifndef LOW
#define LOW                 0
#endif /* LOW */

/* ----- NRF24L01�Ĵ����������� ----- */
#define NRF_READ_REG        0x00  //�����üĴ���,��5λΪ�Ĵ�����ַ
#define NRF_WRITE_REG       0x20  //д���üĴ���,��5λΪ�Ĵ�����ַ
#define NRF_RD_RX_PLOAD     0x61  //��RX��Ч����,1~32�ֽ�
#define NRF_WR_TX_PLOAD     0xA0  //дTX��Ч����,1~32�ֽ�
#define NRF_FLUSH_TX        0xE1  //���TX FIFO�Ĵ���.����ģʽ����
#define NRF_FLUSH_RX        0xE2  //���RX FIFO�Ĵ���.����ģʽ����
#define NRF_REUSE_TX_PL     0xE3  //����ʹ����һ������,CEΪ��,���ݰ������Ϸ���.
#define NRF_NOP             0xFF  //�ղ���,����������״̬�Ĵ���
/* ��̬ */
#define NRF_R_RX_PL_WID			0x60
#define NRF_W_ACK_PLOAD			0xA8
#define NRF_WR_TX_PLOAD_NACK	0xB0

/* ----- END ----- */

/* ----- NRF24L01�Ĵ�����ַ ----- */
#define NRF_CONFIG          0x00  // ���üĴ�����ַ                             
#define NRF_EN_AA           0x01  // ʹ���Զ�Ӧ���� 
#define NRF_EN_RXADDR       0x02  // ���յ�ַ����
#define NRF_SETUP_AW        0x03  // ���õ�ַ���(��������ͨ��)
#define NRF_SETUP_RETR      0x04  // �����Զ��ط�
#define NRF_RF_CH           0x05  // RFͨ��
#define NRF_RF_SETUP        0x06  // RF�Ĵ���	
#define NRF_STATUS          0x07  // ״̬�Ĵ���
#define NRF_OBSERVE_TX      0x08  // ���ͼ��Ĵ���
#define NRF_CD              0x09  // �ز����Ĵ���
#define NRF_RX_ADDR_P0      0x0A  // ����ͨ��0���յ�ַ
#define NRF_RX_ADDR_P1      0x0B  // ����ͨ��1���յ�ַ
#define NRF_RX_ADDR_P2      0x0C  // ����ͨ��2���յ�ַ
#define NRF_RX_ADDR_P3      0x0D  // ����ͨ��3���յ�ַ
#define NRF_RX_ADDR_P4      0x0E  // ����ͨ��4���յ�ַ
#define NRF_RX_ADDR_P5      0x0F  // ����ͨ��5���յ�ַ
#define NRF_TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define NRF_RX_PW_P0        0x11  // ��������ͨ��0��Ч���ݿ��(1~32�ֽ�) 
#define NRF_RX_PW_P1        0x12  // ��������ͨ��1��Ч���ݿ��(1~32�ֽ�) 
#define NRF_RX_PW_P2        0x13  // ��������ͨ��2��Ч���ݿ��(1~32�ֽ�) 
#define NRF_RX_PW_P3        0x14  // ��������ͨ��3��Ч���ݿ��(1~32�ֽ�) 
#define NRF_RX_PW_P4        0x15  // ��������ͨ��4��Ч���ݿ��(1~32�ֽ�)
#define NRF_RX_PW_P5        0x16  // ��������ͨ��5��Ч���ݿ��(1~32�ֽ�)
#define NRF_FIFO_STATUS     0x17  // FIFO״̬�Ĵ���
/* ��̬ */
#define NRF_DYNPD			0x1C  // ���ö�̬��Ч�غɳ���
#define NRF_FEATRUE			0x1D  // 
/* ----- END ----- */

/* ----- STATUS�Ĵ��� bitλ���� ----- */
#define NRF_TX_MAX  	0x10  	  // �ﵽ����ʹ����ж�
#define NRF_TX_OK   	0x20  	  // TX��������ж�
#define NRF_RX_OK   	0x40  	  // ���յ������ж�
/* ----- END ----- */

/* ----- 24L01���ͽ������ݿ�ȶ��� ----- */
#define NRF_ADDR_WIDTH  	5     // 5�ֽڵ�ַ���
#define NRF_TX_PLOAD_WIDTH  32    // 32�ֽ���Ч���ݿ�ȣ�������Է�������Ч���ݿ��һ�£�����Է��ղ�����
#define NRF_RX_PLOAD_WIDTH  32    // 32�ֽ���Ч���ݿ��
/* ----- END ----- */

#define NRF_CSN_GPIO_PORT			GPIOC
#define NRF_CSN_GPIO_PIN			GPIO_Pin_3

#define NRF_SCK_GPIO_PORT			GPIOC
#define NRF_SCK_GPIO_PIN			GPIO_Pin_6

#define NRF_MISO_GPIO_PORT			GPIOB
#define NRF_MISO_GPIO_PIN			GPIO_Pin_15

#define NRF_MOSI_GPIO_PORT			GPIOC
#define NRF_MOSI_GPIO_PIN			GPIO_Pin_7

#define NRF_CE_GPIO_PORT			GPIOC
#define NRF_CE_GPIO_PIN				GPIO_Pin_2

#define NRF_IRQ_GPIO_PORT			GPIOC
#define NRF_IRQ_GPIO_PIN			GPIO_Pin_8

#define NRF_CE(x)     GPIO_WriteBit(NRF_CE_GPIO_PORT,NRF_CE_GPIO_PIN,(BitAction)x)
#define NRF_CSN(x)    GPIO_WriteBit(NRF_CSN_GPIO_PORT,NRF_CSN_GPIO_PIN,(BitAction)x)
#define NRF_MISO   	  GPIO_ReadInputDataBit(NRF_MISO_GPIO_PORT,NRF_MISO_GPIO_PIN)
#define NRF_MOSI(x)   GPIO_WriteBit(NRF_MOSI_GPIO_PORT,NRF_MOSI_GPIO_PIN,(BitAction)x)
#define NRF_SCK(x)    GPIO_WriteBit(NRF_SCK_GPIO_PORT,NRF_SCK_GPIO_PIN,(BitAction)x)
#define NRF_IRQ       GPIO_ReadInputDataBit(NRF_IRQ_GPIO_PORT,NRF_IRQ_GPIO_PIN)

void NRF24L_Write_Buf( const uint8_t *pBuf, uint8_t Len );
void NRF24L_Read_Buf( uint8_t *pBuf, uint8_t Len );
uint8_t NRF24L_SendPacket( const uint8_t *pData, uint8_t Const );
uint8_t NRF24L_ReceivePacket( uint8_t *pData, uint8_t Const );
void NRF24L_Config(void);
uint8_t NRF24L_Init(void);


#endif /* __NRF24L01_H */


/*------------------------------- END OF FILE -------------------------------*/


