#ifndef __W5500_REG_H
#define __W5500_REG_H


/* ----- Common Register Block ----- */
/* @brief Mode Register */
#define MR                           (0x0000)
/* @brief Gateway IP Address Register */
#define GAR0                         (0x0001)
#define GAR1                         (0x0002)
#define GAR2                         (0x0003)
#define GAR3                         (0x0004)
/* @brief Subnet Mask Register */
#define SUBR0                        (0x0005)
#define SUBR1                        (0x0006)
#define SUBR2                        (0x0007)
#define SUBR3                        (0x0008)
/* @brief Source MAC Address Register */
#define SHAR0                        (0x0009)
#define SHAR1                        (0x000A)
#define SHAR2                        (0x000B)
#define SHAR3                        (0x000C)
#define SHAR4                        (0x000D)
#define SHAR5                        (0x000E)
/* @brief Source IP Address Register */
#define SIPR0                        (0x000F)
#define SIPR1                        (0x0010)
#define SIPR2                        (0x0011)
#define SIPR3                        (0x0012)
/* @brief Interrupt Low Level Timer Register */
#define INTLEVEL0                    (0x0013)
#define INTLEVEL1                    (0x0014)
/* @brief Interrupt Register */
#define IR                           (0x0015)
/* @brief Interrupt Mask Register */
#define IMR                          (0x0016)
/* @brief Socket Interrupt Register */
#define SIR                          (0x0017)
/* @brief Socket Interrupt Mask Register */
#define SIMR                         (0x0018)
/* @brief Timeout register address( 1 is 100us ) */
#define RTR0                         (0x0019)
#define RTR1                         (0x001A)
/* @brief Retry Count reigster */
#define RCR                          (0x001B)
/* @brief PPP LCP Request Timer register */
#define PTIMER                       (0x001C)
/* @brief PPP LCP Magic number register */
#define PMAGIC                       (0x001D)
/* @brief Destination MAC Address Register in PPPoE mode */
#define PHAR0                        (0x001E)
#define PHAR1                        (0x001F)
#define PHAR2                        (0x0020)
#define PHAR3                        (0x0021)
#define PHAR4                        (0x0022)
#define PHAR5                        (0x0023)
/* @brief Session ID Register in PPPoE mode */
#define PSID0                        (0x0024)
#define PSID1                        (0x0025)
/* @brief Maximum Receive Unit in PPPoE mode */
#define PMRU0                        (0x0026)
#define PMRU1                        (0x0027)
/* @brief Unreachable IP Address Register */
#define UIPR0                        (0x0028)
#define UIPR1                        (0x0029)
#define UIPR2                        (0x002A)
#define UIPR3                        (0x002B)
/* @brief Unreachable Port Register */
#define UPORT0                       (0x002C)
#define UPORT1                       (0x002D)
/* @brief W5500 PHY Configuration Register */
#define PHYCFGR                      (0x002E)
/* @brief W5500 Chip Version Register */
#define VERSIONR                     (0x0039)
/* ----- END ----- */

