/******************** (C) COPYRIGHT 2014 SONiX *******************************
* COMPANY:			SONiX
* DATE:					2015/03/17
* AUTHOR:				SA1
* IC:						SN32F700
* DESCRIPTION:	Timer related functions.
*____________________________________________________________________________
* REVISION	Date				User		Description
* 1.0				2015/03/31	 SA1		1. First release
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
#include	<SN32F240.h>
#include	"Node_Task_Machine.h"
#include	"Node_Task_Key.h"
#include	"Node_Task_AD.h"
#include	"Node_Task_Com.h"
#include	"Node_Task_Ctrl.h"
#include	"..\Driver\Com\Com.h"
#include	"..\Driver\GPIO\GPIO.h"
#include	"..\UserApp\Sys_Task_Machine.h"
#include	"..\GSM\Gsm_Task_Machine.h"
/*_____ D E F I N I T I O N S ______________________________________________*/
/*_____ D E C L A R A T I O N S ____________________________________________*/
uint8_t		W_Node_Num;
uint16_t	W_Node_Event;
NODE_INFORMATION	Node_Infor[8];
uint16_t	NodeEvent_Ff[NODE_EVT_FF_LEN];
uint8_t		Wt_NodeEvent_Pt;
uint8_t		Rd_NodeEvent_Pt;
uint8_t		Node_Burp_Tm;
uint8_t		Node_Task_Step;
uint8_t		Node_Com_Tm;
uint16_t	Node_Close_Tm[NODE_TOTAL_NUM];			//节点插入，延迟上报时间
/*_____ F U N C T I O N S __________________________________________________*/

