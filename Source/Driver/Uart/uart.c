/******************** (C) COPYRIGHT 2013 SONiX *******************************
* COMPANY:			SONiX
* DATE:					2013/12
* AUTHOR:				SA1
* IC:						SN32F700
* DESCRIPTION:	USART0 related functions.
*____________________________________________________________________________
* REVISION	Date				User		Description
* 1.0				2013/12/23	SA1			1. First release
*
*____________________________________________________________________________
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS TIME TO MARKET.
* SONiX SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL 
* DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH SOFTWARE
* AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN 
* IN CONNECTION WITH THEIR PRODUCTS.
*****************************************************************************/


/*_____ I N C L U D E S ____________________________________________________*/
#include <SN32F240.h>
#include "UART.h"
#include "..\..\UserApp\Sys_Task_Machine.h"
#include	"..\..\Gsm\Gsm_Task_Machine.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/
char	Uart_Rx_Buf[UART_RX_LEN];
char	Uart_Tx_Buf[450];

uint32_t 	Uart_Rx_Pt;
uint32_t	Uart_Rx_Cnt;
uint8_t		Uart_Rx_TmOut;


/*_____ D E F I N I T I O N S ______________________________________________*/


/*_____ M A C R O S ________________________________________________________*/


/*_____ F U N C T I O N S __________________________________________________*/