/* ----- Socket Register Block ----- */
/* @brief socket Mode register */
#define Sn_MR                        (0x0000)
/* @brief Socket n Command Register */
#define Sn_CR                        (0x0001)
/* @brief Socket n Command Register */
#define Sn_IR                        (0x0002)
/* @brief Socket n Status Register */
#define Sn_SR                        (0x0003)
/* @brief Socket n Source Port Register */
#define Sn_PORT0                     (0x0004)
#define Sn_PORT1                     (0x0005)
/* @brief Socket n Destination Hardware Address Register */
#define Sn_DHAR0                     (0x0006)
#define Sn_DHAR1                     (0x0007)
#define Sn_DHAR2                     (0x0008)
#define Sn_DHAR3                     (0x0009)
#define Sn_DHAR4                     (0x000A)
#define Sn_DHAR5                     (0x000B)
/* @brief Socket n Destination IP Address Register */
#define Sn_DIPR0                     (0x000C)
#define Sn_DIPR1                     (0x000D)
#define Sn_DIPR2                     (0x000E)
#define Sn_DIPR3                     (0x000F)
/* @brief Socket n Destination Port Register */
#define Sn_DPORT0                    (0x0010)
#define Sn_DPORT1                    (0x0011)
/* @brief Socket n Maximum Segment Size Register */
#define Sn_MSSR0                     (0x0012)
#define Sn_MSSR1                     (0x0013)
/* @brief Socket n IP Type of Service(TOS) Register */
#define Sn_TOS                       (0x0015)
/* @brief Socket n TTL Register */
#define Sn_TTL                       (0x0016)
/* @brief Socket n RX Buffer Size Register */
#define Sn_RXBUF_SIZE                (0x001E)
/* @brief Socket n TX Buffer Size Register */
#define Sn_TXBUF_SIZE                (0x001F)
/* @brief Socket n TX Free Size Register */
#define Sn_TX_FSR0                   (0x0020)
#define Sn_TX_FSR1                   (0x0021)
/* @brief Socket n TX Read Pointer Register */
#define Sn_TX_RD0                    (0x0022)
#define Sn_TX_RD1                    (0x0023)
/* @brief Socket n TX Write Pointer Register */
#define Sn_TX_WR0                    (0x0024)
#define Sn_TX_WR1                    (0x0025)
/* @brief Socket n Received Size Register */
#define Sn_RX_RSR0                   (0x0026)
#define Sn_RX_RSR1                   (0x0027)
/* @brief Socket n RX Read Data Pointer Register */
#define Sn_RX_RD0                    (0x0028)
#define Sn_RX_RD1                    (0x0029)
/* @brief Socket n RX Write Pointer Register */
#define Sn_RX_WR0                    (0x002A)
#define Sn_RX_WR1                    (0x002B)
/* @brief Socket n Interrupt Mask Register */
#define Sn_IMR                       (0x002C)
/* @brief Socket n Keep Alive Time Register */
#define Sn_FRAG                      (0x002D)
/* @brief Socket n Keep Alive Time Register */
#define Sn_KPALVTR                   (0x002F)
/* ----- END ----- */

/* ----- Control Phase ----- */
#define COMM_REG                     (0x00)
#define Sn_REG(ch)                   (0x08 | (ch << 5))
#define Sn_TX_BUFFER(ch)             (0x10 | (ch << 5))
#define Sn_RX_BUFFER(ch)             (0x18 | (ch << 5))

/* ----- END ----- */

/* PHYCFGR register values */
#define RST_PHY		0x80
#define OPMODE		0x40
#define DPX			0x04
#define SPD			0x02
#define LINK		0x01

/* MODE register values */
#define MR_RST                       0x80     /**< reset */
#define MR_WOL                       0x20     /**< Wake on Lan */
#define MR_PB                        0x10     /**< ping block */
#define MR_PPPOE                     0x08     /**< enable pppoe */
#define MR_UDP_FARP                  0x02     /**< enbale FORCE ARP */

/* IR register values */
#define IR_CONFLICT                  0x80     /**< check ip confict */
#define IR_UNREACH                   0x40     /**< get the destination unreachable message in UDP sending */
#define IR_PPPoE                     0x20     /**< get the PPPoE close message */
#define IR_MAGIC                     0x10     /**< get the magic packet interrupt */

/* Sn_MR values */
#define Sn_MR_UCASTB                 0x10     /**< Unicast Block in UDP Multicating*/
#define Sn_MR_ND                     0x20     /**< No Delayed Ack(TCP) flag */
#define Sn_MR_MC                     0x20     /**< Multicast IGMP (UDP) flag */
#define Sn_MR_BCASTB                 0x40     /**< Broadcast blcok in UDP Multicating */
#define Sn_MR_MULTI                  0x80     /**< support UDP Multicating */

/* Sn_MR values of Protocol */
#define Sn_MR_CLOSE                  0x00     /**< unused socket */
#define Sn_MR_TCP                    0x01     /**< TCP */
#define Sn_MR_UDP                    0x02     /**< UDP */
#define Sn_MR_MACRAW                 0x04     /**< MAC LAYER RAW SOCK */

