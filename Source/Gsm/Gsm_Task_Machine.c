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
#include	"SN32F240.h"
#include	"GSM_Task_Machine.h"
#include 	"..\Driver\GPIO\gpio.h"
#include	"..\Driver\Timer\Timer.h"
#include	"..\Driver\Uart\Uart.h"
#include	"..\Utility\Utility.h"
#include	"..\UserApp\Sys_Task_Machine.h"
#include	"..\IntAndGnd\IntAndGnd_Task_Machine.h"
#include	"..\Node\Node_Task_Machine.h"
#include	"Gsm_string.h"
#include	"algorithm.h"
#include	"..\EDCode\md5.h"
#include	"..\Driver\ExterFlash\ExterFlash.h"
#include	"..\Driver\Flash\Flash.h"
#include	"..\Node\Node_Task_Ctrl.h"
/*_____ D E F I N I T I O N S ______________________________________________*/
__attribute__((at(0x0000FF00))) const uint32_t SecKey_Tab[4]={0xffffffff,0xffffffff,0xffffffff,0xffffffff};
__attribute__((at(0x0000FF10))) const uint32_t ICCID_ID_Tab[5]={0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff};
#if ID_DEBUG==0
__attribute__((at(0x0000FF24)))	const uint32_t RollingTable={0xffffffff}; //生产批次
#else
//__attribute__((at(0x0000FF24)))	const uint32_t RollingTable={0x00274968}; //debug
//__attribute__((at(0x0000FF24)))	const uint32_t RollingTable={0x20170701}; //debug
__attribute__((at(0x0000FF24)))	const uint32_t RollingTable={0x20170711}; 	//debug
//__attribute__((at(0x0000FF24)))	const uint32_t RollingTable={0x20170818}; //debug
//__attribute__((at(0x0000FF24)))	const uint32_t RollingTable={0x20170901}; //debug
//__attribute__((at(0x0000FF24)))	const uint32_t RollingTable={0x20171226}; //debug
#endif
__attribute__((at(0x0000FF28)))	const uint32_t Rom_VerTab={0xff080100};

//==============================================================================
const	char	AT_Tab[] ={"AT\r\n"};								//AT测试
const	char	IPR_Tab[] ={"AT+IPR=115200\r\n"};		//设置波特率
const	char	CPIN_Tab[] ={"AT+CPIN?\r\n"};				//检测SIM卡
const	char 	CREG_Tab[]={"AT+CREG?\r\n"};				//检测网络注册
const	char	CSQ_Tab[]={"AT+CSQ\r\n"};						//读取信号质量
const	char 	CGATT_Tab[]={"AT+CGATT?\r\n"};			//检测GPRS是否成功
const	char	CSTT_Tab[]={"AT+CSTT=\"CMIOT\"\r\n"};//设置APN
const	char	CIICR_Tab[]={"AT+CIICR\r\n"};				//激活PDP场景
const	char	CIFSR_Tab[]={"AT+CIFSR\r\n"};				//获取模块本机IP地址
const	char	EGMR_Tab[]={"AT+EGMR=0,5\r\n"};			//获取sn序列号
const	char	ICCID_Tab[]={"AT+ICCID\r\n"};				//获取ICCID			
const	char	CGSN_Tab[]={"AT+CGSN\r\n"};					//获取IMEI
const	char	CCLK_Tab[]={"AT+CCLK?\r\n"};				//获取时间
const	char	CNUM_Tab[]={"AT+CNUM\r\n"};					//读取手机号码
const	char	CPOWD_Tab[]={"AT+CPOWD=1\r\n"};			//软件关闭
const	char	CIPSTATUS_Tab[]={"AT+CIPSTATUS?"};	//
const	char	MIPCLOSE_Tab[]={"AT+MIPCLOSE\r\n"}; //MQTT Close
const	char	MIPDISCONNECT_Tab[]={"AT+MDISCONNECT\r\n"}; //MQTT Close
const	char	ISL_Tab[] ={"AT+ISLKVRSCAN\r\n"};			//读取2G版本
//==============================================================================
#if NET_DEBUG==1
const	char	MIPSTART_Tab[]={"AT+MIPSTART=\"mq2.dian.so\",\"1883\"\r\n"};//建立与服务器的TCP/IP连接
#endif
#if NET_DEBUG==2
const	char	MIPSTART_Tab[]={"AT+MIPSTART=\"mqtt-pre1.dian.so\",\"1883\"\r\n"};//建立与服务器的TCP/IP连接
#endif
#if NET_DEBUG==0
const	char	MIPSTART_Tab[]={"AT+MIPSTART=\"mq.dian.so\",\"1883\"\r\n"};//建立与服务器的TCP/IP连接
#endif

const	char	MCONNECT_Tab[]={"AT+MCONNECT=1,180\r\n"};										//与服务器建立MQTT连接
const	char	MCONFIG_Tab[]={"AT+MCONFIG=\"######################\",\"######################\",\"****************\"\r\n"};
const	char	MSUB_Tab[]={"AT+MSUB=\"device/in/######################\",1\r\n"};
const	char	MPUB_Tab[]={"AT+MPUB=\"device/out/"}; 
const	char	MQTTMSGET_Tab[] ={"AT+MQTTMSGGET\r\n"};
const	char	MQTTMSGET0_Tab[] ={"AT+ MQTTMSGGET=0\r\n"};
const	char	MQTTMSGET1_Tab[] ={"AT+MQTTMSGGET=1\r\n"};
const	char	MQTTMSGET2_Tab[] ={"AT+MQTTMSGGET=2\r\n"};
const	char	MQTTMSGET3_Tab[] ={"AT+MQTTMSGGET=3\r\n"};
const	char	MQTTSTATU_Tab[]={"AT+MQTTSTATU\r\n"};								//读取MQTT连接状态
const	char	MQTTTMOUT_Tab[]={"AT+MQTTTMOUT=200\r\n"};						//MQTT Time Out
//==============================================================================
const	char	HTTPSSL0_Tab[]={"AT+HTTPSSL=0\r\n"};								//设置使用HTTP
#if NET_DEBUG==1
const	char	HTTPPARA_URL_Tab[]={"AT+HTTPPARA=URL,\"bdev.dian.so/lhc/2.0/device/handle?a="};//设置HTTP URL
#endif
#if NET_DEBUG==2
const	char	HTTPPARA_URL_Tab[]={"AT+HTTPPARA=URL,\"lhc-pre1.dian.so/lhc/2.0/device/handle?a="};//设置HTTP URL
#endif
#if NET_DEBUG==0
const	char	HTTPPARA_URL_Tab[]={"AT+HTTPPARA=URL,\"b.dian.so/lhc/2.0/device/handle?a="};
#endif
const	char	HTTPPARA_PORT_Tab[]={"AT+HTTPPARA=PORT,80\r\n"};		//设置HTTP端口
const	char	HTTPSETUP_Tab[]={"AT+HTTPSETUP\r\n"};								//创建HTTP连接
const	char	HTTPACTION0_Tab[]={"AT+HTTPACTION=0\r\n"};					//GET HTTP Data
const	char	HTTPCLOSE_Tab[]={"AT+HTTPCLOSE\r\n"};								//关闭HTTP
//==============================================================================
const	char	HTTPSSL1_Tab[]={"AT+HTTPSSL=1\r\n"};								//设置使用HTTPS
const	char	HTTPSPARA_URL_Tab[]={"AT+HTTPPARA=URL,"};						//设置HTTPS URL
const	char	HTTPSPARA_PORT_Tab[]={"AT+HTTPPARA=PORT,443\r\n"};	//设置HTTPS端口
const	char	HTTPSPARA_BREAK_Tab[]={"AT+HTTPPARA=BREAK,"};				//分段请求起始位置
const	char	HTTPSPARA_BREAKEND_Tab[]={"AT+HTTPPARA=BREAKEND,"};	//分段请求结束位置
const	char	HTTPSPARA_CONNECT0_Tab[]={"AT+HTTPPARA=connect,0\r\n"};////设置HTTPS短连接 
const	char	HTTPSPARA_CONNECT1_Tab[]={"AT+HTTPPARA=connect,1\r\n"};//设置HTTPS长连接 
//==============================================================================
const	char	HTTP_ACTIVE_Tab[]={"xd.device.active"};
const	char	HTTP_OPEN_Tab[]={"xd.device.open.update"};
const	char	HTTP_UPDATE_STATUS_Tab[]={"xd.device.upgrade.status.update"};
//==============================================================================
const	uint8_t	GSM_Init_Tab[]={GSM_TX_AT_CMD,GSM_TX_AT_CMD,GSM_TX_AT_CMD,GSM_TX_IRP_CMD,GSM_TX_CPIN_CMD, \
	GSM_TX_CREG_CMD,GSM_TX_CGATT_CMD,GSM_TX_CSTT_CMD,GSM_TX_CSQ_CMD,GSM_TX_CIICR_CMD,GSM_TX_CIFSR_CMD, \
	GSM_TX_EGMR_CMD,GSM_TX_ICCID_CMD,GSM_TX_CGSN_CMD,GSM_TX_ISL_CMD,GSM_TX_OVER_CMD}; \
const	uint8_t	GSM_POWD_Tab[]={GSM_POWD_CMD,GSM_TX_OVER_CMD};

const	uint8_t	GSM_MQTT_Init00_Tab[]={GSM_TX_MCONFIG_CMD,GSM_TX_MIPSTART_CMD,GSM_TX_MCONNECT_CMD,GSM_TX_MSUB_CMD,GSM_TX_OVER_CMD};
const	uint8_t	GSM_MQTT_Init05_Tab[]={GSM_TX_MQTTTMOUT_CMD,GSM_TX_MCONFIG_CMD,GSM_TX_MIPSTART_CMD,GSM_TX_MCONNECT_CMD,GSM_TX_MSUB_CMD,GSM_TX_OVER_CMD};
const	uint8_t	GSM_MQTT_Get_Tab[]={GSM_TX_MSGGET_CMD,GSM_TX_OVER_CMD};
const	uint8_t	GSM_MQTT_Get0_Tab[]={GSM_TX_MSGGET0_CMD,GSM_TX_OVER_CMD};
const	uint8_t	GSM_MQTT_Get1_Tab[]={GSM_TX_MSGGET1_CMD,GSM_TX_OVER_CMD};
const	uint8_t	GSM_MQTT_Get2_Tab[]={GSM_TX_MSGGET2_CMD,GSM_TX_OVER_CMD};
const	uint8_t	GSM_MQTT_Get3_Tab[]={GSM_TX_MSGGET3_CMD,GSM_TX_OVER_CMD};
const	uint8_t	GSM_MQTT_PUB_Tab[]={GSM_CCLK_CMD,GSM_TX_CSQ_CMD,GSM_TX_MPUB_CMD,GSM_TX_OVER_CMD};
const	uint8_t	GSM_MQTT_Stop_Tab[]={GSM_TX_MIPCLOSE_CMD,GSM_TX_OVER_CMD};
const	uint8_t	GSM_MQTT_STATU_Tab[]={GSM_TX_MQTTSTATU_CMD,GSM_TX_OVER_CMD};

