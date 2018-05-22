/******************** (C) COPYRIGHT 2014 SONiX *******************************
* COMPANY:			SONiX
* DATE:				2015/03/17
* AUTHOR:			SA1
* IC:				SN32F700
* DESCRIPTION:	 	System state functions.
*____________________________________________________________________________
* REVISION	Date				User		Description
* 1.0			  2015/03/31	SA1			1. First release
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
#include 	<SN32F240.h>
#include	"Sys_Task_Machine.h"
#include 	"..\Driver\GPIO\gpio.h"
#include	"..\Driver\Timer\Timer.h"
#include	"..\Driver\Uart\Uart.h"
#include	"..\Driver\AD\AD.h"
#include	"..\GSM\Gsm_Task_Machine.h"
#include	"..\Node\Node_Task_Machine.h"
#include	"..\IntAndGnd\IntAndGnd_Task_Machine.h"
#include	"..\Node\Node_Task_Key.h"
#include	"..\Node\Node_Task_AD.h"
#include	"..\Node\Node_Task_Ctrl.h"
#include	"..\Node\Node_Task_Com.h"
#include	"..\Driver\Flash\Flash.h"
#include	"..\Driver\SPI\SPI.h"
#include	"..\Utility\Utility.h"
#include	"..\Driver\WDT\WDT.h"
/*_____ D E F I N I T I O N S ______________________________________________*/
uint8_t		Sys_State;
uint8_t		Sys_RdNode_Cnt;
uint8_t		Sys_NodeTask_Step;
uint8_t		Sys_GsmTask_Step;
uint8_t		Sys_Power_On;
uint32_t	Rolling_Code;
uint8_t		Sys_Node_Evt_Str;
/*_____ F U N C T I O N S __________________________________________________*/

