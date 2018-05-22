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
#include 	<SN32F240.h>
#include	"Node_Task_Key.h"
#include	"..\Driver\COM\COM.h"
#include	"..\UserApp\Sys_Task_Machine.h"
#include	"..\Gsm\Gsm_Task_Machine.h"
#include	"Node_Task_Ctrl.h"
#include	"Node_Task_Com.h"
#include	"..\Utility\Utility.h"
/*_____ D E F I N I T I O N S ______________________________________________*/
uint8_t	Node_Key_In[NODE_TOTAL_NUM];
uint8_t	Node_Key_Chk[NODE_TOTAL_NUM];
uint8_t	Node_Key_Cvt[NODE_TOTAL_NUM];
uint8_t	Node_Key_Old[NODE_TOTAL_NUM];
uint8_t	Node_Key_Change[NODE_TOTAL_NUM];
uint8_t	Node_Key_Chat[NODE_TOTAL_NUM];

/*_____ F U N C T I O N S __________________________________________________*/
/*****************************************************************************
* Function		: Node_Task_Key
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Node_Task_Key(void)
{
	uint8_t	 node_num;
	if(Sys_State !=SYS_NORMAL_STA) return;			//在上电或更新状态，不检测
	//==========================================================================
	if(Sys_Node_Evt_Str ==0)										//在通信中,不唤醒
	{
		for(node_num=0;node_num<NODE_TOTAL_NUM;node_num++)
		{
			S_ON_Out(node_num);										//开电唤醒
		}	
		UT_DelayNx10us(60);											//delay
		for(node_num=0;node_num<NODE_TOTAL_NUM;node_num++)
		{
			if(Node_Infor[node_num].Node_Status ==NODE_OUT_STA)
			{
				S_OFF_Out(node_num);								//关闭充电
			}
		}		
	}
	//==========================================================================
	for(node_num=0;node_num<NODE_TOTAL_NUM;node_num++)
	{
		Node_Key_In[node_num] =0;
		if(HDI_In(node_num) ==0) Node_Key_In[node_num] =1;
		//	
		if(Node_Key_In[node_num] == Node_Key_Chk[node_num])
		{
			if(Node_Key_Change[node_num] ==1)
			{
				if(Node_Key_Chat[node_num] ==0)
				{
					Node_Key_Cvt[node_num] = Node_Key_Chk[node_num];
					Node_Key_Change[node_num] =0;
				}
			}
		}
		else
		{
			Node_Key_Chk[node_num] = Node_Key_In[node_num];
			Node_Key_Change[node_num] = 1;
			Node_Key_Chat[node_num] =5;
		}
		//
		if(Node_Key_Cvt[node_num] !=Node_Key_Old[node_num])
		{
		 if(Node_Key_Cvt[node_num] ==1)
		 {
			 Wt_NodeEvent_Ff((node_num<<8)|NODE_IN_EVT);					//节点插入
		 }
		 else 
		 {
			 Wt_NodeEvent_Ff((node_num<<8)|NODE_OUT_EVT);					//节点取走
		 }				
			Node_Key_Old[node_num] = Node_Key_Cvt[node_num];
		}

		if(Node_Key_Chat[node_num] !=0)
		{
			Node_Key_Chat[node_num] --;
		}
	}			
}

