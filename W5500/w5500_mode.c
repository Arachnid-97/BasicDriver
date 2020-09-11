#include "w5500_mode.h"
#include "w5500_drv.h"
#include "w5500.h"


//uint8_t W5500_Tx_Buf[] = {0};
//uint8_t W5500_Rx_Buf[] = {0};

/************************************************
�������� �� W5500_TCP_Server
��    �� �� TCP����˴���
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void W5500_TCP_Server(void)
{
    uint16_t len = 0;
    uint8_t buf[2048] = {0};
    uint16_t temp = 0;

    temp = Get_Sn_SR(SOCK_TCP_S_PORT);

	switch(Get_Sn_SR(SOCK_TCP_S_PORT))										/*��ȡsocket��״̬*/
	{
		case SOCK_CLOSED:													/*socket���ڹر�״̬*/
                Socket_Config(SOCK_TCP_S_PORT, TCP_SERVER, Local_Net.LPort);/*��socket*/
                break;

		case SOCK_INIT:														/*socket�ѳ�ʼ��״̬*/
                Socket_Listen(SOCK_TCP_S_PORT);								/*socket��������*/
                break;

		case SOCK_ESTABLISHED:										        /*socket�������ӽ���״̬*/
                if(Get_Sn_IR(SOCK_TCP_S_PORT) & Sn_IR_CON)
                {
                    Set_Sn_IR(SOCK_TCP_S_PORT, Sn_IR_CON);					/*��������жϱ�־λ*/
                }

                len = Get_Sn_Rx_RSR(SOCK_TCP_S_PORT);
                if(len > 0)
                {
                    TCP_Buf_Recv(SOCK_TCP_S_PORT, buf, len);				/*��������Client������*/
//                    buf[len] = 0x00; 											/*����ַ���������*/
//                    printf("%s\r\n",buf);
//                    TCP_Buf_Send(SOCK_TCP_S_PORT, buf, len);					/*��Client��������*/
                }
                break;

		case SOCK_CLOSE_WAIT:												/*socket���ڵȴ��ر�״̬*/
                Socket_Colse(SOCK_TCP_S_PORT);
                break;

        default:
                break;
	}

}

/************************************************
�������� �� W5500_TCP_Client
��    �� �� TCP�ͻ��˴���
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void W5500_TCP_Client(void)
{
    uint16_t len = 0;
    uint8_t buf[2048] = {0};

	switch(Get_Sn_SR(SOCK_TCP_C_PORT))								  		/*��ȡsocket��״̬*/
	{
		case SOCK_CLOSED:											        /*socket���ڹر�״̬*/
                Socket_Config(SOCK_UDP_PORT, TCP_CLIENT, Local_Net.LPort);
                break;

		case SOCK_INIT:													    /*socket���ڳ�ʼ��״̬*/
                Socket_Connect(SOCK_TCP_C_PORT,Socket_0.RIp,Socket_0.RPort);/*socket���ӷ�����*/
                break;

		case SOCK_ESTABLISHED: 												/*socket�������ӽ���״̬*/
                if(Get_Sn_IR(SOCK_TCP_C_PORT) & Sn_IR_CON)
                {
                    Set_Sn_IR(SOCK_TCP_C_PORT, Sn_IR_CON); 					/*��������жϱ�־λ*/
                }

                len = Get_Sn_Rx_RSR(SOCK_TCP_C_PORT);
                if(len > 0)
                {
                    TCP_Buf_Recv(SOCK_TCP_C_PORT,buf,len); 					/*��������Server������*/
//                    buf[len] = 0x00;  										/*����ַ���������*/
//                    printf("%s\r\n",buf);
//                    TCP_Buf_Send(SOCK_TCP_C_PORT, buf, len);					/*��Server��������*/
                }
                break;

		case SOCK_CLOSE_WAIT: 											    /*socket���ڵȴ��ر�״̬*/
                Socket_Colse(SOCK_TCP_C_PORT);
                break;

        default:
                break;
	}
}