/*****************************************************************************
* Function		: Sys_Task_Machine
* Description	: System Task
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void Sys_Task_Machine(void)
{	
	//=====================================================================
	switch(Sys_NodeTask_Step)
	{
		case 0:
			W_Node_Event =Rd_NodeEvent_Ff();						//检测是否有节点事件发生
			if(W_Node_Event ==NODE_NO_EVT)	break;			//
			Initial_Node_Event();												//
		break;
		case 1:																				//检测通信
			Node_Com_Over();
		break;
		case	2:																			//检测AD
			Node_AD_Over();															//
		break;
		default:	break;
	}
	//=====================================================================
	switch(Sys_GsmTask_Step)
	{
		case 0:
			Gsm_Event =Rd_GsmEvent_Ff();									//检测是否有GSM发送事件发生
			if(Gsm_Event ==GSM_NO_EVT)	break;						//
			Sys_GsmTask_Step =1;													//		
		break;
		case 1:
			Init_Gsm_Report();														//	
		break;
		case 2:
			if((FGsm_HTTP_Rx_OK ==1)||(FGsm_MQTT_Tx_OK==1))		
			{
				FGsm_HTTP_Rx_OK =0;
				FGsm_MQTT_Tx_OK =0;
				Sys_GsmTask_Step =0;
				Gsm_Report_Over();
			}
		break;
		default:	break;
	}
}
/*****************************************************************************
* Function		: Initial_Node_Event
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/	
void	Initial_Node_Event(void)
{	
	W_Node_Num =(W_Node_Event>>8)&0x07;				//Node Num
	W_Node_Event = W_Node_Event &0xff;				//Node Event	
	switch(W_Node_Event)
	{
		case 	NODE_IN_EVT:											//节点插入
		case	NODE_OFF_EVT:											//节点关闭  
		case	NODE_ALL_EVT:											//读取所有节点
		case	NODE_ONFAIL_EVT:									//节点打开失败
		case	NODE_ONEREP_EVT:									//上报单个节点信息
			Initial_Node_Handle();								
		break;
		case	NODE_ON_REQ_EVT:									//服务器请求打开节点
			Initial_Node_Onreq_Handle();
		break;
		case 	NODE_ON_EVT:											//节点打开
			Initial_Node_On_Handle();			
		break;
		case 	NODE_OUT_EVT:											//节点取走
			Initial_Node_Out_Handle();
		break;
		default: break;
	}
}
/*****************************************************************************
* Function		: Initial_Sys_Normal
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/	
void	Initial_Sys_Normal(void)
{
	Sys_Power_On =1;
	Sys_State =SYS_NORMAL_STA;				//Inot Normal 	
	Clear_Node_Ff();									//
	Init_M_S_LED();
}
/*****************************************************************************
* Function		: Sys_Init
* Description	: Initial System
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void Sys_Init(void)
{
	SN_PMU->LATCHCTRL2=0x5A5A0001;
	while (SN_PMU->LATCHST == 0x1);
	SN_PMU->LATCHCTRL2 = 0x5A5A0000;
	SN_PMU->LATCHCTRL1 = 0x5A5A0000;
	
	Sysclk_Init();											//Initial system clock
	PFPA_Init();												//Initial PFPA
	GPIO_Init();												//Initial All Gpio
	Systick_Init();											//Initial timer 
	AD_Init();													//Initial AD
	Uart_Init();												//Initial Uart
	CT32B0_Init();											//Initial CT32B0
	CT32B1_Init();											//Initial CT32B1
	CT32B2_Init();											//Initial CT32B2
	CT16B2_Init();											//Initial CT16B2
	CT16B1_Init();											//Initial CT16B1
	SPI_Init();													//Initial SSP0
	WDT_Init();													//Initial Wdt
#if UART_DEBUG ==1
	Test_Uart_Init();
#endif	
	Premain();													//
}
/*****************************************************************************
* Function		: Premain
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void Premain(void)
{
	UT_DelayNms(200);											//Power On delay
	Sys_Power_On =0;											//
	Sys_State =SYS_PWRON_STA;							//Power On state
	Gsm_State = GSM_PWR_STA;							//Gsm Power On state
	Gsm_Pwr_State =GSM_VOL_ON_STA;				//
	Node_Link_State_Init();								//读取节点连接状态
	Rolling_Code =*(uint32_t*)(0xFF24);		//读取生产批次 
}
/*****************************************************************************
* Function		: Sysclk_Init
* Description	: Initial System clock PLL48M
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void 	Sysclk_Init(void)
{
	SystemInit();
}
/*****************************************************************************
* Function		: Wt_GsmEvent_Ff
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Wt_GsmEvent_Ff(uint16_t	gsm_event)
{
	GsmEvent_Ff[Wt_GsmEvent_Pt] = gsm_event;
	if(++Wt_GsmEvent_Pt >=GSM_EVT_FF_LEN) Wt_GsmEvent_Pt=0;
}
/*****************************************************************************
* Function		: Rd_NodeEvent_Ff
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
uint16_t	Rd_GsmEvent_Ff(void)
{
	uint16_t gsm_event;
	if(Wt_GsmEvent_Pt !=Rd_GsmEvent_Pt)
	{
		gsm_event =GsmEvent_Ff[Rd_GsmEvent_Pt];
		if(++Rd_GsmEvent_Pt>=GSM_EVT_FF_LEN) Rd_GsmEvent_Pt=0;
		return gsm_event;
	}
	else
	{
		return GSM_NO_EVT;					//没有事件发生
	}
}
/*****************************************************************************
* Function		: Sys_Save_Flash
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Sys_Save_Flash(void)
{
	uint32_t	Flash_Buf[20];
	
	SN_SYS0->AHBCP =2;		///4
	Flash_Buf[0] =SecKey[0]|(SecKey[1]<<8)|(SecKey[2]<<16)|(SecKey[3]<<24);					//Secky
	Flash_Buf[1] =SecKey[4]|(SecKey[5]<<8)|(SecKey[6]<<16)|(SecKey[7]<<24);
	Flash_Buf[2] =SecKey[8]|(SecKey[9]<<8)|(SecKey[10]<<16)|(SecKey[11]<<24);
	Flash_Buf[3] =SecKey[12]|(SecKey[13]<<8)|(SecKey[14]<<16)|(SecKey[15]<<24);
	Flash_Buf[4] =ICCID_ID[0]|(ICCID_ID[1]<<8)|(ICCID_ID[2]<<16)|(ICCID_ID[3]<<24);	//ICCID
	Flash_Buf[5] =ICCID_ID[4]|(ICCID_ID[5]<<8)|(ICCID_ID[6]<<16)|(ICCID_ID[7]<<24);			
	Flash_Buf[6] =ICCID_ID[8]|(ICCID_ID[9]<<8)|(ICCID_ID[10]<<16)|(ICCID_ID[11]<<24);
	Flash_Buf[7] =ICCID_ID[12]|(ICCID_ID[13]<<8)|(ICCID_ID[14]<<16)|(ICCID_ID[15]<<24);
	Flash_Buf[8] =ICCID_ID[16]|(ICCID_ID[17]<<8)|(ICCID_ID[18]<<16)|(ICCID_ID[19]<<24);	
	Flash_Buf[9] =Rolling_Code;												//DeviD
	Flash_Buf[10] =Rom_VerTab;												//Rom Vertion
	FLASH_EraseSector(0xFC00);												//擦除flash	
	UT_DelayNms(1);
	FLASH_ProgramPage(0xFF00,44,(uint8_t*)Flash_Buf);	//写flash
	Flash_Buf[0] =0xAAAA5555;
	FLASH_ProgramPage(0xFFFC,4,(uint8_t*)Flash_Buf);	//写flash
	SN_SYS0->AHBCP =0;		///4
}
