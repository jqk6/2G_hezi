#ifndef __USART_H
#define __USART_H

/*_____ I N C L U D E S ____________________________________________________*/
#include	"stdint.h"

/*_____ D E F I N I T I O N S ______________________________________________*/
#define	UART_DEBUG 1
//Line Status
#define	USART1_LS_RDR	0x1
#define	USART1_LS_OE	0x2
#define	USART1_LS_PE	0x4
#define	USART1_LS_FE	0x8
#define	USART1_LS_BI	0x10
#define	USART1_LS_THRE	0x20
#define	USART1_LS_TEMT	0x40
#define	USART1_LS_RXFE	0x80
#define	USART1_LS_TXERR	0x100
#define	USART0_LS_TEMT	0x40
//Recv FIFO
#define USART_RECV_FIFO_LENGH	16

//Read FIFO
#define USART_READ_FIFO_LENGH	8


#define	UART_RX_LEN		800
/*_____ D E C L A R A T I O N S ____________________________________________*/
extern	char	Uart_Rx_Buf[800];
extern	char	Uart_Tx_Buf[450];

extern	uint8_t		Uart_Rx_TmOut;
extern	uint32_t Uart_Rx_Cnt;
extern	uint32_t Uart_Rx_Pt;

extern void Uart_Init(void);
extern	void Disable_Uart(void);
extern	void Test_Uart_Init(void);
extern void UART1_SendByte(uint32_t len);
extern void UART0_SendByte(uint32_t len);

extern	void	Uart_Tx_Print(uint32_t len,char *p);
extern	void	Uart_Rx_Print(uint32_t len,char *p);

void	Uart1_Tx_Print(uint32_t len,char *p);

#endif	/* __SN32F700_USART0_H */