/************************************************
�������� �� W5500_UDP_Deal
��    �� �� UDP����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void W5500_UDP_Deal(void)
{
    uint16_t len = 0;
    uint8_t buf[2048] = {0};

	switch(Get_Sn_SR(SOCK_UDP_PORT))                                   /*��ȡsocket��״̬*/
	{
		case SOCK_CLOSED:                                              /*socket���ڹر�״̬*/
                Socket_Config(SOCK_UDP_PORT, UDP_MODE, Local_Net.LPort);/*��ʼ��socket*/
                break;

		case SOCK_UDP:                                                 /*socket��ʼ�����*/
                W5500_Delay_ms(10);
                if(Get_Sn_IR(SOCK_UDP_PORT) & Sn_IR_RECV)
                {
                    Set_Sn_IR(SOCK_UDP_PORT, Sn_IR_RECV);              /*������ж�*/
                }

                len = Get_Sn_Rx_RSR(SOCK_UDP_PORT);
                if(len > 0)                                           /*���յ�����*/
                {
                    UDP_Buf_Recv(SOCK_UDP_PORT, buf, len, Socket_0.RIp, Socket_0.RPort);    /*W5500���շ�����������*/
//                    buf[len - 8] = 0x00;                                                    /*����ַ���������*/
//                    printf("%s\r\n",buf);                                                   /*��ӡ���ջ���*/
//                    UDP_Buf_Send(SOCK_UDP_PORT, buf, len - 8, Socket_0.RIp, Socket_0.RPort);/*W5500�ѽ��յ������ݷ��͸�Remote*/
                }
                break;

        default:
                break;
	}

}

/************************************************
�������� �� Socket_Disconnect
��    �� �� Socket�Ͽ�
��    �� �� S ---- Socket number
�� �� ֵ �� ��
*************************************************/
void Socket_Disconnect( uint8_t S )
{
    Set_Sn_CR(S, Sn_CR_DISCON);
    while(Get_Sn_SR(S) != SOCK_CLOSED);     // �ȴ� Socket�Ͽ�
}

/************************************************
�������� �� Socket_Colse
��    �� �� Socket�ر�
��    �� �� S ---- Socket number
�� �� ֵ �� ��
*************************************************/
void Socket_Colse( uint8_t S )
{
    Set_Sn_CR(S, Sn_CR_CLOSE);
    while(Get_Sn_SR(S) != SOCK_CLOSED);     // �ȴ� Socket�ر�
    Set_Sn_IR(S, 0xFF);                     // �� Socket�����ж�
}

/************************************************
�������� �� TCP_Buf_Recv
��    �� �� TCP���ݽ���
��    �� �� S ---- Socket number
            pBuf ---- ����
            Len ---- ����
�� �� ֵ �� ��
*************************************************/
void TCP_Buf_Recv( uint8_t S, uint8_t *pBuf, uint16_t Len )
{
    uint16_t addr = 0;

    if(0 == Len)
    {
        return ;
    }

    addr = Get_Sn_Rx_RD(S);

    W5500_Read_Buf(addr, Sn_RX_BUFFER(S), pBuf, Len); // ��ȡ����

    addr += Len;
    Set_Sn_Rx_RD(S, addr);
    Set_Sn_CR(S, Sn_CR_RECV);
}

/************************************************
�������� �� TCP_Buf_Send
��    �� �� TCP���ݷ���
��    �� �� S ---- Socket number
            pBuf ---- ����
            Len ---- ����
�� �� ֵ �� ��
*************************************************/
void TCP_Buf_Send( uint8_t S, const uint8_t *pBuf, uint16_t Len )
{
    uint16_t addr = 0;

    if(0 == Len)
    {
        return ;
    }

    addr = Get_Sn_Tx_WR(S);

    W5500_Write_Buf(addr, Sn_TX_BUFFER(S), pBuf, Len);  // д������

    addr += Len;
    Set_Sn_Tx_WR(S, addr);
    Set_Sn_CR(S, Sn_CR_SEND);
}