const	uint8_t	GSM_HTTP_Init_Tab[]={GSM_HTTP_CLOSE_CMD,GSM_HTTPSSL0_CMD,GSM_CCLK_CMD,GSM_HTTPPARA_PORT_CMD,GSM_HTTPPARA_URL_CMD,GSM_HTTPSETUP_CMD,GSM_HTTPACTION0_CMD,GSM_TX_OVER_CMD};
const	uint8_t	GSM_HTTPS_Init_Tab[]={GSM_HTTP_CLOSE_CMD,GSM_HTTPSSL1_CMD,GSM_HTTPSPARA_CONNECT1_CMD,GSM_CCLK_CMD,GSM_HTTPSPARA_PORT_CMD,GSM_HTTPSPARA_URL_CMD,GSM_HTTPSETUP_CMD,GSM_TX_OVER_CMD};
const	uint8_t	GSM_HTTP_GET_Update_Data_Tab[]={GSM_HTTPSBREAK_CMD,GSM_HTTPSBREAKEND_CMD,GSM_HTTPACTION0_CMD,GSM_TX_OVER_CMD};
//==============================================================================	
uint8_t		*Gsm_Tx_Tab;						//指向要发送的命令表
uint8_t		Gsm_Tab_pt;							//发送的命令表指针
uint8_t		Gsm_State;							//Gsm状态
uint8_t		Gsm_Tx_State;						//Gsm发送状态
uint8_t		Gsm_Pwr_State;					//Gsm Power状态
uint32_t	Tm_Gsm_Ini;							//Gsm 上电时间delay
uint32_t	Gsm_Tx_TmOut;						//Gsm 命令发送溢出时间
uint8_t		Gsm_Tx_CntOut;					//Gsm 命令发送溢出次数
uint32_t	Gsm_Tx_TmOut_Tmp;				//Gsm 命令发送临时溢出时间
uint8_t		Gsm_Tx_CntOut_Tmp;			//Gsm 命令发送临时溢出次数

char			Device_ID[23];					//设备ID
char			ICCID_ID[20];						//CCID
char			SN_ID[14];							//SN
char			IMEI_ID[15];						//IMEI
char 			SecKey[17];							//加密码
char			PassWord[16];						//服务器登陆密码
char			Num_ID[12];							//手机号码
char			Sign[32];								//签名
char			Rom_Version[6];					//Rom version
uint64_t	Timestamp;							//时间戳
uint64_t	Timestamp_NLink;				//丢失连接时间戳
uint8_t		CSQ_Buf;								//信号强度

char			Gsm_URx_Dt[800];				//Gsm Uart接收数据
char			Gsm_Rx_Dt[450];					//Gsm Uart数据解码
char			Http_Update_Web[90];		//Https更新网址

uint8_t		Gsm_MQTT_Rept_Mode;			//MQTT Report Mode
uint8_t		Gsm_Rept_Node;					//Gsm 上报的节点号
uint8_t		Gsm_Txing;							//Gsm正在发送标志
uint8_t		FGsm_Init_OK;						//Gsm初始化OK
uint8_t		FGsm_Rdy;								//Gsm Modul Ready
//uint8_t		Gsm_Pack_data[500];
uint8_t		Gsm_Pack_data[450];
uint8_t		FGsm_MQTT_Tx_OK;
uint8_t		FGsm_MQTT_Init_OK;
uint8_t		FGsm_Tx_Enb;
uint8_t		FGsm_HTTP_Rx_OK;