/*****************************************************************************
* Function		: Uart_Init
* Description	: Initialization of Uart1
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void Uart_Init(void)
{
	SN_SYS1->AHBCLKEN_b.USART1CLKEN = 1;			//Enables HCLK for USART1
	SN_USART1->LC = 0x3;											//Word Length = 8-bit character length, Diasble Parity
	SN_USART1->LC_b.DLAB = 1;								//Divisor Latch Access (0:Disable access to Divisor Latches, 1:Enable access to Divisor Latches)
	//USART PCLK = 48MHz, Baud rate = 115200
	SN_USART1->FD_b.DIVADDVAL =5;
	SN_USART1->FD_b.MULVAL =7;
	SN_USART1->DLM =0;
	SN_USART1->DLL =16;
	SN_USART1->LC_b.DLAB= 0;								//Divisor Latch Access (0:Disable access to Divisor Latches, 1:Enable access to Divisor Latches)
	SN_USART1->FIFOCTRL =0x01;
	SN_USART1->IE = 5;											//Enable RDA interrupt, RLS interrupt	
	SN_USART1->CTRL= 0xC1;									//Enable USART TX/RX
	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn, 0);
}
/*****************************************************************************
* Function		: Disable Uart
* Description	: Initialization of Uart1
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void Disable_Uart(void)
{
	SN_USART1->CTRL_b.USARTEN =0;									//Enable USART TX/RX
	NVIC_DisableIRQ(USART1_IRQn);
}
/*****************************************************************************
* Function		: Test_Uart_Init
* Description	: 
* Input				: data - data to be sent
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void Test_Uart_Init(void)
{
	SN_SYS1->AHBCLKEN_b.USART0CLKEN = 1;			//Enables HCLK for USART0
	SN_USART0->LC = 0x3;										//Word Length = 8-bit character length, Diasble Parity
	SN_USART0->LC_b.DLAB = 1;								//Divisor Latch Access (0:Disable access to Divisor Latches, 1:Enable access to Divisor Latches)
	//USART PCLK = 48MHz, Baud rate = 115200
	SN_USART0->FD_b.DIVADDVAL =5;
	SN_USART0->FD_b.MULVAL =7;
	SN_USART0->DLM =0;
	SN_USART0->DLL =16;
	SN_USART0->LC_b.DLAB= 0;								//Divisor Latch Access (0:Disable access to Divisor Latches, 1:Enable access to Divisor Latches)
	SN_USART1->FIFOCTRL =0x01;
	SN_USART0->IE = 5;											//Enable RDA interrupt, RLS interrupt	
	SN_USART0->CTRL= 0x81;									//Enable USART TX
}
/*****************************************************************************
* Function		: USART1_SendByte
* Description	: MCU sends Byte through UTXD1
* Input				: data - data to be sent
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void UART1_SendByte(uint32_t len)
{
	uint32_t	i;
	for(i=0;i<len;i++)
	{
		SN_USART1->TH = Uart_Tx_Buf[i]; 
		while ((SN_USART1->LS & USART1_LS_TEMT) == 0); 	//wait send over
	}
}
/*****************************************************************************
* Function		: USART1_SendByte
* Description	: MCU sends Byte through UTXD1
* Input				: data - data to be sent
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void UART0_SendByte(uint32_t len)
{
	uint32_t	i;
	for(i=0;i<len;i++)
	{
		SN_USART0->TH = Uart_Tx_Buf[i]; 
		while ((SN_USART0->LS & USART0_LS_TEMT) == 0); 	//wait send over
	}
}
/*****************************************************************************
* Function		: Uart_Tx_Print
* Description	: 
* Input				: 
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Uart_Tx_Print(uint32_t len,char *p)
{
	uint32_t i;
	SN_USART0->CTRL_b.USARTEN =1;	
	SN_USART0->TH = 'T'; 
	while ((SN_USART0->LS & USART0_LS_TEMT) == 0); 	//wait send over
	SN_USART0->TH = ':'; 
	while ((SN_USART0->LS & USART0_LS_TEMT) == 0); 	//wait send over
	
	for(i=0;i<len;i++)
	{
		SN_USART0->TH = *(p+i); 
		while ((SN_USART0->LS & USART0_LS_TEMT) == 0); 	//wait send over
	}		
	
	SN_USART0->TH = '\n'; 
	while ((SN_USART0->LS & USART0_LS_TEMT) == 0); 		//wait send over	
	SN_USART0->CTRL_b.USARTEN =0;	
}
void Uart1_Tx_Print(uint32_t len,char *p)
{
	uint32_t i;
	SN_USART1->CTRL_b.USARTEN =1;	
	SN_USART1->TH = 'T'; 
	while ((SN_USART1->LS & USART1_LS_TEMT) == 0); 	//wait send over
	SN_USART1->TH = ':'; 
	while ((SN_USART1->LS & USART1_LS_TEMT) == 0); 	//wait send over
	
	for(i=0;i<len;i++)
	{
		SN_USART1->TH = *(p+i); 
		while ((SN_USART1->LS & USART1_LS_TEMT) == 0); 	//wait send over
	}		
	
	SN_USART1->TH = '\n'; 
	while ((SN_USART1->LS & USART1_LS_TEMT) == 0); 		//wait send over	
	SN_USART1->CTRL_b.USARTEN =0;	
}

/*****************************************************************************
* Function		: Uart_Rx_Print
* Description	: 
* Input				: 
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Uart_Rx_Print(uint32_t len,char *p)
{
	uint32_t i;
	SN_USART0->CTRL_b.USARTEN =1;	
	SN_USART0->TH = 'R'; 
	while ((SN_USART0->LS & USART0_LS_TEMT) == 0); 	//wait send over
	SN_USART0->TH = ':'; 
	while ((SN_USART0->LS & USART0_LS_TEMT) == 0); 	//wait send over	

	for(i=0;i<len;i++)
	{
		SN_USART0->TH = *(p+i); 
		while ((SN_USART0->LS & USART0_LS_TEMT) == 0); 	//wait send over
	}	
	
	SN_USART0->TH = '\n'; 
	while ((SN_USART0->LS & USART0_LS_TEMT) == 0); 	//wait send over		
	SN_USART0->CTRL_b.USARTEN =0;	
}
/*****************************************************************************
* Function		: USART1_IRQHandler
* Description	: USART1 interrupt service routine
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
__irq void USART1_IRQHandler(void)
{
	uint32_t II_Buf;
	uint32_t Uart_Rx_tmp;
	
	II_Buf = SN_USART1->II;
	//==============================================================================
	if((II_Buf&0x0e)==0x4)								//Receive Data Available
	{
		Uart_Rx_Buf[Uart_Rx_Pt++] =SN_USART1->RB;					//保存接收数据
		if(Uart_Rx_Pt>=UART_RX_LEN) Uart_Rx_Pt=0;
		Uart_Rx_TmOut =2;																	//Clear Time out
	}
	else
	{
		Uart_Rx_tmp =SN_USART1->RB;
		Uart_Rx_tmp =SN_USART1->LS;
	}	
}