/************************************************
�������� �� UDP_Buf_Recv
��    �� �� UDP���ݽ��գ�Э�飺IP(4 Byte) + Port(2 Byte) + data_len(2 Byte) + data(n Byte)��
��    �� �� S ---- Socket number
            pBuf ---- ����
            Len ---- ����
            Addr ---- remote IP
            Port ---- remote port
�� �� ֵ �� ��
*************************************************/
void UDP_Buf_Recv( uint8_t S, uint8_t *pBuf, uint16_t Len, uint8_t *Addr, uint16_t Port )
{
    uint8_t head[8] = {0};
    uint16_t addr = 0;
    uint16_t data_len = 0;

    if(0 == Len)
    {
        return ;
    }

    addr = Get_Sn_Rx_RD(S);
    W5500_Read_Buf(addr, Sn_RX_BUFFER(S), head, 8);
    addr += 8;

    /* ��ȡԶ�� IP */
    *Addr = head[0];
    *(Addr + 1) = head[1];
    *(Addr + 2) = head[2];
    *(Addr + 3) = head[3];

    /* ��ȡԶ�̶˿� */
    Port = head[4];
    Port = (Port << 8) + head[5];

    /* ��ȡ���ݳ��� */
    data_len = head[6];
    data_len = (data_len << 8) + head[7];

    W5500_Read_Buf(addr, Sn_RX_BUFFER(S), pBuf, data_len);  // ��ȡ����

    addr += data_len;
    Set_Sn_Rx_RD(S, addr);
    Set_Sn_CR(S, Sn_CR_RECV);
}

/************************************************
�������� �� UDP_Buf_Send
��    �� �� UDP���ݷ��ͣ�Э�飺IP(4 Byte) + Port(2 Byte) + data_len(2 Byte) + data(n Byte)��
��    �� �� S ---- Socket number
            pBuf ---- ����
            Len ---- ����
            Addr ---- remote IP
            Port ---- remote port
�� �� ֵ �� ��
*************************************************/
void UDP_Buf_Send( uint8_t S, const uint8_t *pBuf, uint16_t Len, const uint8_t *Addr, uint16_t Port )
{
    uint16_t addr = 0;

    if(((0 == *Addr) && (0 == *(Addr + 1)) \
        && (0 == *(Addr + 2)) && (0 == *(Addr + 3))) \
            || (0 == Port) || (0 == Len))
    {
        return ;
    }

    /* д��Զ�� IP���˿� */
    Set_Sn_DIPR(S, Addr);
    Set_Sn_DPORT(S, Port);

    addr = Get_Sn_Tx_WR(S);

    W5500_Write_Buf(addr, Sn_TX_BUFFER(S), pBuf, Len);  // д������

    addr += Len;
    Set_Sn_Tx_WR(S, addr);
    Set_Sn_CR(S, Sn_CR_SEND);
}

/************************************************
�������� �� Socket_Listen
��    �� �� ����ָ�� Socket(0~7)��Ϊ�������ȴ�Զ������������
��    �� �� S ---- Socket number
            Port ---- �˿ں�
�� �� ֵ �� 0 / 1
*************************************************/
_Bool Socket_Listen( uint8_t S )
{
    if(Get_Sn_SR(S) == SOCK_INIT)
    {
        Set_Sn_CR(S, Sn_CR_LISTEN);         // ���� SocketΪ����ģʽ
        W5500_Delay_ms(10);
        if(Get_Sn_SR(S) == SOCK_LISTEN)
        {
            return 1;
        }
    }

//    Set_Sn_CR(S, Sn_CR_CLOSE);
    return 0;
}

/************************************************
�������� �� Socket_Connect
��    �� �� ����ָ�� Socket(0~7)Ϊ�ͻ�����Զ�̷���������
��    �� �� S ---- Socket number
            Address ---- Զ�� IP
            Port ---- �˿ں�
�� �� ֵ �� 0 / 1
*************************************************/
_Bool Socket_Connect( uint8_t S, uint8_t *Address, uint16_t Port )
{
    if(Get_Sn_SR(S) == SOCK_INIT)
    {
        if(((Address[0] != 0xFF) && (Address[1] != 0xFF)
                && (Address[2] != 0xFF) && (Address[3] != 0xFF)) ||
           ((Address[0] == 0x00) && (Address[1] != 0x00)
                && (Address[2] != 0x00) && (Address[3] != 0x00)) ||
           (Port != 0x00))
        {
            Set_Sn_DIPR(S, Address);
            Set_Sn_DPORT(S, Port);
            Set_Sn_CR(S, Sn_CR_CONNECT);                 // ��������

            while(Get_Sn_SR(S) != SOCK_SYNSENT)
            {
                if(Get_Sn_SR(S) == SOCK_ESTABLISHED)
                {
                    return 1;
                }
                else if(Get_Sn_IR(S) & Sn_IR_TIMEOUT)
                {
                    Set_Sn_IR(S, Sn_IR_TIMEOUT);
                    break;
                }
            }
        }
    }

//    Set_Sn_CR(S, Sn_CR_CLOSE);
    return 0;
}

