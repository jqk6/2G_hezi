/******************** (C) COPYRIGHT 2014 SONiX *******************************
* COMPANY:					SONiX
* DATE:						2017/04/27
* AUTHOR:					SA1
* IC:							SN32F700B
* DESCRIPTION:	 	Main functions.
*____________________________________________________________________________
* REVISION	Date				User		Description
* 014+G05		2017/09/01	bear		1.ED18
*	
___________________________________________________________________
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS TIME TO MARKET.
* SONiX SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL 
* DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH SOFTWARE
* AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN 
* IN CONNECTION WITH THEIR PRODUCTS.
*****************************************************************************/
/*_____ I N C L U D E S ____________________________________________________*/
#include	<SN32F240.h>
#include	"UserApp\Sys_Task_Machine.h"
#include	"IntAndGnd\IntAndGnd_Task_Machine.h"
#include	"Gsm\GSM_Task_Machine.h"
#include	"Node\Node_Task_Machine.h"

#include	"Driver\Uart\Uart.h"

/*_____ D E F I N I T I O N S ______________________________________________*/


/*_____ D E C L A R A T I O N S ____________________________________________*/

#include	"NODE/Node_Task_Ctrl.h"
/*_____ F U N C T I O N S __________________________________________________*/
int main(void)
{
	Sys_Init();												//Initial System
	
	//M_ON_Init(7);
	while(1)
	{
		SN_WDT->FEED =0x5AFA55AA;				//Clear Watch Dog	
		IntAndGnd_Task_Machine();				//Time Task
		Node_Task_Machine();						//Node Task
		Sys_Task_Machine();							//Sysem Task
		GSM_Task_Machine();							//Gsm Tak
	}
}
/*****************************************************************************
* Function		: HardFault_Handler
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
__irq void	HardFault_Handler(void)
{
	NVIC_SystemReset();
}