/*****************************************************************************
* Function		: Node_Link_State_Init
* Description	: 上电检测所有节点状态初始化节点
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Node_Link_State_Init(void)
{	
	uint32_t i,j,keyin,keychk;
	//=========================================================================
	j=0;
	while(j<0x2000)
	{
		keyin =0;
		for(i=0;i<NODE_TOTAL_NUM;i++)
		{
			if(HDI_In(i)==0) keyin |=(1<<i);
		}
		if(keyin ==keychk)
		{
			j++;
		}
		else
		{
			j =0;
			keychk =keyin;
		}
	}
	//=========================================================================
	for(i=0;i<NODE_TOTAL_NUM;i++)
	{
		Clear_Node_State(i);					//初始化节点信息
		S_ON_Out(i);									//打开充电
		M_OFF_Init(i);								//关闭电磁铁
		Node_Close_Tm[i] =0;					//清节点关闭延迟时间
		Node_Key_In[i] =keychk &0x01;
		Node_Key_Chk[i] =keychk &0x01;
		Node_Key_Cvt[i] =keychk &0x01;
		Node_Key_Old[i] =keychk &0x01;
		keychk >>=1;
	}
	LED_ALLFlash_Init(NODE_TOTAL_NUM,TM_LED_FLASH_NONET);			//上电MQTT和基站都未连接,进入快闪状态,
	W_Node_Num =0;
	Wt_NodeEvent_Ff(NODE_ALL_EVT);	//读取所有节点事件
}
/*****************************************************************************
* Function		: Clear_Node_Ff
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Clear_Node_Ff(void)
{ uint8_t i;
	Sys_NodeTask_Step =0;							//	
	Wt_NodeEvent_Pt =0;
	Rd_NodeEvent_Pt =0;
	for(i=0;i<NODE_EVT_FF_LEN;i++) NodeEvent_Ff[i] =0;
}
/*****************************************************************************
* Function		: Node_Task_Machine
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Node_Task_Machine(void)
{
	if(Node_Burp_Tm!=0) return;
	Node_Burp_Tm =2;
	switch(Node_Task_Step)
	{
		case 0:
			Node_Task_Key();							//Read Node State
		break;
		case 1:
			Node_Com_Tx(W_Node_Num);			//Read Bat information			
		break;
		case 2:
			Node_Com_Rx(W_Node_Num);				//
			Node_Task_AD();									//Read Bat AD
		break;
		default:	break;
	}
	if(++Node_Task_Step>=3) Node_Task_Step=0;
}
/*****************************************************************************
* Function		: Node_Com_Over
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Node_Com_Over(void)
{
	switch(Node_Com_Step)
	{
		case 0:
			if(Node_Com_Tm==0)
			{
				S_OFF_Out(W_Node_Num);					//关闭充电
				Node_Com_Tm =TM_S_ONOFF;
				Node_Com_Step =1;
			}
		break;
		case 1:
			if(Node_Com_Tm==0)								//
			{
				S_ON_Out(W_Node_Num);						//打开充电
				Node_Com_Step =2;
				FNode_Com_Str =1;								//打大通讯
				FNode_Com_Stp =0;								//
				Node_Com_Err =0;								//
			}
		break;
		case 2:
			if(FNode_Com_Stp ==1)										
			{
				FNode_Com_Stp =0;
				FNode_Com_Str =0;
				FNode_AD_Str =1;								//打开AD
				FNode_AD_Stp =0;
				AD_Burp_SCnt =0;								//
				AD_Burp_NSCnt =0;
				Node_Com_Step =0;
				Sys_NodeTask_Step =2;
			}			
		break;
	}
}
/*****************************************************************************
* Function		: Node_AD_Over
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Node_AD_Over(void)
{	
	uint8_t i;
	if(FNode_AD_Stp ==1)										//Com和AD检测完毕
	{
		FNode_AD_Str =0;	FNode_AD_Stp =0;
		Sys_NodeTask_Step =0;
		Sys_Node_Evt_Str =0;
		//=======================================================================
		switch(W_Node_Event)
		{
			case	NODE_IN_EVT:
				if(Node_Infor[W_Node_Num].Node_Status ==NODE_IN_STA)	//这样处理,防止关闭情况下插拔会上报.以及关闭充电
				{
					LED_On_Init(W_Node_Num);														//打开LED	
					if(Node_Infor[W_Node_Num].Node_AD_Status ==BAT_NSHORT)	
					{ //插入设备读取ID正确，并且没有短路
						S_ON_Out(W_Node_Num);															//打开充电
						Node_Close_Tm[W_Node_Num] =TM_OFFREP_DELAY;				//Delay再上报
						M_OFF_Init(W_Node_Num);														//关闭电池铁		
					}
				}
				else
				{
					LED_Off_Init(W_Node_Num);														//关闭LED
					S_OFF_Out(W_Node_Num);															//关闭充电
				}
			break;
			case	NODE_OUT_EVT:		
				S_OFF_Out(W_Node_Num);																//关闭充电
				LED_Off_Init(W_Node_Num);															//关闭LED
				Node_Close_Tm[W_Node_Num] =0;		
				Wt_GsmEvent_Ff((W_Node_Num<<8)|GSM_MQTT_OUT_EVT);			//
			break;
			case NODE_ON_EVT:
				if(M_Status[W_Node_Num] ==M_CLOSE)										//当电池铁是关闭时,才允许打开开始闪烁LED
				{
					M_ON_Init(W_Node_Num);															//打开电池铁
					LED_Flash_Init(W_Node_Num,TM_LED_NOR,TM_LED_OUT);		//打开未取走快闪
				}
			break;
			case NODE_OFF_EVT:
				if(Node_Infor[W_Node_Num].Node_Status ==NODE_IN_STA)	//插入设备读取ID正确
				{
					Wt_GsmEvent_Ff((W_Node_Num<<8)|GSM_MQTT_CLOSE_EVT); //关闭上报
					M_OFF_Init(W_Node_Num);															//关闭电池铁
					S_ON_Out(W_Node_Num);																//打开充电
					LED_On_Init(W_Node_Num);														//打开LED	
				}
				else
				{
					//M_ON_Init(W_Node_Num);															//打开电池铁		
					S_OFF_Out(W_Node_Num);															//关闭充电
					LED_Off_Init(W_Node_Num);														//关闭LED				
				}
			break;
			case NODE_ALL_EVT:
				if(++W_Node_Num>=NODE_TOTAL_NUM)						//查询完所有节点
				{
					Gsm_Node_Rpt_Step =0;
					if(Sys_State ==SYS_NORMAL_STA)
					{
						for(i=0;i<NODE_TOTAL_NUM;i++)						//在读取充电宝时，打开充电。因此在读完所有后需关闭没有的
						{
							if(Node_Infor[i].Node_Status ==NODE_OUT_STA)
							{
								S_OFF_Out(i);												//关闭充电
							}
						}
						Wt_GsmEvent_Ff(GSM_MQTT_TIMER_EVT);			//定时上报事件
					}
				}
				else
				{
					Initial_Node_Handle();										//继续读取
				}
			break;
			case	NODE_ON_REQ_EVT:												//服务器打开请求
				Wt_GsmEvent_Ff((W_Node_Num<<8)|GSM_MQTT_OPENREQ_EVT);
			break;
			case	NODE_ONFAIL_EVT:												//节点打开失败
				Wt_GsmEvent_Ff((W_Node_Num<<8)|GSM_MQTT_ONFAIL_EVT);
			break;
			case	NODE_ONEREP_EVT:												//上报单个节点信息
				Wt_GsmEvent_Ff((W_Node_Num<<8)|GSM_MQTT_ONEREP_EVT);
			break;
			default: break;
		}
	}
}
/*****************************************************************************
* Function		: Initial_Node_Handle
* Description	: 节点事件读取节点信息,以及短路状态
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Initial_Node_Handle(void)
{
	S_ON_Out(W_Node_Num);									//提前打开充电
	Node_Com_Step =0;											//分3次,第一次充电，第二次放电，第3次读
	Node_Com_Tm =TM_S_ONOFF;							//
	Sys_NodeTask_Step =1;									//
	Sys_Node_Evt_Str =1;									//Node Event Start
}
/*****************************************************************************
* Function		: Initial_Node_Out_Handle
* Description	: 节点取走事件,不去读取节点信息
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Initial_Node_Out_Handle(void)
{
	Node_Com_Step =0;											//
	Node_Com_Tm =0;												//
	Sys_NodeTask_Step =2;									//
	Sys_Node_Evt_Str =1;									//Node Event Start
	FNode_AD_Stp =1;											//默认AD以及通信完成
//	Clear_Node_State1(W_Node_Num);				//清节点信息
//	Clear_Node_BatID(W_Node_Num);
	Clear_Node_State(W_Node_Num);
}
/*****************************************************************************
* Function		: Initial_Node_Onreq_Handle
* Description	: 服务器打开请求事件,不去读取节点信息
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Initial_Node_Onreq_Handle(void)
{
	Node_Com_Step =0;											//
	Node_Com_Tm =0;												//
	Sys_NodeTask_Step =2;									//
	Sys_Node_Evt_Str =1;									//Node Event Start
	FNode_AD_Stp =1;											//默认AD以及通信完成
}
/*****************************************************************************
* Function		: Initial_Node_On_Handle
* Description	: 节点打开事件,不去读取节点信息
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Initial_Node_On_Handle(void)
{
	Node_Com_Step =0;											//
	Node_Com_Tm =0;												//
	Sys_NodeTask_Step =2;									//
	Sys_Node_Evt_Str =1;									//Node Event Start
	FNode_AD_Stp =1;											//默认AD以及通信完成
}
/*****************************************************************************
* Function		: Wt_NodeEvent_Ff
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Wt_NodeEvent_Ff(uint16_t	node_event)
{
	NodeEvent_Ff[Wt_NodeEvent_Pt] = node_event;
	if(++Wt_NodeEvent_Pt >=NODE_EVT_FF_LEN) Wt_NodeEvent_Pt=0;
}
/*****************************************************************************
* Function		: Rd_NodeEvent_Ff
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
uint16_t	Rd_NodeEvent_Ff(void)
{
	uint16_t node_event;
	if(Wt_NodeEvent_Pt !=Rd_NodeEvent_Pt)
	{
		node_event =NodeEvent_Ff[Rd_NodeEvent_Pt];
		if(++Rd_NodeEvent_Pt>=NODE_EVT_FF_LEN) Rd_NodeEvent_Pt=0;
		return node_event;
	}
	else
	{
		return NODE_NO_EVT;					//没有事件发生
	}
}
/*****************************************************************************
* Function		: Clear_Node_State
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Clear_Node_State(uint8_t node_Num)
{
	uint8_t i;
	Node_Infor[node_Num].Node_Status =0;
	Node_Infor[node_Num].Node_AD_Status =0;
	for(i=0;i<16;i++)
	{
		Node_Infor[node_Num].Bat_ID[i] ='0';	
	}
	Node_Infor[node_Num].Bat_ID[16] ='\0';	

	Node_Infor[node_Num].Bat_Status[0] ='0';	
	Node_Infor[node_Num].Bat_Status[1] ='\0';	
	
	for(i=0;i<4;i++)
	{
		Node_Infor[node_Num].Bat_Vol[i] ='0';	
	}		
	Node_Infor[node_Num].Bat_Vol[4] ='\0';	
	
	Node_Infor[node_Num].M_State =0;
}	
/*****************************************************************************
* Function		: Clear_Node_State1
* Description	: 在充电宝取走时,除了充电宝ID不清除,其他信息都清除
*							: 充电宝信息在取走上报完成后再清除
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Clear_Node_State1(uint8_t node_Num)
{
	uint8_t i;
	Node_Infor[node_Num].Node_Status =0;
	Node_Infor[node_Num].Node_AD_Status =0;
//	for(i=0;i<16;i++)
//	{
//		Node_Infor[node_Num].Bat_ID[i] ='0';	
//	}
//	Node_Infor[node_Num].Bat_ID[16] ='\0';	

	Node_Infor[node_Num].Bat_Status[0] ='0';	
	Node_Infor[node_Num].Bat_Status[1] ='\0';	
	
	for(i=0;i<4;i++)
	{
		Node_Infor[node_Num].Bat_Vol[i] ='0';	
	}		
	Node_Infor[node_Num].Bat_Vol[4] ='\0';	
	
	Node_Infor[node_Num].M_State =0;
}	
/*****************************************************************************
* Function		: Clear_Node_BatID
* Description	: 清除充电宝ID
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Clear_Node_BatID(uint8_t node_num)
{
	uint8_t i;
	
	for(i=0;i<16;i++)
	{
		Node_Infor[node_num].Bat_ID[i] ='0';	
	}
	Node_Infor[node_num].Bat_ID[16] ='\0';	
}
