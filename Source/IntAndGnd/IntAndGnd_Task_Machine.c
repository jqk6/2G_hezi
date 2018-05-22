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
#include	"SN32F240.h"
#include	"..\UserApp\Sys_Task_Machine.h"
#include	"IntAndGnd_Task_Machine.h"
#include	"..\Node\Node_Task_Machine.h"
#include	"..\Gsm\Gsm_Task_Machine.h"
#include	"..\Node\Node_Task_Key.h"
#include	"..\Node\Node_Task_ad.H"
#include	"..\Node\Node_Task_Ctrl.h"
#include	"..\Node\Node_Task_Com.h"
/*_____ D E F I N I T I O N S ______________________________________________*/
/*_____ D E C L A R A T I O N S ____________________________________________*/

uint8_t	FTm_10ms;
uint8_t	tm_msec;
/*_____ F U N C T I O N S __________________________________________________*/

/*****************************************************************************
* Function		: Systick Initial
* Description	: Initial 10ms interval
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void IntAndGnd_Task_Machine(void)
{
	if((SysTick->CTRL &0x10000)!=0)	//10ms
	{
		SysTick->VAL =0;							//Clear flag
		M_Tm_Task();									//
		Gsm_Tm_Task();								//
		Com_Tm_Task();								//
		LED_Tm_Task();								//
		Time_Task();									//icʱ��
	}
}
/*****************************************************************************
* Function		: Gsm_Tm_Task
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Gsm_Tm_Task(void)			
{
	FTm_10ms =1;
	Gsm_Tx_ChkTmout();												//���gsm����timeout
	//==========================================================================
	if((Gsm_Timer_Tx!=0)&&(FMQTT_NLink==0)&&(FHTTP_Link==0))	//��ʱ���ͼ��
	{
		if(--Gsm_Timer_Tx==0)
		{
			Wt_NodeEvent_Ff(NODE_ALL_EVT);					//��ʱ�����¼�
			Gsm_Timer_Tx =GSM_TIMER_TX_TIME;				//��ʼ����ʱ����ʱ��
		}
	}
	if((Gsm_MQTT_Tmlink!=0)&&(FMQTT_NLink==0)&&(FHTTP_Link==0))	//��ʱ���MQTT����״̬
	{
		if(--Gsm_MQTT_Tmlink==0)
		{
			Wt_GsmEvent_Ff(GSM_MQTT_STATUS_EVT);		
			if(Gsm_Ver_ID ==GSM_VERSION_ID00)					//��ʼ����ʱ���״̬ʱ��
			{
				Gsm_MQTT_Tmlink =GSM_TM_MQTT_STATUS00;		
			}
			else if(Gsm_Ver_ID ==GSM_VERSION_ID05)
			{
				Gsm_MQTT_Tmlink =GSM_TM_MQTT_STATUS05;					
			}
		}	
	}
	//==========================================================================
	if(Http_Req_Tmout !=0)							
	{
		if(--Http_Req_Tmout ==0)							
		{
			if(++Http_Req_Cnt_Tmp >=Http_Req_Cnt)
			{
				Http_Req_Tmout =0;
				Http_Req_Cnt =0;
				Http_Req_Cnt_Tmp =0;
				Sys_GsmTask_Step =0;							//����
				switch(Gsm_Event &0xff)
				{
					case	GSM_HTTP_ACTIVE_EVT:			//�����
						if(Sys_Power_On ==0)					//�ϵ缤��
						{
							Wt_GsmEvent_Ff(GSM_CLOSE_EVT);	//����ر�gsm		
						}
					break;
					case	GSM_HTTP_OPENREQ_EVT:
						Wt_NodeEvent_Ff((Gsm_Event&0x0700)|NODE_ONFAIL_EVT);				//�ڵ��ʧ��
						LED_Flash_Init(((Gsm_Event>>8)&0x07),TM_ONFAIL_FLASH,CNT_ONFIAL_FLASH);//��λ��ʧ����˸10��
					break;
					default: break;
				}
			}
			else
			{
				Sys_GsmTask_Step =1;				//����
			}
		}
	}
	//==========================================================================
	if(Http_Update_Tmout!=0)
	{
		if(--Http_Update_Tmout==0)
		{
			Sys_GsmTask_Step =0;				//����
			Clear_Gsm_Ff();	
			Wt_GsmEvent_Ff(GSM_CLOSE_EVT);		//����ر�gsm		
//			Http_Update_Status =HTTP_UPDATE_STATUS_FAIL;
//			Wt_GsmEvent_Ff(GSM_HTTP_UPDATE_STATUS_EVT);			//����ʧ���ϱ�
		}
	}
}
/*****************************************************************************
* Function		: Com_Tm_Task
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Com_Tm_Task(void)
{ 
	uint8_t i;
	if(Node_Burp_Tm !=0)	Node_Burp_Tm--; 
	if(Node_Com_Tm !=0)		Node_Com_Tm--;
	for(i=0;i<NODE_TOTAL_NUM;i++)
	{
		if(Node_Close_Tm[i]!=0)  
		{
			if(--Node_Close_Tm[i]==0)
			{
				Wt_NodeEvent_Ff((i<<8)|NODE_OFF_EVT);		//����һ��ʱ����͹ر�
			}
		}
	}
}
/*****************************************************************************
* Function		: M_Tm_Task
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	M_Tm_Task(void)
{
	uint8_t i;
	switch(Sys_State)
	{
		case SYS_PWRON_STA:	
			//break;
		case SYS_UPDATE_STA:
			//break;
		case SYS_NORMAL_STA:
		for(i=0;i<NODE_TOTAL_NUM;i++)
		{
			if(M_ON_Init_Tm[i] !=0)							//20S��
			{
				M_ON_Init_Tm[i]--;
				if(M_ON_Tm[i] !=0)								//����ʱ��ݼ�
				{
					if(--M_ON_Tm[i] ==0)
					{
						M_ON_PWM(i);									//��PWM
						M_PWM_Tm[i] =200;							//
					}
				}
				if(M_PWM_Tm[i] !=0)								//PWMʱ��ݼ�
				{
					if(--M_PWM_Tm[i] ==0)
					{
						M_OFF_PWM(i);									//�ر�PWM����
						M_ON_Out(i);									//
						M_ON_Tm[i] =200;							//
					}
				}				
			}
			else
			{
				if(M_ON_Tm[i] !=0)								//����ʱ��ݼ�
				{
					if(--M_ON_Tm[i] ==0)
					{
						M_ON_PWM(i);									//��PWM
						M_PWM_Tm[i] =6000;						//
					}
				}
				if(M_PWM_Tm[i] !=0)								//PWMʱ��ݼ�
				{
					if(--M_PWM_Tm[i] ==0)
					{
						M_OFF_PWM(i);									//�ر�PWM����
						M_ON_Out(i);									//
						M_ON_Tm[i] =200;							//
					}
				}
			}
		}		
		break;
		default: break;
	}
}
/*****************************************************************************
* Function		: LED_Tm_Task
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	LED_Tm_Task(void)
{
	uint8_t i;
	switch(Sys_State)
	{
		case SYS_PWRON_STA:																	//�ϵ�״̬
		if(--LED_Toggle_Tm[NODE1] ==0)
		{
			if((FGsm_Init_OK ==0)&&(FGsm_MQTT_Init_OK ==0))		//û�����ϻ�վ����
			{
				LED_Toggle_Tm[NODE1] =TM_LED_FLASH_NONET;				
			}
			else if((FGsm_Init_OK ==1)&&(FGsm_MQTT_Init_OK ==0))//���ϻ�վ,û��������
			{
				LED_Toggle_Tm[NODE1] =TM_LED_FLASH_NOMQTT;			
			}
			else																							//��վ���ƶ�������,������������,��վû�����������
			{
				LED_Toggle_Tm[NODE1] =TM_LED_FLASH_NETMQTT;	
			}
			LED_Status =LED_Status^0xff;
		}	
		break;
		case SYS_NORMAL_STA:																//����ģʽ������ʱ��˸��ʾ,���һֱδ���ߣ��ر�
			for(i=0;i<NODE_TOTAL_NUM;i++)
			{
				if(LED_Toggle_Tm[i]!=0)
				{
					if(--LED_Toggle_Tm[i]==0)
					{
						LED_Toggle_Tm[i] =LED_Toggle_Tmtmp[i];
						LED_Toggle_Out(i);							//
						if(LED_Toggle_Cnt[i]!=0)
						{
							if(--LED_Toggle_Cnt[i] ==0)
							{
								LED_On_Init(i);													//��LED
								Wt_NodeEvent_Ff((i<<8)|NODE_OFF_EVT);		//�ڵ�ر�
								M_OFF_Init(i);													//�رյ����
							}
						}
					}
				}
			}
		break;
		case SYS_UPDATE_STA:																//Զ������������˸ָʾ
			if(--LED_Toggle_Tm[NODE1] ==0)
			{
				LED_Toggle_Tm[NODE1] =TM_LED_RUN;
				switch(LED_Update_Step)
				{
					case 0:
						if(Http_Update_StrAddr>=(8192*1)) {LED_Status =0x03;	LED_Update_Step =1;}
						else {LED_Status ^=0x01;}
					break;
					case 1:
						if(Http_Update_StrAddr>=(8192*2)) {LED_Status =0x07;	LED_Update_Step =2;}
						else {LED_Status |=0x01;	LED_Status ^=0x02;}				
					break;
					case 2:
						if(Http_Update_StrAddr>=(8192*3)) {LED_Status =0x0f;	LED_Update_Step =3;}
						else {LED_Status |=0x03;	LED_Status ^=0x04;}				
					break;						
					case 3:
						if(Http_Update_StrAddr>=(8192*4)) {LED_Status =0x1f;	LED_Update_Step =4;}
						else {LED_Status |=0x07;	LED_Status ^=0x08;}				
					break;
					case 4:
						if(Http_Update_StrAddr>=(8192*5)) {LED_Status =0x3f;	LED_Update_Step =5;}
						else {LED_Status |=0x0f;	LED_Status ^=0x10;}				
					break;						
					case 5:
						if(Http_Update_StrAddr>=(8192*6)) {LED_Status =0x7f;	LED_Update_Step =6;}
						else {LED_Status |=0x1f;	LED_Status ^=0x20;}				
					break;							
					case 6:
						if(Http_Update_StrAddr>=(8192*7)) {LED_Status =0xff;	LED_Update_Step =7;}
						else {LED_Status |=0x3f;	LED_Status ^=0x40;}				
					break;							
					case 7:
						if(Http_Update_StrAddr>=(8192*8)) {LED_Status =0x00;	LED_Update_Step =8;}
						else {LED_Status |=0x7f;	LED_Status ^=0x80;}				
					break;							
					default: break;
				}
			}			
		break;
		case	SYS_NONET_STA:																	//����״̬
			if(--LED_Toggle_Tm[NODE1] ==0)
			{
				if((FGsm_Init_OK ==0)&&(FGsm_MQTT_Init_OK ==0))		//û�����ϻ�վ����
				{
					LED_Toggle_Tm[NODE1] =TM_LED_FLASH_NONET;				
				}
				else if((FGsm_Init_OK ==1)&&(FGsm_MQTT_Init_OK ==0))//���ϻ�վ,û��������
				{
					LED_Toggle_Tm[NODE1] =TM_LED_FLASH_NOMQTT;			
				}
				else																							//��վ���ƶ�������,������������,��վû�����������
				{
					LED_Toggle_Tm[NODE1] =TM_LED_FLASH_NETMQTT;	
				}
				LED_Status =LED_Status^0xff;
			}			
		break;
		default:	break;
	}
}
/*****************************************************************************
* Function		: Time_Task
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Time_Task(void)
{
	if(++tm_msec>=100)						//1s?
	{
		tm_msec =0;
		if(++IC_sec>=60)						//1min?
		{
			IC_sec =0;
			if(++IC_min>=60)					//1hour?
			{
				IC_min =0;
				if(++IC_hour>=24)				//1day
				{
					IC_hour =0;
					IC_day++;
					if((IC_mon==1)||(IC_mon==3)||(IC_mon==5)||(IC_mon==7)||(IC_mon==8)||(IC_mon==10)||(IC_mon==12))
					{
						if(IC_day>=31)
						{
							IC_day=0;	
							if(++IC_mon>=12) {IC_mon =0;IC_year++;}
						}
					}
					else if((IC_mon==4)||(IC_mon==6)||(IC_mon==9)||(IC_mon==11))
					{
						if(IC_day>=30)
						{
							IC_day=0;	
							if(++IC_mon>=12) {IC_mon =0;IC_year++;}
						}					
					}
					else if(IC_mon==2)
					{
						if((IC_year%4==0&&IC_year%100!=0)||(IC_year%400==0))			//����
						{
							if(IC_day>=29)
							{
								IC_day=0;	
								if(++IC_mon>=12) {IC_mon =0;IC_year++;}
							}						
						}
						else
						{
							if(IC_day>=28)
							{
								IC_day=0;	
								if(++IC_mon>=12) {IC_mon =0;IC_year++;}
							}							
						}
					}
				}
			}
		}
	}
}