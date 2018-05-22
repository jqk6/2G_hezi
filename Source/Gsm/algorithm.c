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
#include 	<SN32F240.h>
#include	"algorithm.h"
#include <stdio.h>
#include <stdlib.h>
#include	"..\EDCode\cJSON.h"
#include	"..\EDCode\aes.h"
#include	"..\EDCode\md5.h"
#include	"..\EDCode\base64.h"
#include	"Gsm_Task_Machine.h"
#include	"..\UserApp\Sys_Task_Machine.h"
#include	"..\Node\Node_Task_Machine.h"
#include	"..\Driver\Uart\Uart.h"
#include	"..\Node\Node_Task_Ctrl.h"
/*****************************************************************************
* Function		: Gsm_PowerOn_Report_Init
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Gsm_MQTT_Report_Payload_Init(uint8_t	mqtt_report_mode)
{
	cJSON *root,*root1,*root2,*root3,*fld;
	uint32_t i,len;
	char *out;
	unsigned char	md5[32];
	char 	aes[500];
	uint8_t	node_m_status;

	if(mqtt_report_mode ==MQTT_REPORT_MODE_PUBLINK)
	{
		Gsm_Pack_data[0]='l';
		Gsm_Pack_data[1]='\0';
		return;
	}
	root=cJSON_CreateObject();
//	cJSON_AddNumberToObject(root,"protocol",4);	
	cJSON_AddNumberToObject(root,"t",Timestamp);	
	cJSON_AddItemToObject(root, "data",root1=cJSON_CreateObject());	
//	cJSON_AddStringToObject(root1,"devId",Device_ID);
	cJSON_AddItemToObject(root1, "dps",root2=cJSON_CreateObject());	
	cJSON_AddNumberToObject(root2,"140",CSQ_Buf);									//信号强度
	switch(mqtt_report_mode)
	{
		case MQTT_REPORT_MODE_TIMER:		//定时上报
			cJSON_AddStringToObject(root2,"121",Rom_Version);							//Rom Version
			cJSON_AddStringToObject(root2,"107","state");									//上报状态
			cJSON_AddItemToObject(root2, "detail",root3=cJSON_CreateArray());
			for(i=0;i<2;i++)
			{
				cJSON_AddItemToArray(root3,fld=cJSON_CreateObject());
				cJSON_AddNumberToObject(fld,"126",Gsm_Node_Rpt_Step*2+i);													//仓位号
				node_m_status = Update_MQTT_Node_Report_Status(Gsm_Node_Rpt_Step*2+i,MQTT_REPORT_MODE_TIMER);//				
				cJSON_AddNumberToObject(fld,"127",node_m_status);																	//仓位状态
				cJSON_AddStringToObject(fld,"125",Node_Infor[Gsm_Node_Rpt_Step*2+i].Bat_ID);			//充电宝ID
				cJSON_AddStringToObject(fld,"124",Node_Infor[Gsm_Node_Rpt_Step*2+i].Bat_Status);	//充电宝状态	
				cJSON_AddStringToObject(fld,"101",Node_Infor[Gsm_Node_Rpt_Step*2+i].Bat_Vol);			//充电宝电压
			}						
		break;
		case MQTT_REPORT_MODE_PWRON:		//开机上报
		cJSON_AddStringToObject(root2,"121",Rom_Version);							//Rom Version
		cJSON_AddStringToObject(root2,"107","event");									//上报事件
		cJSON_AddNumberToObject(root2,"106",7);	
		cJSON_AddItemToObject(root2, "detail",root3=cJSON_CreateArray());
		for(i=0;i<2;i++)
		{
			cJSON_AddItemToArray(root3,fld=cJSON_CreateObject());
			cJSON_AddNumberToObject(fld,"126",Gsm_Node_Rpt_Step*2+i);														//仓位号
			node_m_status = Update_MQTT_Node_Report_Status(Gsm_Node_Rpt_Step*2+i,MQTT_REPORT_MODE_PWRON);	//			
			cJSON_AddNumberToObject(fld,"127",node_m_status);																		//仓位状态
			cJSON_AddStringToObject(fld,"125",Node_Infor[Gsm_Node_Rpt_Step*2+i].Bat_ID);				//充电宝ID
			cJSON_AddStringToObject(fld,"124",Node_Infor[Gsm_Node_Rpt_Step*2+i].Bat_Status);		//充电宝状态	
			cJSON_AddStringToObject(fld,"101",Node_Infor[Gsm_Node_Rpt_Step*2+i].Bat_Vol);				//充电宝电压
		}			
		break;
		case MQTT_REPORT_MODE_RLINK:			//断网重连上报
			cJSON_AddNumberToObject(root2,"tl",Timestamp_NLink);						//断网时间
			cJSON_AddStringToObject(root2,"107","event");										//上报事件
			cJSON_AddNumberToObject(root2,"106",8);	
			cJSON_AddItemToObject(root2, "detail",root3=cJSON_CreateArray());
			for(i=0;i<2;i++)
			{
				cJSON_AddItemToArray(root3,fld=cJSON_CreateObject());
				cJSON_AddNumberToObject(fld,"126",Gsm_Node_Rpt_Step*2+i);															//仓位号
				node_m_status = Update_MQTT_Node_Report_Status(Gsm_Node_Rpt_Step*2+i,MQTT_REPORT_MODE_RLINK);	//	
				cJSON_AddNumberToObject(fld,"127",node_m_status);																		//仓位状态
				cJSON_AddStringToObject(fld,"125",Node_Infor[Gsm_Node_Rpt_Step*2+i].Bat_ID);				//充电宝ID
				cJSON_AddStringToObject(fld,"124",Node_Infor[Gsm_Node_Rpt_Step*2+i].Bat_Status);		//充电宝状态	
				cJSON_AddStringToObject(fld,"101",Node_Infor[Gsm_Node_Rpt_Step*2+i].Bat_Vol);				//充电宝电压
			}				
		break;
		case MQTT_REPORT_MODE_OPENREQ:			//仓位打开上报申请
			cJSON_AddStringToObject(root2,"107","event");										//上报事件
			cJSON_AddNumberToObject(root2,"106",9);		
			cJSON_AddItemToObject(root2, "detail",root3=cJSON_CreateArray());
			cJSON_AddItemToArray(root3,fld=cJSON_CreateObject());
			cJSON_AddNumberToObject(fld,"126",Gsm_Rept_Node);																//仓位号
			node_m_status = Update_MQTT_Node_Report_Status(Gsm_Rept_Node,MQTT_REPORT_MODE_NODEOUT);	//
			cJSON_AddNumberToObject(fld,"127",node_m_status);														//仓位状态
			cJSON_AddStringToObject(fld,"125",Node_Infor[Gsm_Rept_Node].Bat_ID);				//充电宝ID
			cJSON_AddStringToObject(fld,"124",Node_Infor[Gsm_Rept_Node].Bat_Status);		//充电宝状态	
			cJSON_AddStringToObject(fld,"101",Node_Infor[Gsm_Rept_Node].Bat_Vol);			//充电宝电压
		break;
		case MQTT_REPORT_MODE_NODEOUT:			//充电宝取走上报
			cJSON_AddStringToObject(root2,"107","event");									//上报事件
			cJSON_AddNumberToObject(root2,"106",10);		
			cJSON_AddItemToObject(root2, "detail",root3=cJSON_CreateArray());		
			cJSON_AddItemToArray(root3,fld=cJSON_CreateObject());
			cJSON_AddNumberToObject(fld,"126",Gsm_Rept_Node);																//仓位号
			node_m_status = Update_MQTT_Node_Report_Status(Gsm_Rept_Node,MQTT_REPORT_MODE_NODEOUT);	//
			cJSON_AddNumberToObject(fld,"127",node_m_status);																//仓位状态
			cJSON_AddStringToObject(fld,"125",Node_Infor[Gsm_Rept_Node].Bat_ID);						//充电宝ID
			cJSON_AddStringToObject(fld,"124",Node_Infor[Gsm_Rept_Node].Bat_Status);				//充电宝状态	
			cJSON_AddStringToObject(fld,"101",Node_Infor[Gsm_Rept_Node].Bat_Vol);						//充电宝电压
//			Clear_Node_BatID(Gsm_Rept_Node);
		break;
		case MQTT_REPORT_MODE_NODECLOSE:			//仓位关闭上报
			cJSON_AddStringToObject(root2,"107","event");									//上报事件
			cJSON_AddNumberToObject(root2,"106",12);		
			cJSON_AddItemToObject(root2, "detail",root3=cJSON_CreateArray());		
			cJSON_AddItemToArray(root3,fld=cJSON_CreateObject());
			cJSON_AddNumberToObject(fld,"126",Gsm_Rept_Node);																//仓位号
			node_m_status = Update_MQTT_Node_Report_Status(Gsm_Rept_Node,MQTT_REPORT_MODE_NODECLOSE);//
			cJSON_AddNumberToObject(fld,"127",node_m_status);														//仓位状态
			cJSON_AddStringToObject(fld,"125",Node_Infor[Gsm_Rept_Node].Bat_ID);				//充电宝ID
			cJSON_AddStringToObject(fld,"124",Node_Infor[Gsm_Rept_Node].Bat_Status);		//充电宝状态	
			cJSON_AddStringToObject(fld,"101",Node_Infor[Gsm_Rept_Node].Bat_Vol);			//充电宝电压		
		break;
		case	MQTT_REPORT_MODE_OPENFAIL:			//仓位打开失败上报
			cJSON_AddStringToObject(root2,"107","event");									//上报事件
			cJSON_AddNumberToObject(root2,"106",18);		
			cJSON_AddItemToObject(root2, "detail",root3=cJSON_CreateArray());		
			cJSON_AddItemToArray(root3,fld=cJSON_CreateObject());
			cJSON_AddNumberToObject(fld,"126",Gsm_Rept_Node);																//仓位号
			node_m_status = Update_MQTT_Node_Report_Status(Gsm_Rept_Node,MQTT_REPORT_MODE_NODECLOSE);//
			cJSON_AddNumberToObject(fld,"127",node_m_status);														//仓位状态
			cJSON_AddStringToObject(fld,"125",Node_Infor[Gsm_Rept_Node].Bat_ID);				//充电宝ID
			cJSON_AddStringToObject(fld,"124",Node_Infor[Gsm_Rept_Node].Bat_Status);		//充电宝状态	
			cJSON_AddStringToObject(fld,"101",Node_Infor[Gsm_Rept_Node].Bat_Vol);			//充电宝电压					
		break;
		case	MQTT_REPORT_MODE_ONEREP:				//单个仓位信息上报
			cJSON_AddStringToObject(root2,"121",Rom_Version);							//Rom Version
			cJSON_AddStringToObject(root2,"107","state");									//上报状态
			cJSON_AddItemToObject(root2, "detail",root3=cJSON_CreateArray());
			cJSON_AddItemToArray(root3,fld=cJSON_CreateObject());
			cJSON_AddNumberToObject(fld,"126",Gsm_Rept_Node);																//仓位号
			node_m_status = Update_MQTT_Node_Report_Status(Gsm_Rept_Node,MQTT_REPORT_MODE_NODECLOSE);//
			cJSON_AddNumberToObject(fld,"127",node_m_status);														//仓位状态
			cJSON_AddStringToObject(fld,"125",Node_Infor[Gsm_Rept_Node].Bat_ID);				//充电宝ID
			cJSON_AddStringToObject(fld,"124",Node_Infor[Gsm_Rept_Node].Bat_Status);		//充电宝状态	
			cJSON_AddStringToObject(fld,"101",Node_Infor[Gsm_Rept_Node].Bat_Vol);			//充电宝电压	
		break;
		case	MQTT_REPORT_MODE_PUBLINK:
			
		break;
		default:	break;
	}
	out=cJSON_PrintUnformatted(root);	
	
#if UART_DEBUG ==1											//print
	Uart_Tx_Print(strlen(out),out);	
#endif
	
	cJSON_Delete(root);	
	
	len =aes128_ecb_encrypt((uint8_t*)out,(uint8_t*)SecKey,(uint8_t*)aes);
	base64_encode((unsigned char*)aes,len,out);
	len =19;															//填充数据
	for(i=0;i<strlen(out);i++)
	{
		Gsm_Pack_data[len+i] =*(out+i);
	}
	Gsm_Pack_data[len+i] ='\0';
	//======================================================
	//签名加密数据
	aes[0] ='d'; aes[1] ='a'; aes[2] ='t'; aes[3] ='a'; aes[4] ='=';	
	len=5;
	for(i=0;i<strlen(out);i++)
	{
		aes[len+i] =*(out+i);
	}
	len =len +i;
	aes[len++] ='|'; 	aes[len++] ='|'; 
	aes[len++] ='p';	aes[len++] ='v';	aes[len++] ='=';	aes[len++] =B1_PROTOCOL_VER_0;	aes[len++] =B1_PROTOCOL_VER_1;	aes[len++] =B1_PROTOCOL_VER_2;	
	aes[len++] ='|';	aes[len++] ='|';	
	for(i=0;i<16;i++)
	{
		aes[len++] =SecKey[i];	
	}
	aes[len] ='\0';
	free(out);	

	//======================================================
	get_md5_32_str((unsigned char*)aes,strlen(aes),md5);	
	
	Gsm_Pack_data[0] =B1_PROTOCOL_VER_0;
	Gsm_Pack_data[1] =B1_PROTOCOL_VER_1;
	Gsm_Pack_data[2] =B1_PROTOCOL_VER_2;
	len =3;
	for(i=8;i<24;i++)
	{
		Gsm_Pack_data[len++] =md5[i];
	}	
}
/*****************************************************************************
* Function		: Gsm_HTTP_Report_Payload_Init
* Description	: 计算HTTP sign
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Gsm_HTTP_Report_Payload_Init(void)
{
	unsigned char s[300];
	uint32_t i,j,k,len;
	cJSON *root;char *out;
	char 	aes[300];
	s[0] ='a';	s[1] ='=';
	j=2;
	switch(Gsm_Event &0xff)
	{
		case GSM_HTTP_ACTIVE_EVT:									//HTTP 激活请求
			for(i=0;i<strlen(HTTP_ACTIVE_Tab);i++)
			{
				s[j++] =HTTP_ACTIVE_Tab[i];
			}
		break;
		case GSM_HTTP_OPENREQ_EVT:								//HTTP打开仓位请求
			for(i=0;i<strlen(HTTP_OPEN_Tab);i++)		
			{																				
				s[j++] =HTTP_OPEN_Tab[i];								
			}																				
		break;
		case	GSM_HTTP_UPDATE_STATUS_EVT:				//HTTP更新状态上报
			for(i=0;i<strlen(HTTP_UPDATE_STATUS_Tab);i++)		
			{																				
				s[j++] =HTTP_UPDATE_STATUS_Tab[i];								
			}					
		break;
		default: break;
	}
	s[j++] ='|';	s[j++] ='|';
	s[j++] ='d';	s[j++] ='a';	s[j++] ='t';	s[j++] ='a';	s[j++] ='=';
	switch(Gsm_Event &0xff)
	{
		case GSM_HTTP_ACTIVE_EVT:									//HTTP 激活请求
			Gsm_Pack_data[0] ='\0';
		break;
		case GSM_HTTP_OPENREQ_EVT:								//HTTP打开仓位请求
			root=cJSON_CreateObject();
			cJSON_AddNumberToObject(root,"slotId",Gsm_Rept_Node);																//仓位号
			cJSON_AddNumberToObject(root,"slotStatus",Node_Infor[Gsm_Rept_Node].Node_Status);	//仓位状态
			cJSON_AddStringToObject(root,"batteryBankId",Node_Infor[Gsm_Rept_Node].Bat_ID);				//充电宝ID																			
			out=cJSON_PrintUnformatted(root);
#if UART_DEBUG ==1											//print
	Uart_Tx_Print(strlen(out),out);	
#endif		
			len =aes128_ecb_encrypt((uint8_t*)out,(uint8_t*)SecKey,(uint8_t*)aes);
			cJSON_Delete(root);	
			free(out);
			k=0;
			for(i=0;i<len;i++)
			{
				Gsm_Pack_data[k++] =s[j++] =InttoAscii(aes[i]>>4); 
				Gsm_Pack_data[k++] =s[j++] =InttoAscii(aes[i]&0x0f);
			}
			Gsm_Pack_data[k] ='\0';
		break;
		case	GSM_HTTP_UPDATE_STATUS_EVT:
			root=cJSON_CreateObject();
			cJSON_AddNumberToObject(root,"upgradeStatus",Http_Update_Status);								//
			out=cJSON_PrintUnformatted(root);
#if UART_DEBUG ==1											//print
	Uart_Tx_Print(strlen(out),out);	
#endif		
			len =aes128_ecb_encrypt((uint8_t*)out,(uint8_t*)SecKey,(uint8_t*)aes);
			cJSON_Delete(root);	
			free(out);
			k=0;
			for(i=0;i<len;i++)
			{
				Gsm_Pack_data[k++] =s[j++] =InttoAscii(aes[i]>>4); 
				Gsm_Pack_data[k++] =s[j++] =InttoAscii(aes[i]&0x0f);
			}
			Gsm_Pack_data[k] ='\0';
		break;
		default: break;
	}
	s[j++] ='|';	s[j++] ='|';	
	s[j++] ='d';	s[j++] ='e';	s[j++] ='v';	s[j++] ='I';	s[j++] ='d';	s[j++] ='=';
	for(i=0;i<22;i++)
	{
		s[j++] =Device_ID[i];
	}
	s[j++] ='|';	s[j++] ='|';	
	s[j++] ='i';	s[j++] ='c';	s[j++] ='c';	s[j++] ='I';	s[j++] ='d';	s[j++] ='=';
	for(i=0;i<20;i++)
	{
		s[j++] =ICCID_ID[i];
	}
	s[j++] ='|';	s[j++] ='|';	
	s[j++] ='t';	s[j++] ='=';	
	s[j++] =Timestamp /1000000000+0x30;
	s[j++] =(Timestamp%1000000000)/100000000+0x30;
	s[j++] =(Timestamp%1000000000%100000000)/10000000+0x30;
	s[j++] =(Timestamp%1000000000%100000000%10000000)/1000000+0x30;
	s[j++] =(Timestamp%1000000000%100000000%10000000%1000000)/100000+0x30;
	s[j++] =(Timestamp%1000000000%100000000%10000000%1000000%100000)/10000+0x30;
	s[j++] =(Timestamp%1000000000%100000000%10000000%1000000%100000%10000)/1000+0x30;
	s[j++] =(Timestamp%1000000000%100000000%10000000%1000000%100000%10000%1000)/100+0x30;
	s[j++] =(Timestamp%1000000000%100000000%10000000%1000000%100000%10000%1000%100)/10+0x30;
	s[j++] =(Timestamp%1000000000%100000000%10000000%1000000%100000%10000%1000%100%10)/1+0x30;
		
	s[j++] ='|';	s[j++] ='|';	
	s[j++] ='v';	s[j++] ='=';	s[j++] =B1_HTTP_VER_0;	s[j++] =B1_HTTP_VER_1;	s[j++] =B1_HTTP_VER_2;
	s[j++] ='|';	s[j++] ='|';
	switch(Gsm_Event &0xff)
	{
		case	GSM_HTTP_ACTIVE_EVT:
			for(i=0;i<SN_ID_LEN;i++)
			{	
				s[j++] =SN_ID[i];
			}	
		break;
		case	GSM_HTTP_OPENREQ_EVT:
		case	GSM_HTTP_UPDATE_STATUS_EVT:
			for(i=0;i<16;i++)
			{	
				s[j++] =SecKey[i];
			}		
		break;
	}
	s[j] = '\0';
	
	get_md5_32_str(s,strlen((char*)s),(unsigned char*)Sign);	
	
#if UART_DEBUG ==1											//print
	Uart_Tx_Print(strlen((char*)s),(char*)s);	
#endif
}
/*****************************************************************************
* Function		: Gsm_PowerOn_Report_Init
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
#if SEK_DEBUG ==1
	char SecKey1[17];
#endif
void	Decode_MQTT_Server_Data(void)
{
	char	si[600],so[600],s[5];
	uint32_t i,j,k,l;uint16_t data_len;
	uint8_t node_num,node_status,server_request_mode;
	//=====================================================================
	j=0;																																//提取长度
	for(i=41;i<strlen(Gsm_Rx_Dt);i++)
	{
		if(Gsm_Rx_Dt[i]==',')
		{
			break;
		}
		else
		{
			if((Gsm_Rx_Dt[i]>='0')&&(Gsm_Rx_Dt[i]<='9'))
			s[j++]=Gsm_Rx_Dt[i];
		}
	}
	s[j] ='\0';
	data_len =AsciitoInt(s);					//
	//======================================================================
	for(i=41;i<strlen(Gsm_Rx_Dt);i++)
	{																							
		if((Gsm_Rx_Dt[i]=='3')&&(Gsm_Rx_Dt[i+1]=='.')&&(Gsm_Rx_Dt[i+2]=='0'))	//3.0			
		{	
			j =0;	
			for(l=0;l<data_len;l++)																	//提取有效数据
			{
				si[j++] =Gsm_Rx_Dt[i+19+l];
			}
			si[j] ='\0';
			//=====================================================================
			if((si[0]=='{')&&(si[1]=='\"')&&(si[2]=='p')&&(si[3]=='r'))
			{
			
			}
			else
			{
		#if	SEK_DEBUG ==1	
				SecKey1[0]='Q';	SecKey1[1]='a';	SecKey1[2]='P';	SecKey1[3]='w';	SecKey1[4]='Q';	SecKey1[5]='5';	SecKey1[6]='N';	SecKey1[7]='H';
				SecKey1[8]='x';	SecKey1[9]='g';	SecKey1[10]='A';SecKey1[11]='i';	SecKey1[12]='Z';	SecKey1[13]='L';	SecKey1[14]='9';	SecKey1[15]='K';	
				SecKey1[16] ='\0';
				j=base64_decode(si,(unsigned char*)so);
				aes128_ecb_decrypt((uint8_t *)so,j,(uint8_t *)SecKey1,(uint8_t *)si);
		#else		
				j=base64_decode(si,(unsigned char*)so);
				aes128_ecb_decrypt((uint8_t *)so,j,(uint8_t *)SecKey,(uint8_t *)si);
		#endif
			}
	#if UART_DEBUG ==1											//print
		Uart_Tx_Print(strlen((char*)si),(char*)si);	
	#endif
			//======================================================================
			for(i=0;i<strlen(si);i++)					//查找服务器请求种类
			{
				if((si[i]=='\"')&&(si[i+1]=='1')&&(si[i+2]=='2')&&(si[i+3]=='6')&&(si[i+4]=='\"'))	//打开关闭仓位
				{
					server_request_mode =1;	break;
				}
				if((si[i]=='\"')&&(si[i+1]=='1')&&(si[i+2]=='1')&&(si[i+3]=='5')&&(si[i+4]=='\"'))	//通知设备上报或激活
				{
					server_request_mode =2;	break;
				}
				if((si[i]=='\"')&&(si[i+1]=='p')&&(si[i+2]=='r')&&(si[i+3]=='o'))										//通知设备更新
				{
					if((si[i+11]=='1')&&(si[i+12]=='5'))	
					{
						server_request_mode =3;	break;
					}				
				}
			}
			//======================================================================
			switch(server_request_mode)
			{
				case 1:			//仓位打开关闭
					for(i=0;i<strlen(si);i++)
					{
						if((si[i]=='\"')&&(si[i+1]=='1')&&(si[i+2]=='\"'))											//status
						{
							if((si[i+4]=='t')&&(si[i+5]=='r')&&(si[i+6]=='u')&&(si[i+7]=='e'))
							{
								node_status =NODE_ON_REQ_EVT;		//open
							}
							else if((si[i+4]=='f')&&(si[i+5]=='a')&&(si[i+6]=='l')&&(si[i+7]=='s'))
							{
								node_status =NODE_OFF_EVT;		//close
							}
						}
						if((si[i]=='\"')&&(si[i+1]=='1')&&(si[i+2]=='2')&&(si[i+3]=='6')&&(si[i+4]=='\"'))	//node
						{
							switch(si[i+6])
							{
								case '0': node_num=0;	break;
								case '1': node_num=1;	break;
								case '2': node_num=2;	break;
								case '3': node_num=3;	break;
								case '4': node_num=4;	break;
								case '5': node_num=5;	break;
								case '6': node_num=6;	break;
								case '7': node_num=7;	break;
								default: break;
							}
						}
					}
					Wt_NodeEvent_Ff((node_num<<8)|node_status);					//节点打开或关闭
				break;
				case 2:		//通知设备上报或激活
					for(i=0;i<strlen(si);i++)
					{
						if((si[i]=='\"')&&(si[i+1]=='1')&&(si[i+2]=='1')&&(si[i+3]=='5')&&(si[i+4]=='\"'))	
						{
							if(si[i+6]=='1')				//通知设备上报
							{
								Wt_NodeEvent_Ff(NODE_ALL_EVT);						//
								break;
							}
							else if(si[i+6]=='2')		//激活
							{
								Wt_GsmEvent_Ff(GSM_HTTP_ACTIVE_EVT);							 //
								break;
							}
							else if(si[i+6]=='4')				//通知设备上报单个仓位信息
							{
								for(j=0;j<strlen(si);j++)
								{
									if((si[j]=='\"')&&(si[j+1]=='1')&&(si[j+2]=='2')&&(si[j+3]=='6')&&(si[j+4]=='\"'))	
									{
										switch(si[j+6])
										{
											case '0': node_num=0;	break;
											case '1': node_num=1;	break;
											case '2': node_num=2;	break;
											case '3': node_num=3;	break;
											case '4': node_num=4;	break;
											case '5': node_num=5;	break;
											case '6': node_num=6;	break;
											case '7': node_num=7;	break;
											default: break;
										}
										Wt_NodeEvent_Ff((node_num<<8)|NODE_ONEREP_EVT);			//单个节点上报
										break;
									}
								}
								break;
							}
							else if(si[i+6]=='5')				//仓位打开确认
							{
								for(j=0;j<strlen(si);j++)
								{
									if((si[j]=='\"')&&(si[j+1]=='1')&&(si[j+2]=='\"'))											//status
									{
										if((si[j+4]=='t')&&(si[j+5]=='r')&&(si[j+6]=='u')&&(si[j+7]=='e'))
										{
											node_status =NODE_ON_EVT;		//open
										}
										else if((si[j+4]=='f')&&(si[j+5]=='a')&&(si[j+6]=='l')&&(si[j+7]=='s'))
										{
											node_status =NODE_OFF_EVT;		//close
										}
									}
									if((si[j]=='\"')&&(si[j+1]=='1')&&(si[j+2]=='2')&&(si[j+3]=='6')&&(si[j+4]=='\"'))	
									{
										switch(si[j+6])
										{
											case '0': node_num=0;	break;
											case '1': node_num=1;	break;
											case '2': node_num=2;	break;
											case '3': node_num=3;	break;
											case '4': node_num=4;	break;
											case '5': node_num=5;	break;
											case '6': node_num=6;	break;
											case '7': node_num=7;	break;
											default: break;
										}
									}
								}
								if(node_status ==NODE_ON_EVT)	{Wt_NodeEvent_Ff((node_num<<8)|NODE_ON_EVT);}			//打开仓位
								break;							
							}
						}					
					}					
				break;
				case 3:		//通知更新
					for(i=0;i<strlen(si);i++)
					{
						if((si[i]=='\"')&&(si[i+1]=='u')&&(si[i+2]=='r')&&(si[i+3]=='l')&&(si[i+4]=='\"'))	
						{
							j =0;	i=i+7;
							while(si[i]!='\"')
							{
								so[j++]=si[i++];
							}
							so[j] ='\0';
							
							if((so[j-15] =='2')&&(so[j-14] =='G'))							//是4G 代码
							{
								//check sum
								CheckSum_Buf1 =0;
								CheckSum_Buf2 =0;
								CheckSum = (ChartoInt(so[j-8])<<12)|(ChartoInt(so[j-7])<<8)|(ChartoInt(so[j-6])<<4)|(ChartoInt(so[j-5]));
								Http_Update_Status = HTTP_UPDATE_STATUS_UPDATING;
								Wt_GsmEvent_Ff(GSM_HTTP_UPDATE_STATUS_EVT);				//上报更新进行状态
							
								k =0;
								if((so[0]=='h')&&(so[1]=='t')&&(so[2]=='t')&&(so[3]=='p')&&(so[4]==':'))
								{
									for(j=7;j<strlen(so);j++)				//http url 
									{
										Http_Update_Web[k++] =so[j];
									}						
								}
								else if((so[0]=='h')&&(so[1]=='t')&&(so[2]=='t')&&(so[3]=='p')&&(so[4]=='s')&&(so[5]==':'))
								{
									for(j=8;j<strlen(so);j++)				//http url 
									{
										Http_Update_Web[k++] =so[j];
									}						
								}
								else
								{
									for(j=0;j<strlen(so);j++)				//http url 
									{
										Http_Update_Web[k++] =so[j];
									}									
								}
								Http_Update_Web[k] ='\0';	
							}
							else
							{
								Http_Update_Status = HTTP_UPDATE_STATUS_FAIL;
								Wt_GsmEvent_Ff(GSM_HTTP_UPDATE_STATUS_EVT);				//上报更新进行状态	
							}		
							break;
						}					
					}							
				break;
				default: break;
			}	
			return;
		}
	}
}

/*****************************************************************************
* Function		: AsciitoInt
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
uint16_t	AsciitoInt(char *s)
{
	uint16_t data;
	switch(strlen(s))													
	{
		case 1:	{data =s[0]-'0';}	break;
		case 2:	{data =(s[0]-'0')*10+(s[1]-'0');}	break;
		case 3: {data =(s[0]-'0')*100+(s[1]-'0')*10+(s[2]-'0');}	break;
		case 4: {data =(s[0]-'0')*1000+(s[1]-'0')*100+(s[2]-'0')*10+(s[3]-'0');}	break;
		default: data ='0';break;
	}
	return data;
}
/*****************************************************************************
* Function		: ChartoInt
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
uint8_t	ChartoInt(char c)
{	
	uint8_t data;
	switch(c)
	{
		case	'0':	data =0; break;
		case	'1':	data =1; break;
		case	'2':	data =2; break;
		case	'3':	data =3; break;
		case	'4':	data =4; break;
		case	'5':	data =5; break;
		case	'6':	data =6; break;
		case	'7':	data =7; break;
		case	'8':	data =8; break;
		case	'9':	data =9; break;		
		case	'a':	data =10; break;		
		case	'b':	data =11; break;		
		case	'c':	data =12; break;		
		case	'd':	data =13; break;		
		case	'e':	data =14; break;		
		case	'f':	data =15; break;				
		case	'A':	data =10; break;		
		case	'B':	data =11; break;		
		case	'C':	data =12; break;		
		case	'D':	data =13; break;		
		case	'E':	data =14; break;		
		case	'F':	data =15; break;	
		default: data =0; break;
	}
	return data;
}
/*****************************************************************************
* Function		: InttoAscii
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
uint8_t	InttoAscii(uint8_t data)
{
	switch(data)
	{
		case 0:	data='0';	break;
		case 1:	data='1';	break;
		case 2:	data='2';	break;
		case 3:	data='3';	break;
		case 4:	data='4';	break;
		case 5:	data='5';	break;
		case 6:	data='6';	break;
		case 7:	data='7';	break;
		case 8:	data='8';	break;
		case 9:	data='9';	break;
		case 10:data='a';	break;
		case 11:data='b';	break;
		case 12:data='c';	break;
		case 13:data='d';	break;
		case 14:data='e';	break;
		case 15:data='f';	break;		
		default:data='0'; break;
	}
	return data;
}
/*****************************************************************************
* Function		: InttoAscii
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
uint8_t	Update_MQTT_Node_Report_Status(uint8_t node_num,uint8_t mode)
{
	uint8_t	status =0;
	if((mode ==MQTT_REPORT_MODE_PWRON)||(mode ==MQTT_REPORT_MODE_RLINK))
	{
		if(Node_Infor[node_num].Node_Status ==0)
		{//充电宝不在
			status =0;
		}
		else if((Node_Infor[node_num].Node_Status ==1)&&(Node_Infor[node_num].M_State ==M_FAIL))
		{//充电宝在,电池铁锁读不到
			status =2;
		}
		else if((Node_Infor[node_num].Node_Status ==1)&&(Node_Infor[node_num].M_State ==M_OK))
		{//充电宝在,电池铁锁读到
			status =1;
		}
	}
	else
	{
		if((Node_Infor[node_num].Node_Status ==0)&&(M_Status[node_num]==M_OPEN))
		{//充电宝不在,电池铁打开
			status =0;
		}
		else if((Node_Infor[node_num].Node_Status ==0)&&(M_Status[node_num]==M_CLOSE))
		{//充电宝不在,电池铁关闭
			status =3;
		}			
		else if((Node_Infor[node_num].Node_Status ==1)&&(Node_Infor[node_num].M_State ==M_FAIL))
		{//充电宝在,电池铁锁读不到
			status =2;
		}
		else if((Node_Infor[node_num].Node_Status ==1)&&(Node_Infor[node_num].M_State ==M_OK))
		{//充电宝在,电池铁锁读到
			status =1;
		}
	}
	return status;
}