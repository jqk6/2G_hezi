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
#include	"Node_Task_AD.h"
#include	"..\UserApp\Sys_Task_Machine.h"
#include	"Node_Task_Ctrl.h"
/*_____ D E F I N I T I O N S ______________________________________________*/
uint8_t		AD_Burp_SCnt;
uint8_t		AD_Burp_NSCnt;
uint8_t		Node_AD_Cnt;
uint8_t		FNode_AD_Enb;
uint8_t		FNode_AD_Str;
uint8_t		FNode_AD_Stp;
/*_____ F U N C T I O N S __________________________________________________*/
/*****************************************************************************
* Function		: Node_Task_Rd_AD
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Node_Task_AD(void)
{
	if(FNode_AD_Str ==0) return;
	Node_Rd_AD(W_Node_Num);
}
/*****************************************************************************
* Function		: Node_Rd_AD
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Node_Rd_AD(uint8_t	Node_Num)
{
	if(Node_Infor[Node_Num].Node_Status ==NODE_OUT_STA)		//节点取走
	{
		Node_Infor[Node_Num].Node_AD_Status  =BAT_NSHORT;		//未短路或过流
		FNode_AD_Stp =1;
	}
	else
	{
		if(Rd_AD_Dt(Node_Num)<0x10)
		{
			AD_Burp_NSCnt =0;													//
			if(++AD_Burp_SCnt>=2) 										//
			{
				Node_Infor[Node_Num].Node_AD_Status  =BAT_SHORT;	//短路
				S_OFF_Out(Node_Num);										//关闭输出
				FNode_AD_Stp =1;
			}
		}
		else
		{
			AD_Burp_SCnt =0;													//
			if(++AD_Burp_NSCnt>=2) 										//
			{
				Node_Infor[Node_Num].Node_AD_Status  =BAT_NSHORT;	//未短路或过流
				FNode_AD_Stp =1;
			}
		}
	}
}
/*****************************************************************************
* Function		: Node_Rd_AD
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
uint32_t	Rd_AD_Dt(uint8_t	Node_Num)
{
	uint8_t i;uint32_t ADC_Sum;
	
	switch (Node_Num)
	{
		case NODE1:	SN_ADC->ADM_b.CHS =AIN3;	break;
		case NODE2:	SN_ADC->ADM_b.CHS =AIN4;	break;
		case NODE3:	SN_ADC->ADM_b.CHS =AIN5;	break;
		case NODE4:	SN_ADC->ADM_b.CHS =AIN6;	break;
		case NODE5:	SN_ADC->ADM_b.CHS =AIN7;	break;
		case NODE6:	SN_ADC->ADM_b.CHS =AIN8;	break;
		case NODE7:	SN_ADC->ADM_b.CHS =AIN9;	break;
		case NODE8:	SN_ADC->ADM_b.CHS =AIN10;	break;		
		default: break;
	}
	ADC_Sum =0;
	for(i=0;i<8;i++)
	{
		SN_ADC->ADM_b.ADS =1;								//AD Start
		while(SN_ADC->ADM_b.EOC ==0);				//wait AD Over
		ADC_Sum =ADC_Sum + (SN_ADC->ADB &0xfff);
	}
	ADC_Sum >>=3;
	return ADC_Sum;
}
