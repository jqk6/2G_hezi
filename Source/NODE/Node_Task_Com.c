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
#include	"Node_Task_Com.h"
#include	"Node_Task_Machine.h"
#include	"..\UserApp\Sys_Task_Machine.h"
#include	"..\Driver\Com\Com.h"
#include	"..\Driver\GPIO\GPIO.h"
#include	"..\Gsm\algorithm.h"
/*_____ D E F I N I T I O N S ______________________________________________*/
uint8_t		FNode_Com_Enb;
uint8_t		Com_Burp_Tm;
uint8_t		Node_Com_Step;
uint8_t		Com_Uart_Tx;
uint8_t		Com_Uart_Rx;
uint8_t		Com_Uart_Rxff[12];
uint8_t		Com_Uart_Rxpt;
uint8_t		Com_Uart_Cnt;
uint8_t		Node_Com_Err;
uint8_t		FNode_Com_Str;
uint8_t		FNode_Com_Stp;
uint8_t		Com_Uart_Tmout;
/*_____ F U N C T I O N S __________________________________________________*/

/*****************************************************************************
* Function		: Clear_Com_Rxff
* Description	: HDO tx HDI rx
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Clear_Com_Rxff(void)
{	
	uint8_t i;
	for(i=0;i<12;i++)												//清接收buffer
	{
		Com_Uart_Rxff[i] =0;
	}
}
/*****************************************************************************
* Function		: Node_Rd_Com
* Description	: HDO tx HDI rx
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Node_Com_Tx(uint8_t	Node_Num)
{
	uint8_t i;
	if(FNode_Com_Str ==0) return;
	//=========================================================================
	Clear_Com_Rxff();												//清接收buffer
	Com_Uart_Tx =0xA0|Node_Num;							//
	Com_Uart_Rxpt =0;												//
	Com_Uart_Tmout =2;											//
	//=========================================================================
	SN_CT16B2->TC =0;												//
	SN_CT16B2->MR0 =52;											//52us
	SN_CT16B2->IC =0xfff;										//
	HDO_Out_L(Node_Num);										//Start
	SN_CT16B2->TMRCTRL_b.CEN =1;						//Enable CT16B2
	while(SN_CT16B2->RIS_b.MR0IF ==0);
	SN_CT16B2->IC =0xfff;
	for(i=0;i<8;i++)
	{
		if((Com_Uart_Tx &0x01) ==0x01)				//
		{
			HDO_Out_H(W_Node_Num);							//
		}
		else
		{
			HDO_Out_L(W_Node_Num);							//
		}
		while(SN_CT16B2->RIS_b.MR0IF ==0);		//
		SN_CT16B2->IC =0xfff;	
		Com_Uart_Tx >>=1;
	}
	HDO_Out_H(W_Node_Num);									//Stop
	while(SN_CT16B2->RIS_b.MR0IF ==0);
	SN_CT16B2->IC =0xfff;		
	SN_CT16B2->TC =0;
	SN_CT16B2->TMRCTRL_b.CEN =0;
	//=========================================================================
	while(Com_Uart_Rxpt<11)
	{
		while(HDI_In(W_Node_Num)==1)
		{
			if(Com_Uart_Tmout==0)
			{
				SN_CT16B2->TMRCTRL_b.CEN =0;
				return;
			}
		}
		Com_Uart_Tmout =2;
		SN_CT16B2->TC =0;
		SN_CT16B2->IC =0xfff;	
		SN_CT16B2->MR0 =78;
		SN_CT16B2->TMRCTRL_b.CEN =1;	
		for(i=0;i<8;i++)
		{
			while(SN_CT16B2->RIS_b.MR0IF ==0);
			SN_CT16B2->IC =0xfff;		
			Com_Uart_Rx>>=1;
			if(HDI_In(W_Node_Num)==1)
			{
				Com_Uart_Rx |=0x80;
			}			
			SN_CT16B2->MR0 =52;	
		}
		Com_Uart_Rxff[Com_Uart_Rxpt++]=Com_Uart_Rx;
		while(HDI_In(W_Node_Num)==0)
		{
			if(Com_Uart_Tmout==0)
			{
				SN_CT16B2->TMRCTRL_b.CEN =0;
				return;	
			}
		}
	}
	SN_CT16B2->TMRCTRL_b.CEN =0;
}
/*****************************************************************************
* Function		: Node_Rd_Com
* Description	: HDO tx HDI rx
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Node_Com_Rx(uint8_t	Node_Num)
{
	uint16_t bat_ad;uint32_t id;
	if(FNode_Com_Str ==0) return;
	//=========================================================================
	SN_CT16B2->TMRCTRL_b.CEN =0;		//Disable Timer	
	//=========================================================================
	if((Com_Uart_Rxff[0] != ((0xa0|Node_Num)^0xff))||(Check_Sum(Com_Uart_Rxff,10)!=Com_Uart_Rxff[10]))
	{
		Clear_Com_Rxff();
		if(++Node_Com_Err>3)					//接收错误 
		{	Node_Com_Err =0;
			FNode_Com_Stp =1;
			FNode_Com_Str =0;
			Clear_Node_State(Node_Num);	//
		} 
		return;
	}
	Node_Infor[Node_Num].Node_Status =NODE_IN_STA;								//读取正确才算节点在
	Node_Infor[Node_Num].Bat_ID[0] =Com_Uart_Rxff[1];							//p ID
	Node_Infor[Node_Num].Bat_ID[1] =(Com_Uart_Rxff[2]>>4)+0x30;		//0
	Node_Infor[Node_Num].Bat_ID[2] =(Com_Uart_Rxff[2]&0x0f)+0x30;	//1
	Node_Infor[Node_Num].Bat_ID[3] ='0';													//0
	Node_Infor[Node_Num].Bat_ID[4] ='0';													//0
	Node_Infor[Node_Num].Bat_ID[5] ='0';													//0
	id =(Com_Uart_Rxff[6]<<24)|(Com_Uart_Rxff[5]<<16)|(Com_Uart_Rxff[4]<<8)|(Com_Uart_Rxff[3]);
	Node_Infor[Node_Num].Bat_ID[6] =(id /1000000000)+0x30;
	Node_Infor[Node_Num].Bat_ID[7] =(id%1000000000)/100000000+0x30;
	Node_Infor[Node_Num].Bat_ID[8] =(id%1000000000%100000000)/10000000+0x30;
	Node_Infor[Node_Num].Bat_ID[9] =(id%1000000000%100000000%10000000)/1000000+0x30;
	Node_Infor[Node_Num].Bat_ID[10] =(id%1000000000%100000000%10000000%1000000)/100000+0x30;
	Node_Infor[Node_Num].Bat_ID[11] =(id%1000000000%100000000%10000000%1000000%100000)/10000+0x30;
	Node_Infor[Node_Num].Bat_ID[12] =(id%1000000000%100000000%10000000%1000000%100000%10000)/1000+0x30;
	Node_Infor[Node_Num].Bat_ID[13] =(id%1000000000%100000000%10000000%1000000%100000%10000%1000)/100+0x30;
	Node_Infor[Node_Num].Bat_ID[14] =(id%1000000000%100000000%10000000%1000000%100000%10000%1000%100)/10+0x30;
	Node_Infor[Node_Num].Bat_ID[15] =(id%1000000000%100000000%10000000%1000000%100000%10000%1000%100%10)/1+0x30;	

	bat_ad =((Com_Uart_Rxff[7]<<8)|(Com_Uart_Rxff[8]))&0xfff;				//Bat Vol
	Node_Infor[Node_Num].Bat_Vol[0] = bat_ad/1000+0x30;
	Node_Infor[Node_Num].Bat_Vol[1] =(bat_ad%1000)/100+0x30;
	Node_Infor[Node_Num].Bat_Vol[2] =(bat_ad%1000%100)/10+0x30;
	Node_Infor[Node_Num].Bat_Vol[3] =(bat_ad%1000%100%10)/1+0x30;																												

	switch(Com_Uart_Rxff[9])																				//Bat Status	
	{
		case	0x11:	Node_Infor[Node_Num].Bat_Status[0] ='0'; break;
		case	0x10:	Node_Infor[Node_Num].Bat_Status[0] ='1';	break;
		case	0x01:	Node_Infor[Node_Num].Bat_Status[0] ='2';	break;
		case	0x00:	Node_Infor[Node_Num].Bat_Status[0] ='3';	break;
		default: Node_Infor[Node_Num].Bat_Status[0] ='0';break;
	}
	
	Node_Infor[Node_Num].M_State = (Com_Uart_Rxff[7]>>4)&0x01;			//读取电池铁锁状态
	
	FNode_Com_Stp =1;
	FNode_Com_Str =0;
	Clear_Com_Rxff();
}
/*****************************************************************************
* Function		: Check_Sum
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
uint8_t	Check_Sum(uint8_t *p,uint8_t len)
{
	uint8_t	i,result =0;
	for(i=0;i<len;i++)
	{
		result = *(p+i) +result;
	}
	return result;
}