uint16_t	GsmEvent_Ff[GSM_EVT_FF_LEN];
uint8_t		Wt_GsmEvent_Pt;
uint8_t		Rd_GsmEvent_Pt;
uint16_t	Gsm_Event;
uint32_t	Gsm_Timer_Tx;
uint8_t		Gsm_Node_Rpt_Step;
uint8_t		FMQTT_NLink;
uint8_t		FHTTP_Link;
uint8_t		MQTT_Close_Cnt;
uint8_t		Http_Update;
uint32_t	Http_Update_StrAddr;				//Http更新开始地址
uint32_t	Http_Update_StpAddr;				//Http更新结束地址
uint32_t	Http_Req_Tmout;							//
uint8_t		Http_Req_Cnt;								//
uint8_t		Http_Req_Cnt_Tmp;						//
uint8_t		Http_Update_Status;					//
uint32_t	Http_Update_Tmout;					//
uint16_t	CheckSum;										//
uint16_t	CheckSum_Buf1;							//
uint16_t	CheckSum_Buf2;							//
uint8_t		Https_GetData_OK;						//
uint8_t		Gsm_ReTx_Process_Cnt;				//流程重发次数
uint16_t	Gsm_MQTT_Tmlink;						//
uint8_t		MQTT_PUB_ACK_ID;						//
uint8_t		Gsm_Ver_ID;
uint16_t 	IC_year;										//IC Timer Year
uint8_t		IC_mon;											//IC Timer month
uint8_t		IC_day;											//IC Timer day
uint8_t		IC_hour;										//IC Timer hour
uint8_t		IC_min;											//IC Timer min
uint8_t		IC_sec;											//IC Timer sec
/*****************************************************************************
* Function		: GSM_Task_Machine
* Description	: Gsm Module Task
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void GSM_Task_Machine(void)
{
	switch(Gsm_State)
	{
		case GSM_PWR_STA:		GSM_Power_Handle();	break;
		case GSM_TX_STA:		GSM_Tx_Handle();		break;
		case GSM_IDLE_STA:	GSM_Idle_Handle();	break;
		default:	break;
	}
}
/*****************************************************************************
* Function		: Gsm_Ram_Init
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Gsm_Tx_Ram_Init(void)
{
	Gsm_Txing =0;									//清正在发送标志
	FGsm_Tx_Enb =0;								//清发送使能标志
	Gsm_Tx_TmOut =0;
	Gsm_Tx_CntOut=0;
	Gsm_Tx_TmOut_Tmp =0;
	Gsm_Tx_CntOut_Tmp =0;
}
/*****************************************************************************
* Function		: Gsm_Http_Ram_Init
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Gsm_Http_Ram_Init(void)
{
	Http_Update =0;								//
	FHTTP_Link =0;								//默认HTTP 关闭
	Http_Req_Tmout =0;
	Http_Req_Cnt =0;
	Http_Req_Cnt_Tmp =0;
}
/*****************************************************************************
* Function		: Clear_Gsm_Ff
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Clear_Gsm_Ff(void)
{ uint8_t i;
	Sys_GsmTask_Step =0;
	Wt_GsmEvent_Pt =0;
	Rd_GsmEvent_Pt =0;
	for(i=0;i<GSM_EVT_FF_LEN;i++) GsmEvent_Ff[i] =0;
}
/*****************************************************************************
* Function		: GSM_Power_Handle
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void GSM_Power_Handle(void)
{
	uint8_t	cnt;
	switch(Gsm_Pwr_State)
	{
		case GSM_VOL_ON_STA:
			GSM_PWR_ON													//打开电源
			FGsm_Rdy =0;												//
			Gsm_Pwr_State =GSM_PWR_IDLE_STA; 		//
		break;
		case GSM_PWR_IDLE_STA:
			Gsm_State = GSM_IDLE_STA;						//Into Gsm Idle status
		break;			
		case GSM_VOL_OFF_STA:
			GSM_PWR_OFF
			Gsm_Pwr_State =GSM_VOL_ON_STA;			//
			UT_DelayNms(3000);									//
//			UT_DelayNms(3000);									//
//			UT_DelayNms(3000);									//
		break;
		default:	break;
	}
}
/*****************************************************************************
* Function		: GSM_Idle_Handle
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	GSM_Idle_Handle(void)
{
	Gsm_Rx_Task();
	Gsm_Tx_Task();
}
/*****************************************************************************
* Function		: Gsm_Tx_Task
* Description	: 发送整个通信流程，并判断是否发送完成
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Gsm_Tx_Task(void)
{
	if(FGsm_Tx_Enb ==0) return;														//发送流程是否开始?
	if((Gsm_Txing==0)&&(Uart_Rx_Pt ==0))									//没有发送并且接收数据处理完才可以发送
	{
		if(*(Gsm_Tx_Tab+Gsm_Tab_pt) !=GSM_TX_OVER_CMD)			//是否所有发送结束？
		{
			Gsm_Tx_State = *(Gsm_Tx_Tab+Gsm_Tab_pt);					//没有结束，查找表发送
			Gsm_State = GSM_TX_STA;
			Gsm_Tab_pt++;
			Gsm_Tx_CntOut_Tmp =0;															//清发送溢出计数Temp Buffer
		}
		else																								//所有发送结束
		{
			FGsm_Tx_Enb =0;																		//流程发送完成,关闭发送流程
			Gsm_ReTx_Process_Cnt =0;													//流程发送完成,清计数
			switch(Gsm_Event&0xff)
			{
				case	GSM_INIT_EVT: 
				case	GSM_CLOSE_EVT:
				case	GSM_MQTT_INIT_EVT:
				case	GSM_MQTT_PWRON_EVT:
				case	GSM_MQTT_TIMER_EVT:
				case	GSM_MQTT_OUT_EVT:
				case	GSM_MQTT_ONFAIL_EVT:
				case	GSM_MQTT_ONEREP_EVT:
				case	GSM_MQTT_CLOSE_EVT:
				case	GSM_MQTT_RLINK_EVT:
				case	GSM_MQTT_GETDATA_EVT:
				case	GSM_MQTT_STOP_EVT:
				case	GSM_MQTT_STATUS_EVT:
				case	GSM_MQTT_GETDATA0_EVT:
				case	GSM_MQTT_GETDATA1_EVT:
				case	GSM_MQTT_GETDATA2_EVT:
				case	GSM_MQTT_GETDATA3_EVT:
				case	GSM_MQTT_OPENREQ_EVT:
					FGsm_MQTT_Tx_OK =1;
				break;
				case	GSM_HTTP_ACTIVE_EVT:	{Http_Req_Tmout =3000;Http_Req_Cnt =1;}break;	//需要接受服务器数据，初始化接收时间
				case	GSM_HTTP_OPENREQ_EVT:	{Http_Req_Tmout =3000;Http_Req_Cnt =1;}break; //需要接受服务器数据，初始化接收时间
				case	GSM_HTTP_UPDATE_STATUS_EVT: {Http_Req_Tmout =3000;Http_Req_Cnt =1;}break; //需要接受服务器数据，初始化接收时间
				case	GSM_HTTPS_INIT_EVT:	FGsm_MQTT_Tx_OK =1; break;
				case	GSM_HTTPS_GETDATA_EVT: break;	
				default: break;
			}
			Gsm_Tx_State = GSM_TX_IDLE;
		}
	}
}
/*****************************************************************************
* Function		: Gsm_Rx_Task
* Description	: 20ms没有uart数据，认为接收结束
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Gsm_Rx_Task(void)
{
	uint32_t i;
	//============================================================================
	if(FTm_10ms ==0) return;
	FTm_10ms =0;
	if(Uart_Rx_TmOut !=0)
	{
		if(--Uart_Rx_TmOut ==0)
		{
			if(Uart_Rx_Pt !=0)
			{
				Uart_Rx_Cnt = Uart_Rx_Pt;		//保存接收数量
				Uart_Rx_Pt =0;
				for(i=0;i<Uart_Rx_Cnt;i++)
				{
					Gsm_URx_Dt[i] =Uart_Rx_Buf[i];	Uart_Rx_Buf[i] =0;
				}
				Gsm_URx_Dt[i] ='\0';
				Handle_Gsm_URx_Dt();
			}
		}
	}
}
/*****************************************************************************
* Function		: Gsm_Tx_ChkTmout
* Description	: 判断发送每一个命令是否正常，并重发
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Gsm_Tx_ChkTmout(void)
{
	if((Gsm_Tx_TmOut !=0)&&(Gsm_Txing ==1))				//AT在发送中
	{
		if(--Gsm_Tx_TmOut==0)												//AT发送超时
		{
			if(++Gsm_Tx_CntOut_Tmp >=Gsm_Tx_CntOut)		//判断重发次数															
			{
				Gsm_Tx_State =GSM_TX_IDLE;							//AT多次未发送成功，根据不同事件处理
				Gsm_State =GSM_IDLE_STA;								//
				Sys_GsmTask_Step =0;										//重新发送流程
				Gsm_Tx_Ram_Init();											//清发送buffer
				switch(Gsm_Event&0xff)
				{
					case	GSM_INIT_EVT:										//Gsm Initial
					case	GSM_MQTT_INIT_EVT:							//MQTT Initial
					case	GSM_MQTT_STOP_EVT:							//MQTT 
						Clear_Gsm_Ff();	
						Wt_GsmEvent_Ff(GSM_CLOSE_EVT);		//软件关闭gsm		
					break;
					case	GSM_MQTT_PWRON_EVT:							//MQTT PUB 失败
					case	GSM_MQTT_TIMER_EVT:							//
					case	GSM_MQTT_OUT_EVT:								//
					case	GSM_MQTT_ONFAIL_EVT:						//
					case	GSM_MQTT_ONEREP_EVT:						//
					case	GSM_MQTT_CLOSE_EVT:							//
					case	GSM_MQTT_RLINK_EVT:							//
					case	GSM_MQTT_GETDATA_EVT:						//
					case	GSM_MQTT_STATUS_EVT:						//
					case	GSM_MQTT_GETDATA0_EVT:					//
					case	GSM_MQTT_GETDATA1_EVT:					//
					case	GSM_MQTT_GETDATA2_EVT:					//
					case	GSM_MQTT_GETDATA3_EVT:					//
					case	GSM_MQTT_OPENREQ_EVT:						//
						Clear_Gsm_Ff();											//
						Wt_GsmEvent_Ff(GSM_MQTT_STOP_EVT);	//关闭MQTT	
					break;
					case	GSM_CLOSE_EVT:									//软件GSM失败
						Clear_Gsm_Ff();	
						Gsm_State =GSM_PWR_STA;							//硬件关闭电源	
						Gsm_Pwr_State =GSM_VOL_OFF_STA;			//									
					break;
					case	GSM_HTTP_ACTIVE_EVT:						//HTTP 激活失败
						Clear_Gsm_Ff();	
						Wt_GsmEvent_Ff(GSM_CLOSE_EVT);				
					break;
					case	GSM_HTTP_OPENREQ_EVT:						//其他HTTP/HTTPS失败
					case	GSM_HTTP_UPDATE_STATUS_EVT:
					case	GSM_HTTPS_INIT_EVT:
						Sys_GsmTask_Step =1;
						if(++Gsm_ReTx_Process_Cnt>=3)				//流程重复发送
						{
							Gsm_ReTx_Process_Cnt =0;
							Sys_GsmTask_Step =0;							//
						}
					break;
					case	GSM_HTTPS_GETDATA_EVT:
							Sys_GsmTask_Step =0;							//
					break;
					default: break;
				}
			}
			else
			{
				//Gsm_Tx_TmOut =Gsm_Tx_TmOut_Tmp;					//重新发送cmd 
				Gsm_State =GSM_TX_STA;
			}		
		}
	}
}
/*****************************************************************************
* Function		: GSM_Tx_Handle
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void GSM_Tx_Handle(void)
{
	switch (Gsm_Tx_State)
	{
		case GSM_TX_AT_CMD:																						//发送AT				
			GSM_Ttansmit((char *)AT_Tab,strlen(AT_Tab),500,20);
		break;		
		case GSM_TX_IRP_CMD:																					//设置固定波特率
			GSM_Ttansmit((char *)IPR_Tab,strlen(IPR_Tab),500,3);				
		break;		
		case GSM_TX_CPIN_CMD:																					//查询SIM卡状态		
			GSM_Ttansmit((char *)CPIN_Tab,strlen(CPIN_Tab),500,5);				
		break;		
		case GSM_TX_CREG_CMD:																					//查询找网状态	
			GSM_Ttansmit((char *)CREG_Tab,strlen(CREG_Tab),1000,5);					
		break;		
		case GSM_TX_CGATT_CMD:																				//查询GPRS是否成功		
			GSM_Ttansmit((char *)CGATT_Tab,strlen(CGATT_Tab),1000,5);			
		break;
		case GSM_TX_CSTT_CMD:																					//设置APN		
			GSM_Ttansmit((char *)CSTT_Tab,strlen(CSTT_Tab),500,5);				
		break;			
		case	GSM_TX_CIICR_CMD:																				//激活PDP场景		
			GSM_Ttansmit((char *)CIICR_Tab,strlen(CIICR_Tab),1000,5);					
		break;
		case	GSM_TX_CSQ_CMD:																					//读取信号强度		
			GSM_Ttansmit((char *)CSQ_Tab,strlen(CSQ_Tab),500,3);												
		break;
		case	GSM_TX_CIFSR_CMD:																				//读取模块IP地址		
			GSM_Ttansmit((char *)CIFSR_Tab,strlen(CIFSR_Tab),1000,5);								
		break;
		case	GSM_TX_EGMR_CMD:																				//读取SN		
			GSM_Ttansmit((char *)EGMR_Tab,strlen(EGMR_Tab),500,3);					
		break;
		case	GSM_TX_ICCID_CMD:																				//读取ICCID
			GSM_Ttansmit((char *)ICCID_Tab,strlen(ICCID_Tab),500,3);			
		break;
		case	GSM_CNUM_CMD:																						//读取手机号码
			GSM_Ttansmit((char *)CNUM_Tab,strlen(CNUM_Tab),500,3);		
		break;
		case	GSM_TX_CGSN_CMD:																				//读取IMEI
			GSM_Ttansmit((char *)CGSN_Tab,strlen(CGSN_Tab),500,3);					
		break;
		case	GSM_CCLK_CMD:																						//获取时间
			GSM_Ttansmit((char *)CCLK_Tab,strlen(CCLK_Tab),500,3);				
		break;
		case	GSM_POWD_CMD:
			GSM_Ttansmit((char *)CPOWD_Tab,strlen(CPOWD_Tab),1000,3);	
		break;
		case	GSM_TX_ISL_CMD:																					//读取模组版本
			GSM_Ttansmit((char *)ISL_Tab,strlen(ISL_Tab),500,3);
		break;
		case	GSM_CIPSTATUS_CMD:																				//取得状态
			GSM_Ttansmit((char *)CIPSTATUS_Tab,strlen(CIPSTATUS_Tab),500,3);	
		break;
		case	GSM_TX_MCONFIG_CMD:																			//配置MQTT参数	
			GSM_Ttansmit_Param((char *)MCONFIG_Tab,strlen(MCONFIG_Tab),500,3);						
		break;
		case	GSM_TX_MIPSTART_CMD:																		//建立与服务器的TCP/IP连接		
			GSM_Ttansmit((char *)MIPSTART_Tab,strlen(MIPSTART_Tab),2000,2);		
		break;
		case	GSM_TX_MCONNECT_CMD:																		//与服务器建立MQTT连接		
			GSM_Ttansmit((char *)MCONNECT_Tab,strlen(MCONNECT_Tab),2000,2);			
		break;
		case	GSM_TX_MSUB_CMD:																				//Sub 
			GSM_Ttansmit_Param((char *)MSUB_Tab,strlen(MSUB_Tab),1000,3);		
		break;
		case	GSM_TX_MSGGET_CMD:																			//读取PUB数据
			GSM_Ttansmit((char *)MQTTMSGET_Tab,strlen(MQTTMSGET_Tab),1000,3);	
		break;
		case	GSM_TX_MSGGET0_CMD:																			//读取PUB数据
			GSM_Ttansmit((char *)MQTTMSGET0_Tab,strlen(MQTTMSGET0_Tab),1000,3);	
		break;
		case	GSM_TX_MSGGET1_CMD:																			//读取PUB数据
			GSM_Ttansmit((char *)MQTTMSGET1_Tab,strlen(MQTTMSGET1_Tab),1000,3);	
		break;
		case	GSM_TX_MSGGET2_CMD:																			//读取PUB数据
			GSM_Ttansmit((char *)MQTTMSGET2_Tab,strlen(MQTTMSGET2_Tab),1000,3);	
		break;
		case	GSM_TX_MSGGET3_CMD:																			//读取PUB数据
			GSM_Ttansmit((char *)MQTTMSGET3_Tab,strlen(MQTTMSGET3_Tab),1000,3);	
		break;		
		case	GSM_TX_MQTTSTATU_CMD:																		//读取MQTT连接状态
			GSM_Ttansmit((char *)MQTTSTATU_Tab,strlen(MQTTSTATU_Tab),1000,3);	
		break;
		case	GSM_TX_MQTTTMOUT_CMD:																		//设置MQTT连接超时
			GSM_Ttansmit((char *)MQTTTMOUT_Tab,strlen(MQTTTMOUT_Tab),1000,3);	
		break;		
		case	GSM_TX_MPUB_CMD:																				//Pub
			Gsm_MQTT_Report_Payload_Init(Gsm_MQTT_Rept_Mode);	
			MQTT_PUB_ACK_ID =0;																					
			GSM_Ttansmit_Data((char *)MPUB_Tab,strlen(MPUB_Tab),4000,1);	
		break;		
		case	GSM_TX_MIPCLOSE_CMD:																		//MQTT  Close
			GSM_Ttansmit((char *)MIPCLOSE_Tab,strlen(MIPCLOSE_Tab),1000,3);
		break;
		case	GSM_TX_MIPDISCONNECT_CMD:
			GSM_Ttansmit((char *)MIPDISCONNECT_Tab,strlen(MIPDISCONNECT_Tab),500,3);
		break;
		case	GSM_HTTPSSL1_CMD:																				//使用HTTPS		
				GSM_Ttansmit((char *)HTTPSSL1_Tab,strlen(HTTPSSL1_Tab),500,3);		
		break;
		case	GSM_HTTPSSL0_CMD:																				//使用HTTP	
				GSM_Ttansmit((char *)HTTPSSL0_Tab,strlen(HTTPSSL0_Tab),500,3);			
		break;	
		case	GSM_HTTPPARA_URL_CMD:																		//设置目标URL		
			Gsm_HTTP_Report_Payload_Init();
			GSM_Ttansmit_Http((char *)HTTPPARA_URL_Tab,strlen(HTTPPARA_URL_Tab),500,3);	
			break;
		case	GSM_HTTPPARA_PORT_CMD:																	//设置端口
			GSM_Ttansmit((char *)HTTPPARA_PORT_Tab,strlen(HTTPPARA_PORT_Tab),500,3);					
		break;
		case	GSM_HTTPSETUP_CMD:																			//建立HTTP/HTTPS连接
			GSM_Ttansmit((char *)HTTPSETUP_Tab,strlen(HTTPSETUP_Tab),1500,1);				
		break;
		case	GSM_HTTP_CLOSE_CMD:																			//关闭HTTP
			GSM_Ttansmit((char *)HTTPCLOSE_Tab,strlen(HTTPCLOSE_Tab),1500,3);		
		break;
		case	GSM_HTTPACTION0_CMD:																		//Get服务器数据
			GSM_Ttansmit((char *)HTTPACTION0_Tab,strlen(HTTPACTION0_Tab),2000,3);	
		break;
		case	GSM_HTTPSPARA_URL_CMD:																	//HTTPS URL
			GSM_Ttansmit_HttpUpdate((char *)HTTPSPARA_URL_Tab,strlen(HTTPSPARA_URL_Tab),1500,3);	
		break;
		case	GSM_HTTPSBREAK_CMD:																			//分段开始地址 
			GSM_Ttansmit_Addr((char *)HTTPSPARA_BREAK_Tab,strlen(HTTPSPARA_BREAK_Tab),1500,3,Http_Update_StrAddr);	
		break;
		case	GSM_HTTPSBREAKEND_CMD:																	//分段结束地址
			GSM_Ttansmit_Addr((char *)HTTPSPARA_BREAKEND_Tab,strlen(HTTPSPARA_BREAKEND_Tab),1500,3,Http_Update_StpAddr);	
		break;
		case	GSM_HTTPSPARA_PORT_CMD:																	//设置HTTPS 端口
			GSM_Ttansmit((char *)HTTPSPARA_PORT_Tab,strlen(HTTPSPARA_PORT_Tab),500,3);	
		break;
		case	GSM_HTTPSPARA_CONNECT1_CMD:
			GSM_Ttansmit((char *)HTTPSPARA_CONNECT1_Tab,strlen(HTTPSPARA_CONNECT1_Tab),500,3);	
		break;
		default:	return;
	}	
#if UART_DEBUG ==1												//print
	Uart_Tx_Print(strlen(Uart_Tx_Buf),Uart_Tx_Buf);	
#endif		
	//UART1_SendByte(strlen(Uart_Tx_Buf));									//		
	
	Gsm_Txing =1;													//Gsm Txing
	Gsm_State =GSM_IDLE_STA;							//切换到idle
}
/*****************************************************************************
* Function		: GSM_Ttansmit
* Description	: 按固定表格长度字符串发送
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void GSM_Ttansmit(char *p,uint8_t len,uint32_t Tm_Out,uint8_t Retry_Cnt)
{
	uint32_t i;
	Gsm_Tx_TmOut =Tm_Out;									//Timeout
	Gsm_Tx_TmOut_Tmp =Tm_Out;							//	
	Gsm_Tx_CntOut =Retry_Cnt;							//Cnt Out

	for(i=0;i<len;i++)
	{
		Uart_Tx_Buf[i] = *(p+i);
	}
	Uart_Tx_Buf[i] ='\0';
}
/*****************************************************************************
* Function		: GSM_Ttansmit_Data
* Description	: 发送带数据包
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void GSM_Ttansmit_Data(char *p,uint8_t len,uint32_t Tm_Out,uint8_t Retry_Cnt)
{
	uint32_t i,j;
	Gsm_Tx_TmOut =Tm_Out;									//Timeout
	Gsm_Tx_TmOut_Tmp =Tm_Out;							//	
	Gsm_Tx_CntOut =Retry_Cnt;							//Cnt Out	

	for(i=0;i<len;i++)
	{
		Uart_Tx_Buf[i] =*(p+i);
	}			
	for(j=0;j<22;j++)											//Device ID
	{
		Uart_Tx_Buf[i++] =Device_ID[j];
	}			
	Uart_Tx_Buf[i++] ='\"'; Uart_Tx_Buf[i++] =',';	
	Uart_Tx_Buf[i++] ='1';  Uart_Tx_Buf[i++] =',';
  Uart_Tx_Buf[i++] ='1';  Uart_Tx_Buf[i++] =',';
	//Data
	Uart_Tx_Buf[i++] ='\"';
	for(j=0;j<strlen(Gsm_Pack_data);j++)
	{
		Uart_Tx_Buf[i++] =Gsm_Pack_data[j];
	}
	Uart_Tx_Buf[i++] ='\"';
	Uart_Tx_Buf[i++] ='\r';
	Uart_Tx_Buf[i++] ='\n';
	Uart_Tx_Buf[i] ='\0';
}
/*****************************************************************************
* Function		: GSM_Ttansmit_Param
* Description	: 发送带参数
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void GSM_Ttansmit_Param(char *p,uint8_t len,uint32_t Tm_Out,uint8_t Retry_Cnt)
{
	uint32_t i, j;
	Gsm_Tx_TmOut =Tm_Out;									//Timeout
	Gsm_Tx_TmOut_Tmp =Tm_Out;							//	
	Gsm_Tx_CntOut =Retry_Cnt;							//Cnt Out
	j =0;
	for(i=0;i<len;i++)
	{
		if(*(p+i) =='#')
		{
			Uart_Tx_Buf[i] =Device_ID[j++];
			if(j>=22) j=0;
		}
		else if(*(p+i) =='*')
		{
			Uart_Tx_Buf[i] =PassWord[j++];
			if(j>=16) j=0;		
		}
		else
		{
			Uart_Tx_Buf[i] =*(p+i);
		}
	}
  Uart_Tx_Buf[i] ='\0';
}
/*****************************************************************************
* Function		: GSM_Ttansmit_Http
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void GSM_Ttansmit_Http(char *p,uint8_t len,uint32_t Tm_Out,uint8_t Retry_Cnt)
{
	uint32_t i, j;
	Gsm_Tx_TmOut =Tm_Out;									//Timeout
	Gsm_Tx_TmOut_Tmp =Tm_Out;							//	
	Gsm_Tx_CntOut =Retry_Cnt;							//Cnt Out
	for(i=0;i<len;i++)											
	{
		Uart_Tx_Buf[i] =*(p+i);
	}
	switch(Gsm_Event &0xff)								//HTTP Interface
	{
		case	GSM_HTTP_ACTIVE_EVT:					//HTTP Active
			for(j=0;j<strlen(HTTP_ACTIVE_Tab);j++)
			{
				Uart_Tx_Buf[i++] =HTTP_ACTIVE_Tab[j];
			}			
		break;
		case GSM_HTTP_OPENREQ_EVT:					//HTTP Open 
			for(j=0;j<strlen(HTTP_OPEN_Tab);j++)		
			{																				
				Uart_Tx_Buf[i++] =HTTP_OPEN_Tab[j];								
			}	
		break;
		case	GSM_HTTP_UPDATE_STATUS_EVT:		//HTTP Update Status
			for(j=0;j<strlen(HTTP_UPDATE_STATUS_Tab);j++)		
			{																				
				Uart_Tx_Buf[i++] =HTTP_UPDATE_STATUS_Tab[j];								
			}				
		break;
		default: break;
	}
	Uart_Tx_Buf[i++] ='&';
	Uart_Tx_Buf[i++] ='d'; Uart_Tx_Buf[i++] ='e'; Uart_Tx_Buf[i++] ='v'; Uart_Tx_Buf[i++] ='I'; Uart_Tx_Buf[i++] ='d';Uart_Tx_Buf[i++] ='=';
	for(j=0;j<strlen(Device_ID);j++)
	{
		Uart_Tx_Buf[i++] =Device_ID[j];
	}
	Uart_Tx_Buf[i++] ='&';
	Uart_Tx_Buf[i++] ='t'; Uart_Tx_Buf[i++] ='=';
	Uart_Tx_Buf[i++] =Timestamp /1000000000+0x30;
	Uart_Tx_Buf[i++] =(Timestamp%1000000000)/100000000+0x30;
	Uart_Tx_Buf[i++] =(Timestamp%1000000000%100000000)/10000000+0x30;
	Uart_Tx_Buf[i++] =(Timestamp%1000000000%100000000%10000000)/1000000+0x30;
	Uart_Tx_Buf[i++] =(Timestamp%1000000000%100000000%10000000%1000000)/100000+0x30;
	Uart_Tx_Buf[i++] =(Timestamp%1000000000%100000000%10000000%1000000%100000)/10000+0x30;
	Uart_Tx_Buf[i++] =(Timestamp%1000000000%100000000%10000000%1000000%100000%10000)/1000+0x30;
	Uart_Tx_Buf[i++] =(Timestamp%1000000000%100000000%10000000%1000000%100000%10000%1000)/100+0x30;
	Uart_Tx_Buf[i++] =(Timestamp%1000000000%100000000%10000000%1000000%100000%10000%1000%100)/10+0x30;
	Uart_Tx_Buf[i++] =(Timestamp%1000000000%100000000%10000000%1000000%100000%10000%1000%100%10)/1+0x30;		
	Uart_Tx_Buf[i++] ='&';
	Uart_Tx_Buf[i++] ='i'; Uart_Tx_Buf[i++] ='c'; Uart_Tx_Buf[i++] ='c'; Uart_Tx_Buf[i++] ='I'; Uart_Tx_Buf[i++] ='d';	Uart_Tx_Buf[i++] ='=';
	for(j=0;j<20;j++)		
	{																				
		Uart_Tx_Buf[i++] =ICCID_ID[j];								
	}		
	Uart_Tx_Buf[i++] ='&';
	Uart_Tx_Buf[i++] ='d'; Uart_Tx_Buf[i++] ='a'; Uart_Tx_Buf[i++] ='t'; Uart_Tx_Buf[i++] ='a'; Uart_Tx_Buf[i++] ='=';
	for(j=0;j<strlen(Gsm_Pack_data);j++)		
	{																				
		Uart_Tx_Buf[i++] =Gsm_Pack_data[j];								
	}	
	Uart_Tx_Buf[i++] ='&';
	Uart_Tx_Buf[i++] ='s'; Uart_Tx_Buf[i++] ='i'; Uart_Tx_Buf[i++] ='g'; Uart_Tx_Buf[i++] ='n'; Uart_Tx_Buf[i++] ='=';
	for(j=0;j<32;j++)		
	{																				
		Uart_Tx_Buf[i++] =Sign[j];								
	}		
	Uart_Tx_Buf[i++] ='\"';
	Uart_Tx_Buf[i++] ='\r';	Uart_Tx_Buf[i++] ='\n'; Uart_Tx_Buf[i] ='\0';
}
/*****************************************************************************
* Function		: GSM_Ttansmit_HttpUpdate
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void GSM_Ttansmit_HttpUpdate(char *p,uint8_t len,uint32_t Tm_Out,uint8_t Retry_Cnt)
{
	uint32_t i,j;
	Gsm_Tx_TmOut =Tm_Out;									//Timeout
	Gsm_Tx_TmOut_Tmp =Tm_Out;							//	
	Gsm_Tx_CntOut =Retry_Cnt;							//Cnt Out

	for(i=0;i<len;i++)
	{
		Uart_Tx_Buf[i] =*(p+i);
	}
	//Data
	Uart_Tx_Buf[i++] ='\"';
	for(j=0;j<strlen(Http_Update_Web);j++)
	{
		Uart_Tx_Buf[i++] =Http_Update_Web[j];
	}
	Uart_Tx_Buf[i++] ='\"';
	Uart_Tx_Buf[i++] ='\r';
	Uart_Tx_Buf[i++] ='\n';
	Uart_Tx_Buf[i] ='\0';	
}
/*****************************************************************************
* Function		: GSM_Ttansmit_Addr
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void GSM_Ttansmit_Addr(char *p,uint8_t len,uint32_t Tm_Out,uint8_t Retry_Cnt,uint32_t addr)
{
	uint32_t i,j;char	s[6];
	Gsm_Tx_TmOut =Tm_Out;									//Timeout
	Gsm_Tx_TmOut_Tmp =Tm_Out;							//	
	Gsm_Tx_CntOut =Retry_Cnt;							//Cnt Out

	for(i=0;i<len;i++)
	{
		Uart_Tx_Buf[i] =*(p+i);
	}
	//Data
	j=0;
	if((addr>=0)&&(addr<=9))							//
	{
		s[j++]=addr+0x30; 
	}
	else if((addr>=10)&&(addr<=99))				//
	{
		s[j++] =addr/10+0x30;
		s[j++] =(addr%10)/1+0x30;	
	}
	else if((addr>=100)&&(addr<=999))				//
	{
		s[j++] =addr/100+0x30;
		s[j++] =(addr%100)/10+0x30;
		s[j++] =(addr%100%10)/1+0x30;		
	}
	else if((addr>=1000)&&(addr<=9999))				//
	{
		s[j++] =addr/1000+0x30;
		s[j++] =(addr%1000)/100+0x30;
		s[j++] =(addr%1000%100)/10+0x30;	
		s[j++] =(addr%1000%100%10)/1+0x30;		
	}
	else if((addr>=10000)&&(addr<=99999))				//
	{
		s[j++] =addr/10000+0x30;
		s[j++] =(addr%10000)/1000+0x30;
		s[j++] =(addr%10000%1000)/100+0x30;	
		s[j++] =(addr%10000%1000%100)/10+0x30;	
		s[j++] =(addr%10000%1000%100%10)/1+0x30;	
	}		
	s[j] ='\0';
	for(j=0;j<strlen(s);j++)
	{
		Uart_Tx_Buf[i++] =s[j];
	}
	Uart_Tx_Buf[i++] ='\r';
	Uart_Tx_Buf[i++] ='\n';
	Uart_Tx_Buf[i] ='\0';	
}
/*****************************************************************************
* Function		: Handle_Gsm_URx_Dt
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Handle_Gsm_URx_Dt(void)
{
	uint32_t	i,j;
	j =0;
	for(i=0;i<Uart_Rx_Cnt;i++)
	{
		if((Gsm_URx_Dt[i] =='\r')&&(Gsm_URx_Dt[i+1]=='\n'))
		{
			i++;
			if(j !=0)
			{			
				Gsm_Rx_Dt[j]='\0';
#if UART_DEBUG ==1
				Uart_Rx_Print(j,Gsm_Rx_Dt);	
#endif
				Parse_Gsm_Rx_Dt(j);
				j=0;
			}
		}
		else
		{
			Gsm_Rx_Dt[j] =Gsm_URx_Dt[i];
			j++;
		}
	}
}
/*****************************************************************************
* Function		: Parse_Gsm_Rx_Dt
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Parse_Gsm_Rx_Dt(uint32_t	len)
{
	uint32_t i,j;uint8_t tmp[3];
	//=========================================================================
	if((isMSUB0(Gsm_Rx_Dt)== MATCH)&&(strlen(Gsm_Rx_Dt) <=9))					//Get MQTT Data
	{
		if(Gsm_Ver_ID ==GSM_VERSION_ID00)
		{
			Wt_GsmEvent_Ff(GSM_MQTT_GETDATA_EVT);			//
		}
		else if(Gsm_Ver_ID ==GSM_VERSION_ID05)
		{
			Wt_GsmEvent_Ff(GSM_MQTT_GETDATA0_EVT);		//		
		}
	}
	else if((isMSUB1(Gsm_Rx_Dt)== MATCH)&&(strlen(Gsm_Rx_Dt) <=9))		//Get MQTT Data
	{
		if(Gsm_Ver_ID ==GSM_VERSION_ID00)
		{
			Wt_GsmEvent_Ff(GSM_MQTT_GETDATA_EVT);			//
		}
		else if(Gsm_Ver_ID ==GSM_VERSION_ID05)
		{
			Wt_GsmEvent_Ff(GSM_MQTT_GETDATA1_EVT);		//		
		}
	}
	else if((isMSUB2(Gsm_Rx_Dt)== MATCH)&&(strlen(Gsm_Rx_Dt) <=9))		//Get MQTT Data
	{
		if(Gsm_Ver_ID ==GSM_VERSION_ID00)
		{
			Wt_GsmEvent_Ff(GSM_MQTT_GETDATA_EVT);			//
		}
		else if(Gsm_Ver_ID ==GSM_VERSION_ID05)
		{
			Wt_GsmEvent_Ff(GSM_MQTT_GETDATA2_EVT);		//		
		}
	}	
	else if((isMSUB3(Gsm_Rx_Dt)== MATCH)&&(strlen(Gsm_Rx_Dt) <=9))		//Get MQTT Data
	{
		if(Gsm_Ver_ID ==GSM_VERSION_ID00)
		{
			Wt_GsmEvent_Ff(GSM_MQTT_GETDATA_EVT);			//
		}
		else if(Gsm_Ver_ID ==GSM_VERSION_ID05)
		{
			Wt_GsmEvent_Ff(GSM_MQTT_GETDATA3_EVT);		//		
		}
	}		
	else if(isContentLength(Gsm_Rx_Dt)==MATCH)
	{
		Decode_HTTP_Server_Data();									//
	}
	else if(isHTTPCLOSED(Gsm_Rx_Dt)==MATCH)				//HTTP Close
	{
		FHTTP_Link =0;							
		if((Http_Update ==1)&&(Http_Update_StrAddr<65536))	//在升级状态中,HTTP关闭
		{
			if(FMQTT_NLink ==0)												//并且GSM没有关闭
			{
				Gsm_Tx_Ram_Init();											//继续升级
				Clear_Gsm_Ff();
				Wt_GsmEvent_Ff(GSM_HTTPS_INIT_EVT);			//HTTPS update event
			}
		}
	}
	else if((isCLOSED(Gsm_Rx_Dt)==MATCH)&&(strlen(Gsm_Rx_Dt) <=20))						//GSM Close
	{
		Gsm_Tx_Ram_Init();
		Clear_Gsm_Ff();
		Gsm_State = GSM_IDLE_STA;															
		Gsm_Tx_State =GSM_TX_IDLE;	
		if(++MQTT_Close_Cnt>3)										//连续3次MQTT丢失，关闭电源重启
		{
			Wt_GsmEvent_Ff(GSM_CLOSE_EVT);					//
		}
		else
		{
			Wt_GsmEvent_Ff(GSM_MQTT_STOP_EVT);			//MQTT Close
		}
	}
	else if((isRDY(Gsm_Rx_Dt)==MATCH)&&(strlen(Gsm_Rx_Dt) <=5))	//Gsm Module Ready
	{
		FGsm_Rdy =1;
		Clear_Gsm_Ff();														//Clear fifo
		Gsm_Tx_Ram_Init();												//Clear Tx Ram
		Gsm_Http_Ram_Init();											//
		FGsm_MQTT_Init_OK =0;											//
		FGsm_Init_OK =0;													//
		Gsm_ReTx_Process_Cnt =0;									//
		Gsm_State = GSM_IDLE_STA;									//Into Gsm Idle status
		Wt_GsmEvent_Ff(GSM_INIT_EVT);							//GSM Initial Event
		if(Sys_Power_On ==1)
		{
			FMQTT_NLink =1;	
		}
	}
	else if(isPINGRESP(Gsm_Rx_Dt)==MATCH)				//MQTT Link Lost
	{
		Gsm_Tx_Ram_Init();
		Clear_Gsm_Ff();
		Gsm_State = GSM_IDLE_STA;															
		Gsm_Tx_State =GSM_TX_IDLE;	
		Wt_GsmEvent_Ff(GSM_MQTT_STOP_EVT);			//MQTT Close	
	}
	
	switch(Gsm_Tx_State)
	{
		case	GSM_TX_AT_CMD:		if(isOK(Gsm_Rx_Dt)== MATCH) 	{Gsm_Txing =0;} break;	//AT OK
		case	GSM_TX_IRP_CMD:		if(isOK(Gsm_Rx_Dt)== MATCH)		{Gsm_Txing =0;}	break;	//IRP OK
		case	GSM_TX_CPIN_CMD:	if(isCPIN(Gsm_Rx_Dt)== MATCH)	{Gsm_Txing =0;} break;	//CPIN Ready
		case	GSM_TX_CREG_CMD:	if(isCREG(Gsm_Rx_Dt)== MATCH)	{Gsm_Txing =0;} break;	//CREG Ready
		case	GSM_TX_CGATT_CMD:	if(isCGATT(Gsm_Rx_Dt)== MATCH){Gsm_Txing =0;} break;	//CGATT Ready
//		case	GSM_TX_CSQ_CMD:		if(isOK(Gsm_Rx_Dt)== MATCH)		{Gsm_Txing =0;}	break;	//CSQ
		case	GSM_TX_CSTT_CMD:	if(isOK(Gsm_Rx_Dt)== MATCH)		{Gsm_Txing =0;}	break;	//
		case	GSM_TX_CIICR_CMD:	if(isOK(Gsm_Rx_Dt)== MATCH) 	{Gsm_Txing =0;}	break;	//CIICR OK
		case	GSM_TX_CIFSR_CMD:																												//得到IP 地址																
			if((isCIFSR(Gsm_Rx_Dt)==MATCH)||(isOK(Gsm_Rx_Dt)== MATCH))
			{
			
			}
			else
			{
				Gsm_Txing =0;
			} 
		break;	//CIPSR
		case  GSM_POWD_CMD:			if(isPOWD(Gsm_Rx_Dt)== MATCH)	{Gsm_Txing =0;}	break;
		case	GSM_CIPSTATUS_CMD: if(isOK(Gsm_Rx_Dt)== MATCH)	{Gsm_Txing =0;}	break;
		case	GSM_TX_ISL_CMD:																													//读取版本返回
		if(isGSMVER00(Gsm_Rx_Dt)==MATCH)
		{
			Gsm_Ver_ID = GSM_VERSION_ID00;
		}
		else if(isGSMVER05(Gsm_Rx_Dt)==MATCH)
		{
			Gsm_Ver_ID = GSM_VERSION_ID05;
		}
		if(isOK(Gsm_Rx_Dt)== MATCH)	{Gsm_Txing =0;}	
		break;
		case	GSM_TX_EGMR_CMD:																												//读取SN返回
			if((Gsm_Rx_Dt[0]=='+')&&(Gsm_Rx_Dt[1]=='E')&&(Gsm_Rx_Dt[2]=='G')&&(Gsm_Rx_Dt[3]=='M')&&(Gsm_Rx_Dt[4]=='R')&&(Gsm_Rx_Dt[5]==':'))
			{
				for(i=0;i<SN_ID_LEN;i++)
				{
					SN_ID[i] =Gsm_Rx_Dt[8+i];
				}
			}		
			if(isOK(Gsm_Rx_Dt)== MATCH)		{Gsm_Txing =0;}
		break;
		case	GSM_TX_ICCID_CMD:																												//读取ICCID返回
			if((Gsm_Rx_Dt[0]=='+')&&(Gsm_Rx_Dt[1]=='I')&&(Gsm_Rx_Dt[2]=='C')&&(Gsm_Rx_Dt[3]=='C')&&(Gsm_Rx_Dt[4]=='I')&&(Gsm_Rx_Dt[5]=='D')&&(Gsm_Rx_Dt[6]==':'))
			{
				for(i=0;i<20;i++)
				{
					ICCID_ID[i] =Gsm_Rx_Dt[8+i];
				}
			}		
			if(isOK(Gsm_Rx_Dt)== MATCH)		{Gsm_Txing =0;}
		break;			
		case GSM_TX_CGSN_CMD:																													//读取IMEI返回
			if((Gsm_Rx_Dt[0]>='0')&&(Gsm_Rx_Dt[0]<='9'))
			{
				for(i=0;i<15;i++)
				{
					IMEI_ID[i] =Gsm_Rx_Dt[i];
				}
			}
			if(isOK(Gsm_Rx_Dt)== MATCH)		{Gsm_Txing =0;}
		break;
		case	GSM_CNUM_CMD:									//NUM
			if((Gsm_Rx_Dt[0]>='0')&&(Gsm_Rx_Dt[0]<='9'))
			{
				for(i=0;i<11;i++)
				{
					Num_ID[i] =Gsm_Rx_Dt[i];
				}
			}
			if(isOK(Gsm_Rx_Dt)== MATCH)		{Gsm_Txing =0;}		
		break;
		case	GSM_CCLK_CMD:																														//读取CLK返回
			if((Gsm_Rx_Dt[0]=='+')&&(Gsm_Rx_Dt[1]=='C')&&(Gsm_Rx_Dt[2]=='C')&&(Gsm_Rx_Dt[3]=='L')&&(Gsm_Rx_Dt[4]=='K'))
			{
				Time_convert();
				Timestamp =Set_timestamp(IC_year,IC_mon,IC_day,IC_hour,IC_min,IC_sec);
			}			
			if(isOK(Gsm_Rx_Dt)== MATCH)		{Gsm_Txing =0;}	
		break;
		case	GSM_TX_CSQ_CMD:							//信号强度
			if((Gsm_Rx_Dt[0]=='+')&&(Gsm_Rx_Dt[1]=='C')&&(Gsm_Rx_Dt[2]=='S')&&(Gsm_Rx_Dt[3]=='Q'))
			{
				for(i=0;i<strlen(tmp);i++)
				{
					tmp[i] =0;
				}
				i=6; j=0;
				while(Gsm_Rx_Dt[i]!=',')
				{
					tmp[j++] =Gsm_Rx_Dt[i++];
				}
				tmp[j] ='\0';
				CSQ_Buf = (AsciitoInt(tmp) *100) /32;
			}			
			if(isOK(Gsm_Rx_Dt)== MATCH)		{Gsm_Txing =0;}				
		break;		
		//MQTT	
		case	GSM_TX_MCONFIG_CMD:		if(isOK(Gsm_Rx_Dt)== MATCH)					{Gsm_Txing =0;}	break;	
		case	GSM_TX_MIPSTART_CMD:	
			if(isCONNECT(Gsm_Rx_Dt)== MATCH) 		{Gsm_Txing =0;} 
			if(isALREADYCONNECT(Gsm_Rx_Dt)==MATCH) {Gsm_Txing =0;} 
			break;	//TCP/IP 连接OK
		case	GSM_TX_MCONNECT_CMD:	if( isCONNACK(Gsm_Rx_Dt) == MATCH) 	{Gsm_Txing =0;} break;	//MQTT Link OK			
		case	GSM_TX_MSUB_CMD:			if(isOK(Gsm_Rx_Dt)== MATCH)					{Gsm_Txing =0;} break;	//MSUB
		case	GSM_TX_MPUB_CMD:																																	//MQTT Pub
			if(Gsm_Ver_ID ==GSM_VERSION_ID00)
			{
				if(isPUBACK(Gsm_Rx_Dt)== MATCH)	
				{
					Gsm_Txing =0;
				}
			}
			else if(Gsm_Ver_ID ==GSM_VERSION_ID05)
			{
				if(isPUBID(Gsm_Rx_Dt)== MATCH)																												//保存当前pub id	
				{
					MQTT_PUB_ACK_ID =Gsm_Rx_Dt[9];
				}
				else if(isPUBACK(Gsm_Rx_Dt)== MATCH)																									//收到pub ack			
				{
					if(Gsm_Rx_Dt[10] ==MQTT_PUB_ACK_ID)																									//判断id是否相等
					{
						MQTT_PUB_ACK_ID =0;
						Gsm_Txing =0;
					}
				}				
			}
		break;		
		case	GSM_TX_MSGGET_CMD:
		case	GSM_TX_MSGGET0_CMD:
		case	GSM_TX_MSGGET1_CMD:
		case	GSM_TX_MSGGET2_CMD:
		case	GSM_TX_MSGGET3_CMD:			
			if(isMSUB(Gsm_Rx_Dt)==MATCH) {Decode_MQTT_Server_Data();}
			if(isOK(Gsm_Rx_Dt)== MATCH)	 {Gsm_Txing =0;}
		break;
		case	GSM_TX_MIPCLOSE_CMD:	if(isOK(Gsm_Rx_Dt)== MATCH)					{Gsm_Txing =0;} break;	//MSUB
		case	GSM_TX_MIPDISCONNECT_CMD:	 if(isOK(Gsm_Rx_Dt)== MATCH)					{Gsm_Txing =0;} break;	//MSUB
		case	GSM_TX_MQTTSTATU_CMD:																											//读取MQTT连接状态返回
			if(isMQTTSTATU(Gsm_Rx_Dt)== MATCH)
			{
				if(Gsm_Rx_Dt[11]!='1')									//MQTT Link Lost
				{
					Gsm_Tx_Ram_Init();
					Clear_Gsm_Ff();
					Gsm_State = GSM_IDLE_STA;															
					Gsm_Tx_State =GSM_TX_IDLE;	
					Wt_GsmEvent_Ff(GSM_MQTT_STOP_EVT);			//MQTT Close
				}
			}
			if(isOK(Gsm_Rx_Dt)== MATCH)					
			{Gsm_Txing =0;} 
		break;
		case	GSM_TX_MQTTTMOUT_CMD:	if(isOK(Gsm_Rx_Dt)== MATCH)					{Gsm_Txing =0;} break;	//
		//HTTP 
		case	GSM_HTTPSSL1_CMD:							if(isOK(Gsm_Rx_Dt)== MATCH)	{Gsm_Txing =0;} break;
		case	GSM_HTTPSSL0_CMD:							if(isOK(Gsm_Rx_Dt)== MATCH)	{Gsm_Txing =0;} break;				
		case	GSM_HTTPPARA_URL_CMD: 				if(isOK(Gsm_Rx_Dt)== MATCH)	{Gsm_Txing =0;} break;
		case	GSM_HTTPPARA_PORT_CMD: 				if(isOK(Gsm_Rx_Dt)== MATCH)	{Gsm_Txing =0;}	break;
		case	GSM_HTTPSETUP_CMD:						if(isOK(Gsm_Rx_Dt)== MATCH) {Gsm_Txing =0;FHTTP_Link =1;} break;	
		case	GSM_HTTPACTION0_CMD: 					if(isOK(Gsm_Rx_Dt)== MATCH)	{Gsm_Txing =0;} break;
		case	GSM_HTTP_CLOSE_CMD:						if(isCLOSEOK(Gsm_Rx_Dt)== MATCH)	{Gsm_Txing =0;FHTTP_Link =0;} break;
		//HTTPS
		case	GSM_HTTPSPARA_URL_CMD: 				if(isOK(Gsm_Rx_Dt)== MATCH)	{Gsm_Txing =0;}	break;
		case	GSM_HTTPSPARA_PORT_CMD: 			if(isOK(Gsm_Rx_Dt)== MATCH)	{Gsm_Txing =0;}	break;
		case	GSM_HTTPSBREAK_CMD: 					if(isOK(Gsm_Rx_Dt)== MATCH)	{Gsm_Txing =0;}	break;
		case	GSM_HTTPSBREAKEND_CMD: 				if(isOK(Gsm_Rx_Dt)== MATCH)	{Gsm_Txing =0;}	break;
		case	GSM_HTTPSPARA_CONNECT1_CMD: 	if(isOK(Gsm_Rx_Dt)== MATCH)	{Gsm_Txing =0;} break;	
				
		default:	break;
	}
}
/*****************************************************************************
* Function		: Gsm_Activate_Chk
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Gsm_Activate_Chk(void)
{
	uint8_t i,j;uint32_t k;
	//=========================================================================
	Device_ID[0] =DEVICE_ID0;											//Initial ID
	Device_ID[1] =DEVICE_ID1;	
	
	k= ((Rolling_Code>>20)&0x0f) *100000+((Rolling_Code>>16)&0x0f) *10000;
	k =k+((Rolling_Code>>12)&0x0f) *1000 +((Rolling_Code>>8)&0x0f) *100;
	k =k+((Rolling_Code>>4)&0x0f)  *10+((Rolling_Code>>0)&0x0f)  *1;
	
	Device_ID[2] = InttoAscii((k>>16)&0xf);
	Device_ID[3] = InttoAscii((k>>12)&0xf);
	Device_ID[4] = InttoAscii((k>>8)&0xf);
	Device_ID[5] = InttoAscii((k>>4)&0xf);
	Device_ID[6] = InttoAscii((k>>0)&0xf);	
	
	j =7;
	for(i=0;i<15;i++)
	{
		Device_ID[j++] =IMEI_ID[i];
	}
	Device_ID[j] ='\0';
	//=========================================================================
	Rom_Version[0] =((Rom_VerTab>>0)&0xff)+0x30;
	Rom_Version[1] ='.';
	Rom_Version[2] =((Rom_VerTab>>8)&0xff)+0x30;
	Rom_Version[3] ='.';
	Rom_Version[4] =((Rom_VerTab>>16)&0xff)+0x30;
	Rom_Version[5] ='\0';
	//=========================================================================
	//判断是否需要激活
	j =0;
	for(i=0;i<5;i++)
	{
		if(ICCID_ID_Tab[i] !=(ICCID_ID[j]|(ICCID_ID[j+1]<<8)|(ICCID_ID[j+2]<<16)|(ICCID_ID[j+3]<<24)))
		{
			Wt_GsmEvent_Ff(GSM_HTTP_ACTIVE_EVT);							//未激活,HTTP连接						
			return;
		}
		j=j+4;
	}
	Wt_GsmEvent_Ff(GSM_MQTT_INIT_EVT);										//已经激活,MQTT 连接
}
/*****************************************************************************
* Function		: MQTT_Link_Init
* Description	: PassWord: md5(xd+devid+sn +iccId) 的8bit-24bit
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	MQTT_Link_Init(void)
{
	unsigned char s[200],md5[32];
	uint32_t i,j;
	//==========================================================================
	SecKey[0] =SecKey_Tab[0]&0xff;								//Initia seckey
	SecKey[1] =(SecKey_Tab[0]>>8)&0xff;	
	SecKey[2] =(SecKey_Tab[0]>>16)&0xff;	
	SecKey[3] =(SecKey_Tab[0]>>24)&0xff;	
	SecKey[4] =SecKey_Tab[1]&0xff;
	SecKey[5] =(SecKey_Tab[1]>>8)&0xff;	
	SecKey[6] =(SecKey_Tab[1]>>16)&0xff;	
	SecKey[7] =(SecKey_Tab[1]>>24)&0xff;	
	SecKey[8] =SecKey_Tab[2]&0xff;
	SecKey[9] =(SecKey_Tab[2]>>8)&0xff;	
	SecKey[10] =(SecKey_Tab[2]>>16)&0xff;	
	SecKey[11] =(SecKey_Tab[2]>>24)&0xff;	
	SecKey[12] =SecKey_Tab[3]&0xff;
	SecKey[13] =(SecKey_Tab[3]>>8)&0xff;	
	SecKey[14] =(SecKey_Tab[3]>>16)&0xff;	
	SecKey[15] =(SecKey_Tab[3]>>24)&0xff;	
	SecKey[16] ='\0';
	//==========================================================================
	j =0;																					//Initial Password
	s[j++] ='x';	s[j++] ='d';
	for(i=0;i<22;i++)
	{
		s[j++] =Device_ID[i];
	}
	for(i=0;i<SN_ID_LEN;i++)											//SN
	{																							//
		s[j++] =SN_ID[i];														//
	}																							//
	for(i=0;i<ICCID_ID_LEN;i++)										//ICCID
	{																							//
		s[j++] =ICCID_ID[i];												//
	}																							//
	s[j] ='\0';																		//		
	//==========================================================================
	get_md5_32_str(s,strlen((char*)s),md5);							//md5
	j=8;
	for(i=0;i<PASSWORD_LEN;i++)
	{
		PassWord[i] =md5[j++];
	}		
	//==========================================================================
	if(Gsm_Ver_ID ==GSM_VERSION_ID00)
	{
		Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_Init00_Tab;		//MQTT 建立连接
	}
	else if(Gsm_Ver_ID ==GSM_VERSION_ID05)
	{
		Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_Init05_Tab;		//MQTT 建立连接	
	}
}
/*****************************************************************************
* Function		: Time_convert
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Time_convert(void)
{
	IC_year =2000+(Gsm_Rx_Dt[8]-0x30)*10+(Gsm_Rx_Dt[9]-0x30);
	IC_mon =(Gsm_Rx_Dt[11]-0x30)*10+(Gsm_Rx_Dt[12]-0x30);
	IC_day =(Gsm_Rx_Dt[14]-0x30)*10+(Gsm_Rx_Dt[15]-0x30);
	IC_hour =(Gsm_Rx_Dt[17]-0x30)*10+(Gsm_Rx_Dt[18]-0x30);
	IC_min =(Gsm_Rx_Dt[20]-0x30)*10+(Gsm_Rx_Dt[21]-0x30);
	IC_sec =(Gsm_Rx_Dt[23]-0x30)*10+(Gsm_Rx_Dt[24]-0x30);
}
/*****************************************************************************
* Function		: Set_timestamp
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
uint32_t	Set_timestamp(uint16_t year,uint8_t mon,uint8_t day,uint8_t hour,uint8_t min,uint8_t sec)
{
	uint32_t tm;
	if (0 >= (int) (mon -= 2)) {    /* 1..12 -> 11,12,1..10 */
		mon += 12;      /* Puts Feb last since it has leap day */
		year -= 1;
	}

	tm = ((((unsigned long long) (year/4 - year/100 + year/400 + 367*mon/12 + day) +
								 year*365 - 719499
								)*24 + hour /* now have hours */
							 )*60 + min /* now have minutes */
							)*60 + sec; /* finally seconds */	
	return tm;
}	
/*****************************************************************************
* Function		: Init_Gsm_Report
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Init_Gsm_Report(void)
{
	FGsm_MQTT_Tx_OK =0;	FGsm_HTTP_Rx_OK =0;
	switch(Gsm_Event &0xff)
	{	//=======================================================================
		case	GSM_INIT_EVT:																			//Initial Gsm Event
			FGsm_Init_OK =0;																			//
			Gsm_Tx_Tab = (uint8_t *)GSM_Init_Tab;									//
		break;
		case GSM_CLOSE_EVT:																			//Hardware Close Gsm Module
			FGsm_Init_OK =0;																			//
			FGsm_MQTT_Init_OK =0;																	//
			Gsm_Timer_Tx =0;																			//
			Gsm_MQTT_Tmlink =0;																		//
			FMQTT_NLink =1;																				//
			Gsm_Node_Rpt_Step =0;																	//
			Timestamp_NLink =Set_timestamp(IC_year,IC_mon,IC_day,IC_hour,IC_min,IC_sec);
			Sys_State = SYS_NONET_STA;														//
			Gsm_Tx_Tab = (uint8_t *)GSM_POWD_Tab;									//
			Close_N_M(NODE_TOTAL_NUM);														//关闭所有电池铁
			LED_ALLFlash_Init(NODE_TOTAL_NUM,TM_LED_FLASH_NONET);//断网，全部LED闪烁
		break;			
		//=======================================================================
		case	GSM_MQTT_INIT_EVT:																//Initial MQTT Event
			FGsm_MQTT_Init_OK =0;																	//
			MQTT_Link_Init();																			//													
		break;
		case	GSM_MQTT_TIMER_EVT:																//定时上报
			Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_PUB_Tab;								
			Gsm_MQTT_Rept_Mode =MQTT_REPORT_MODE_TIMER;		
		break;
		case	GSM_MQTT_PWRON_EVT:																//开机上报
			Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_PUB_Tab;	
			Gsm_MQTT_Rept_Mode =MQTT_REPORT_MODE_PWRON;						
		break;
		case	GSM_MQTT_RLINK_EVT:																//断网重连
			Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_PUB_Tab;	
			Gsm_MQTT_Rept_Mode =MQTT_REPORT_MODE_RLINK;			
		break;	
		case	GSM_MQTT_OPENREQ_EVT:															//仓位打开申请上报
			Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_PUB_Tab;	
			Gsm_MQTT_Rept_Mode =MQTT_REPORT_MODE_OPENREQ;							
			Gsm_Rept_Node =(Gsm_Event>>8)&0x07;
		break;
		case	GSM_MQTT_CLOSE_EVT:																//仓位关闭上报
			Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_PUB_Tab;	
			Gsm_MQTT_Rept_Mode =MQTT_REPORT_MODE_NODECLOSE;				
			Gsm_Rept_Node =(Gsm_Event>>8)&0x07;
		break;
		case	GSM_MQTT_OUT_EVT:																	//仓位取走上报	
			Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_PUB_Tab;	
			Gsm_MQTT_Rept_Mode =MQTT_REPORT_MODE_NODEOUT;							
			Gsm_Rept_Node =(Gsm_Event>>8)&0x07;
		break;
		case	GSM_MQTT_ONFAIL_EVT:															//仓位打开失败
			Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_PUB_Tab;	
			Gsm_MQTT_Rept_Mode =MQTT_REPORT_MODE_OPENFAIL;							
			Gsm_Rept_Node =(Gsm_Event>>8)&0x07;			
		break;
		case	GSM_MQTT_ONEREP_EVT:															//上报单个节点信息
			Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_PUB_Tab;	
			Gsm_MQTT_Rept_Mode =MQTT_REPORT_MODE_ONEREP;							
			Gsm_Rept_Node =(Gsm_Event>>8)&0x07;						
		break;
		case	GSM_MQTT_GETDATA_EVT:
			Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_Get_Tab;							//有收到数据，发送读命令
		break;	
		case	GSM_MQTT_GETDATA0_EVT:
			Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_Get0_Tab;						//有收到数据，发送读命令
		break;	
		case	GSM_MQTT_GETDATA1_EVT:
			Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_Get1_Tab;						//有收到数据，发送读命令
		break;	
		case	GSM_MQTT_GETDATA2_EVT:
			Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_Get2_Tab;						//有收到数据，发送读命令
		break;	
		case	GSM_MQTT_GETDATA3_EVT:
			Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_Get3_Tab;						//有收到数据，发送读命令
		break;			
		case	GSM_MQTT_STOP_EVT:																//MQTT Close
			FGsm_MQTT_Init_OK =0;																	//
			Gsm_Timer_Tx =0;																			//
			Gsm_MQTT_Tmlink =0;																		//
			Gsm_Node_Rpt_Step =0;																	//
			FMQTT_NLink =1;																				//
			Timestamp_NLink =Set_timestamp(IC_year,IC_mon,IC_day,IC_hour,IC_min,IC_sec);
			Sys_State = SYS_NONET_STA;														//
			Close_N_M(NODE_TOTAL_NUM);														//关闭所有电池铁
			LED_ALLFlash_Init(NODE_TOTAL_NUM,TM_LED_FLASH_NONET);	//断网，全部LED闪烁
			Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_Stop_Tab;						//					
		break;
		case	GSM_MQTT_STATUS_EVT:															//定时检测MQTT连接状态
			if(Gsm_Ver_ID ==GSM_VERSION_ID00)
			{
				Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_PUB_Tab;	
				Gsm_MQTT_Rept_Mode =MQTT_REPORT_MODE_PUBLINK;
			}
			else if(Gsm_Ver_ID ==GSM_VERSION_ID05)
			{
				Gsm_Tx_Tab = (uint8_t *)GSM_MQTT_STATU_Tab;	
			}
		break;
		//=======================================================================
		case	GSM_HTTP_ACTIVE_EVT:															//HTTP 激活
			Gsm_Tx_Tab = (uint8_t *)GSM_HTTP_Init_Tab;						
		break;
		case GSM_HTTP_OPENREQ_EVT:															//HTTP 请求打开仓位
			Gsm_Tx_Tab = (uint8_t *)GSM_HTTP_Init_Tab;						
			Gsm_Rept_Node =(Gsm_Event>>8)&0x07;
		break;
		case	GSM_HTTP_UPDATE_STATUS_EVT:												//HTTP 上报更新状态
			Gsm_Tx_Tab = (uint8_t *)GSM_HTTP_Init_Tab;						
		break;
		case	GSM_HTTPS_INIT_EVT:																//HTTPS 更新建立
			Gsm_Tx_Tab = (uint8_t *)GSM_HTTPS_Init_Tab;						
		break;		
		case	GSM_HTTPS_GETDATA_EVT:														//得到HTTPS更新数据
			Gsm_Tx_Tab = (uint8_t *)GSM_HTTP_GET_Update_Data_Tab;	
			Https_GetData_OK =0;																	//Clear Flag
		break;
		//=======================================================================
		default:	return;
	}
	Gsm_Tab_pt =0;	Gsm_Txing =0;	
	Gsm_Tx_CntOut_Tmp =0;	
	Http_Req_Tmout =0; Http_Req_Cnt =0;	
	FGsm_Tx_Enb =1;
	Sys_GsmTask_Step =2;																	//	
}
/*****************************************************************************
* Function		: Gsm_Report_Over
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Gsm_Report_Over(void)
{ 
	uint8_t	cnt;
	switch(Gsm_Event&0xff)
	{//=======================================================================
		case	GSM_INIT_EVT:												//Gsm Initial Over
			FGsm_Init_OK =1;
			Gsm_Activate_Chk();											//检测是否激活	
		break;
		case GSM_CLOSE_EVT:												//soft close power module over
			Gsm_State =GSM_PWR_STA;									//into hardware close	module	
			Gsm_Pwr_State =GSM_VOL_OFF_STA;	
		break;		
	 //=======================================================================
		case	GSM_MQTT_INIT_EVT:									//MQTT Initial Over
			FGsm_MQTT_Init_OK =1;										//
			MQTT_Close_Cnt =0;											//
			Gsm_Timer_Tx =GSM_TIMER_TX_TIME;				//初始化定时发送时间
			if(Gsm_Ver_ID ==GSM_VERSION_ID00)				//初始化定时检测状态时间
			{
				Gsm_MQTT_Tmlink =GSM_TM_MQTT_STATUS00;		
			}
			else if(Gsm_Ver_ID ==GSM_VERSION_ID05)
			{
				Gsm_MQTT_Tmlink =GSM_TM_MQTT_STATUS05;					
			}
			if(Http_Update ==0) 										//没有在https更新状态
			{
				if(Sys_Power_On ==0)
				{
					Wt_GsmEvent_Ff(GSM_MQTT_PWRON_EVT);	//开机上报事件							
				}
				else
				{
					Sys_State =SYS_NORMAL_STA;					//
					Init_M_S_LED();											//恢复M/S/LED状态
					Wt_GsmEvent_Ff(GSM_MQTT_RLINK_EVT);	//断网重连上报			
				}
			}			
			else
			{
				Wt_GsmEvent_Ff(GSM_HTTPS_INIT_EVT);		//HTTPS update event
			}
			FMQTT_NLink =0;
		break;	
		case	GSM_MQTT_TIMER_EVT:										//Timer Report Over
			if(++Gsm_Node_Rpt_Step>=(NODE_TOTAL_NUM/2))
			{
				Gsm_Node_Rpt_Step =0;
				Gsm_Timer_Tx =GSM_TIMER_TX_TIME;				//
			}
			else
			{
				Sys_GsmTask_Step =1;										//继续发送
			}					
		break;
		case GSM_MQTT_PWRON_EVT:										//Power On Report Over
			if(++Gsm_Node_Rpt_Step>=(NODE_TOTAL_NUM/2))	//4次上报完成
			{
				Gsm_Node_Rpt_Step =0;
				Initial_Sys_Normal();										//进入正常工作状态
			}
			else
			{
				Sys_GsmTask_Step =1;										//继续发送
			}			
		break;
		case	GSM_MQTT_RLINK_EVT:										//断网重连完成
			if(++Gsm_Node_Rpt_Step>=(NODE_TOTAL_NUM/2))
			{
				Gsm_Node_Rpt_Step =0;										//
				Timestamp_NLink =0;											//
			}
			else
			{
				Sys_GsmTask_Step =1;										//继续发送
			}				
		break;
		case 	GSM_MQTT_OPENREQ_EVT:			
		//	Wt_NodeEvent_Ff((Gsm_Event&0x0700)|NODE_ON_EVT);						//节点打开
		break;	
		case	GSM_MQTT_CLOSE_EVT:				break;
		case	GSM_MQTT_OUT_EVT:					break;			
		case	GSM_MQTT_ONFAIL_EVT:			break;
		case	GSM_MQTT_ONEREP_EVT:			break;
		case	GSM_MQTT_GETDATA_EVT:			break;
		case	GSM_MQTT_STOP_EVT:
			Wt_GsmEvent_Ff(GSM_MQTT_INIT_EVT);				//
		break;
		case	GSM_MQTT_STATUS_EVT: 
			if(Gsm_Ver_ID ==GSM_VERSION_ID00)					//初始化定时检测状态时间
			{
				Gsm_MQTT_Tmlink =GSM_TM_MQTT_STATUS00;		
			}
			else if(Gsm_Ver_ID ==GSM_VERSION_ID05)
			{
				Gsm_MQTT_Tmlink =GSM_TM_MQTT_STATUS05;					
			}
		break;
		//=======================================================================
		case GSM_HTTP_ACTIVE_EVT:
			Http_Req_Tmout =0;	Http_Req_Cnt =0; Http_Req_Cnt_Tmp=0;
			Sys_Save_Flash();														//保存入flash
			if(FGsm_MQTT_Init_OK ==0)	Wt_GsmEvent_Ff(GSM_MQTT_INIT_EVT);			//激活后，MTQQ连接
		break;
		case	GSM_HTTP_OPENREQ_EVT:
			Http_Req_Tmout =0;	Http_Req_Cnt =0; Http_Req_Cnt_Tmp=0;
		break;
		case	GSM_HTTP_UPDATE_STATUS_EVT:
			Http_Req_Tmout =0;	Http_Req_Cnt =0; Http_Req_Cnt_Tmp=0;
			if(Http_Update_Status ==HTTP_UPDATE_STATUS_UPDATING)
			{
				Wt_GsmEvent_Ff(GSM_HTTPS_INIT_EVT);					//开始HTTPS update event			
			}
			else if(Http_Update_Status ==HTTP_UPDATE_STATUS_FAIL)
			{
				Http_Update =0;
				Sys_State = SYS_NORMAL_STA;									//
				Init_M_S_LED();															//更新失败，恢复M/S/LED状态
			}
			Http_Update_Status =0;
		break;
		case GSM_HTTPS_INIT_EVT:
			Sys_State = SYS_UPDATE_STA;								//进入更新状态
			if(Http_Update ==0)
			{
				Http_Update =1;
				Http_Update_Status =0;
				Http_Update_Tmout =(20*60*100);						//更新最大时间
				Http_Update_StrAddr =0;											
				Http_Update_StpAddr =255;	
				Flash_Wt_Cmd(0x06);												//Enable Write
				UT_DelayNms(1);														//
				Flash_Block_Erase();											//擦除flash	
				UT_DelayNms(1000);												//Delay 2S
				UT_DelayNms(1000);												//
				Close_N_M(NODE_TOTAL_NUM);								//关闭所有电池铁
				Close_N_S(NODE_TOTAL_NUM);								//关闭所有充电
			}			
			LED_Run_Init();															//跑马模式
			Wt_GsmEvent_Ff(GSM_HTTPS_GETDATA_EVT);			//开始得到HTTPS更新数据
		break;	
		case	GSM_HTTPS_GETDATA_EVT:
			if(Http_Update_StrAddr>65535)
			{
				Http_Update_Tmout =0;
				UT_DelayNms(200);
				if((CheckSum!= CheckSum_Buf1)||(Chk_Extern_Flash_CheckSum()!= CheckSum_Buf2)) //收到的数据错误/写入flash的数据错误 
				{																
					Http_Update_Status =HTTP_UPDATE_STATUS_FAIL;
					Wt_GsmEvent_Ff(GSM_HTTP_UPDATE_STATUS_EVT);			//更新失败上报
					break;			
				}	
				NVIC_DisableIRQ(USART1_IRQn);			//Disable UART Interrupt
				NVIC_DisableIRQ(CT16B1_IRQn);			//Disable CT16B1 Interrupt
				
				LED_Status =0;
				LED1 =LED_OFF;	LED2 =LED_OFF;	LED3 =LED_OFF;
				LED4 =LED_OFF;	LED5 =LED_OFF;	LED6 =LED_OFF;
				//
				GSM_PWR_OFF
//					UT_DelayNms(3000);										//
//					UT_DelayNms(3000);										//
				UT_DelayNms(3000);										//					
				FLASH_EraseSector(0xFC00);				//擦除最后一个page
				UT_DelayNms(200);
				*(volatile unsigned long*)(0x40060024) =0xa5a50000;	//map boot
				NVIC_SystemReset();
			}
			else
			{
				Wt_GsmEvent_Ff(GSM_HTTPS_GETDATA_EVT);			//继续取更新数据
			}		
		break;
		default: break;
	}
}
/*****************************************************************************
* Function		: Decode_HTTP_Server_Data
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	Decode_HTTP_Server_Data(void)
{
	uint32_t i,j,k,l;uint16_t data_len; char s[5];uint8_t data[256];
	
	j=0;
	for(i=0;i<strlen(Gsm_Rx_Dt);i++)					//从content lendth中找出数据长度
	{
		if((Gsm_Rx_Dt[i]>='0')&&(Gsm_Rx_Dt[i]<='9'))
		{
			s[j++]=Gsm_Rx_Dt[i];
		}
	}
	s[j] ='\0';
	data_len =AsciitoInt(s);													//acsii转换为整形
	//=========================================================================
	for(i=0;i<Uart_Rx_Cnt;i++)									  		//有效数据
	{
		if((Gsm_URx_Dt[i]=='\r')&&(Gsm_URx_Dt[i+1]=='\n')&&(Gsm_URx_Dt[i+2]=='\r')&&(Gsm_URx_Dt[i+3]=='\n'))	
		{
			j =0; 
			for(l=0;l<data_len;l++)															
			{
				data[j++]=Gsm_URx_Dt[i+4+l];
			}
			data[j] ='\0';
			break;
		}			
	}
	//=========================================================================
	switch(Gsm_Event &0xff)
	{
		case	GSM_HTTP_ACTIVE_EVT:
			for(i=0;i<strlen(data);i++)
			{
				if((data[i]=='\"')&&(data[i+1]=='s')&&(data[i+2]=='e')&&(data[i+3]=='c')&&(data[i+4]=='K')&&(data[i+5]=='e')&&(data[i+6]=='y'))
				{
					for(j=0;j<16;j++)
					{
						SecKey[j] =data[i+10+j];							//保存密匙
					}
					FGsm_HTTP_Rx_OK =1;													//
					return;
				}
			}					
		break;
		case GSM_HTTP_OPENREQ_EVT:
			for(i=0;i<strlen(data);i++)
			{
				if((data[i]=='\"')&&(data[i+1]=='o')&&(data[i+2]=='p')&&(data[i+3]=='e')&&(data[i+4]=='n'))
				{
					if((data[i+7]=='t')&&(data[i+8]=='r')&&(data[i+9]=='u')&&(data[i+10]=='e'))
					{
						FGsm_HTTP_Rx_OK =1;													//
						if(Http_Req_Tmout >0)												//15s 内收到
						{
							Wt_NodeEvent_Ff((Gsm_Event&0x0700)|NODE_ON_EVT);						//节点打开
						}
						else
						{
							Wt_NodeEvent_Ff((Gsm_Event&0x0700)|NODE_ONFAIL_EVT);				//节点打开失败
							LED_Flash_Init(((Gsm_Event>>8)&0x07),TM_ONFAIL_FLASH,CNT_ONFIAL_FLASH);//仓位打开失败闪烁10次
						}
						Http_Req_Tmout =0;
						Http_Req_Cnt =0;
						Http_Req_Cnt_Tmp =0;
					}
					else
					{
						FGsm_HTTP_Rx_OK =1;													//
						Http_Req_Tmout =0;
						Http_Req_Cnt =0;
						Http_Req_Cnt_Tmp =0;
						Wt_NodeEvent_Ff((Gsm_Event&0x0700)|NODE_ONFAIL_EVT);					//节点打开失败
						LED_Flash_Init(((Gsm_Event>>8)&0x07),TM_ONFAIL_FLASH,CNT_ONFIAL_FLASH);//仓位打开失败闪烁10次
					}
					return;
				}
			}		
		break;
		case	GSM_HTTP_UPDATE_STATUS_EVT:	
			FGsm_HTTP_Rx_OK =1;
		break;
		case	GSM_HTTPS_INIT_EVT:	break;
		case GSM_HTTPS_GETDATA_EVT:
			FGsm_HTTP_Rx_OK =1;															//Https Get Over
			if(data_len !=256) 
			{
				Https_GetData_OK =0;													//Get Data OK
			}
			else
			{
				for(i=0;i<256;i=i+2)														//原始checksum 
				{
					CheckSum_Buf1 =(data[i]|(data[i+1]<<8))+CheckSum_Buf1;
				}	
				if(Http_Update_StrAddr==(65535-255))						//保存最后256必须数据
				{
					j =0;
					for(i=0;i<10;i++)															//
					{
						data[j]= (*(uint32_t*)(0xFF00+j))&0xff;
						data[j+1]= ((*(uint32_t*)(0xFF00+j))>>8)&0xff;
						data[j+2]=((*(uint32_t*)(0xFF00+j))>>16)&0xff;
						data[j+3]=((*(uint32_t*)(0xFF00+j))>>24)&0xff;
						j =j+4;
					}
				}
				for(i=0;i<256;i=i+2)														//更新后checksum
				{
					CheckSum_Buf2 =(data[i]|(data[i+1]<<8))+CheckSum_Buf2;
				}			
						
				Https_GetData_OK =1;														//Get Data OK
				Flash_Wt_Cmd(0x06);															//Write extern Flash
				UT_DelayNms(1);
				Flash_Wt_NByte(0x02,Http_Update_StrAddr,data,256);
//				UT_DelayNms(100);	
				Http_Update_StrAddr +=256;
				Http_Update_StpAddr +=256;
				break;
			}
		break;
		default: break;
	}
}