/************************************************
�������� �� Socket_TCP
��    �� �� ����ָ�� Socket(0~7)Ϊ TCPģʽ
��    �� �� S ---- Socket number
            Port ---- �˿ں�
�� �� ֵ �� 0 / 1
*************************************************/
_Bool Socket_TCP( uint8_t S, uint16_t Port )
{
    uint16_t temp_port = 2000;

    Socket_Colse(S);
    Set_Sn_MR(S, Sn_MR_TCP);
    if(Port != 0)
    {
        Set_Sn_PORT(S, Port);               // ���� Socket�˿�
    }
    else
    {
        temp_port = Local_Net.LPort + 1;
        Set_Sn_PORT(S, temp_port);
    }
    Set_Sn_CR(S, Sn_CR_OPEN);               // �� Socket
    W5500_Delay_ms(10);
    if(Get_Sn_SR(S) == SOCK_INIT)
    {
        return 1;
    }

    Set_Sn_CR(S, Sn_CR_CLOSE);
    return 0;
}

/************************************************
�������� �� Socket_UDP
��    �� �� ����ָ��Socket(0~7)Ϊ UDP(�㲥)ģʽ
��    �� �� S ---- Socket number
            Port ---- �˿ں�
�� �� ֵ �� 0 / 1
*************************************************/
_Bool Socket_UDP( uint8_t S, uint16_t Port )
{
    uint16_t temp_port = 2000;

    Socket_Colse(S);
    Set_Sn_MR(S, Sn_MR_UDP);
    if(Port != 0)
    {
        Set_Sn_PORT(S, Port);               // ���� Socket�˿�
    }
    else
    {
        temp_port = Local_Net.LPort + 1;
        Set_Sn_PORT(S, temp_port);
    }
    Set_Sn_CR(S, Sn_CR_OPEN);               // �� Socket
    W5500_Delay_ms(10);
    if(Get_Sn_SR(S) == SOCK_UDP)
    {
        return 1;
    }

    Set_Sn_CR(S, Sn_CR_CLOSE);
    return 0;
}

/************************************************
�������� �� Socket_MACRAW
��    �� �� ����Ϊ ��̫�� MACͨ��ģʽ
��    �� �� S ---- Socket number(��ΪSocket 0)
            Port ---- �˿ں�
�� �� ֵ �� 0 / 1
*************************************************/
_Bool Socket_MACRAW( uint8_t S, uint16_t Port )
{
    if(0 == S)
    {
        return 1;
    }

    return 0;
}

/************************************************
�������� �� Socket_Config
��    �� �� Socket ����
��    �� �� S ---- Socket number
            Protocol ---- Э�� >>>> �˿ڵ�����ģʽ
            Port ---- Socket�Ķ˿ں�
�� �� ֵ �� 0 / 1
*************************************************/
uint8_t Socket_Config( uint8_t S, uint8_t Protocol, uint16_t Port )
{
    uint8_t flag = 0;

    switch(Protocol)
    {
        case TCP_SERVER:
                if(Socket_TCP(S, Port))
                {
                    flag = TCP_SERVER;
                }
                break;

        case TCP_CLIENT:
                if(Socket_TCP(S, Port))
                {
                    flag = TCP_CLIENT;
                }
                break;

        case UDP_MODE:
                if(Socket_UDP(S, Port))
                {
                    flag = UDP_MODE;
                }
                break;

        case MACRAW_MODE:
                if(Socket_MACRAW(S, Port))
                {
                    flag = MACRAW_MODE;
                }
                break;

        default:
                Socket_Colse(S);
                break;
    }

    return flag;
}


/*---------------------------- END OF FILE ----------------------------*/