/* Sn_MR values on MACRAW MODE */
#define Sn_MR_MIP6N                  0x10     /**< IPv6 packet Block */
#define Sn_MR_MMB                    0x20     /**< IPv4 Multicasting Block */
#define Sn_MR_BCASTB                 0x40     /**< Broadcast blcok */
#define Sn_MR_MFEN                   0x80     /**< support MAC filter enable */

/* Sn_CR values */
#define Sn_CR_OPEN                   0x01     /**< initialize or open socket */
#define Sn_CR_LISTEN                 0x02     /**< wait connection request in tcp mode(Server mode) */
#define Sn_CR_CONNECT                0x04     /**< send connection request in tcp mode(Client mode) */
#define Sn_CR_DISCON                 0x08     /**< send closing reqeuset in tcp mode */
#define Sn_CR_CLOSE                  0x10     /**< close socket */
#define Sn_CR_SEND                   0x20     /**< update txbuf pointer, send data */
#define Sn_CR_SEND_MAC               0x21     /**< send data with MAC address, so without ARP process */
#define Sn_CR_SEND_KEEP              0x22     /**< send keep alive message */
#define Sn_CR_RECV                   0x40     /**< update rxbuf pointer, recv data */

//#ifdef __DEF_IINCHIP_PPP__
//   #define Sn_CR_PCON                0x23
//   #define Sn_CR_PDISCON             0x24
//   #define Sn_CR_PCR                 0x25
//   #define Sn_CR_PCN                 0x26
//   #define Sn_CR_PCJ                 0x27
//#endif

/* Sn_IR values */
#ifdef __DEF_IINCHIP_PPP__
   #define Sn_IR_PRECV               0x80
   #define Sn_IR_PFAIL               0x40
   #define Sn_IR_PNEXT               0x20
#endif

#define Sn_IR_SEND_OK                0x10     /**< complete sending */
#define Sn_IR_TIMEOUT                0x08     /**< assert timeout */
#define Sn_IR_RECV                   0x04     /**< receiving data */
#define Sn_IR_DISCON                 0x02     /**< closed socket */
#define Sn_IR_CON                    0x01     /**< established connection */

/* Sn_SR values */
#define SOCK_CLOSED                  0x00     /**< closed */
#define SOCK_INIT                    0x13     /**< init state */
#define SOCK_LISTEN                  0x14     /**< listen state */
#define SOCK_SYNSENT                 0x15     /**< connection state */
#define SOCK_SYNRECV                 0x16     /**< connection state */
#define SOCK_ESTABLISHED             0x17     /**< success to connect */
#define SOCK_FIN_WAIT                0x18     /**< closing state */
#define SOCK_CLOSING                 0x1A     /**< closing state */
#define SOCK_TIME_WAIT               0x1B     /**< closing state */
#define SOCK_CLOSE_WAIT              0x1C     /**< closing state */
#define SOCK_LAST_ACK                0x1D     /**< closing state */
#define SOCK_UDP                     0x22     /**< udp socket */
//#define SOCK_IPRAW                   0x32     /**< ip raw mode socket */
#define SOCK_MACRAW                  0x42     /**< mac raw mode socket */
//#define SOCK_PPPOE                   0x5F     /**< pppoe socket */

/* IP PROTOCOL */
#define IPPROTO_IP                   0        /**< Dummy for IP */
#define IPPROTO_ICMP                 1        /**< Control message protocol */
#define IPPROTO_IGMP                 2        /**< Internet group management protocol */
#define IPPROTO_GGP                  3        /**< Gateway^2 (deprecated) */
#define IPPROTO_TCP                  6        /**< TCP */
#define IPPROTO_PUP                  12       /**< PUP */
#define IPPROTO_UDP                  17       /**< UDP */
#define IPPROTO_IDP                  22       /**< XNS idp */
#define IPPROTO_ND                   77       /**< UNOFFICIAL net disk protocol */
#define IPPROTO_RAW                  255      /**< Raw IP packet */


#endif /* __W5500_REG_H */


/*---------------------------- END OF FILE ----------------------------*/


