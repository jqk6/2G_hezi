#ifndef __GSM_TASK_MACHINE_H
#define __GSM_TASK_MACHINE_H
/*_____ I N C L U D E S ____________________________________________________*/
#include 	"stdint.h"
/*_____ D E F I N I T I O N S ______________________________________________*/
#define	ID_DEBUG	1
#define	SEK_DEBUG	0
#define	NET_DEBUG	1

#define MATCH		1
#define NO_MATCH	0

#define	DEVICE_ID0		'b'								//底座ID头码
#define	DEVICE_ID1		'1'
#define	B1_PROTOCOL_VER_0	'3'						//底座MQTT版本
#define	B1_PROTOCOL_VER_1	'.'
#define	B1_PROTOCOL_VER_2	'0'
#define	B1_HTTP_VER_0	'2'								//底座HTTP版本
#define	B1_HTTP_VER_1	'.'
#define	B1_HTTP_VER_2	'0'

#define	MQTT_PROTOCOL_TYPE_UP	4

#define	MQTT_REPORT_MODE_TIMER			1
#define	MQTT_REPORT_MODE_PWRON			2
#define	MQTT_REPORT_MODE_RLINK			3
#define	MQTT_REPORT_MODE_OPENREQ		4
#define	MQTT_REPORT_MODE_NODEOUT		5
#define	MQTT_REPORT_MODE_NODECLOSE	6
#define	MQTT_REPORT_MODE_OPENFAIL		7
#define	MQTT_REPORT_MODE_ONEREP			8
#define	MQTT_REPORT_MODE_PUBLINK		9
//=========================================================================
#define	GSM_IDLE_STA		0
#define	GSM_PWR_STA 		1
#define	GSM_TX_STA			2
//=========================================================================
#define	GSM_VOL_ON_STA  0
#define	GSM_VOL_STABLE_STA	1
#define	GSM_PWR_ON_STA	2
#define	GSM_PWR_IDLE_STA	3
#define	GSM_PWR_OFF_STA	4
#define	GSM_VOL_OFF_STA	5
//=========================================================================
#define	GSM_TX_IDLE				0
#define	GSM_TX_AT_CMD			1
#define	GSM_TX_IRP_CMD		2
#define	GSM_TX_CPIN_CMD		3
#define	GSM_TX_CREG_CMD		4
#define	GSM_TX_CGATT_CMD	5
#define	GSM_TX_CSTT_CMD		6
#define	GSM_TX_CIICR_CMD	7
#define	GSM_TX_CSQ_CMD		8
#define	GSM_TX_CIFSR_CMD	9
#define	GSM_TX_CGSN_CMD		10
#define	GSM_TX_EGMR_CMD		11
#define	GSM_TX_ICCID_CMD	12
#define	GSM_CCLK_CMD			13
#define	GSM_CNUM_CMD			14
#define	GSM_POWD_CMD			15
#define	GSM_CIPSTATUS_CMD	16
#define	GSM_TX_ISL_CMD		17

#define	GSM_TX_MIPSTART_CMD	30
#define	GSM_TX_MCONNECT_CMD	31
#define	GSM_TX_MCONFIG_CMD	32
#define	GSM_TX_MSUB_CMD			33
#define	GSM_TX_MSGGET_CMD		34
#define	GSM_TX_MPUB_CMD			35
#define	GSM_TX_MIPCLOSE_CMD	36
#define	GSM_TX_MIPDISCONNECT_CMD	37
#define	GSM_TX_MSGGET0_CMD	38
#define	GSM_TX_MSGGET1_CMD	39
#define	GSM_TX_MSGGET2_CMD	40
#define	GSM_TX_MSGGET3_CMD	41
#define	GSM_TX_MQTTSTATU_CMD	42
#define	GSM_TX_MQTTTMOUT_CMD	43

#define	GSM_HTTPSSL1_CMD		50
#define	GSM_HTTPSSL0_CMD		51
#define	GSM_HTTPPARA_URL_CMD	52
#define	GSM_HTTPPARA_PORT_CMD	53
#define	GSM_HTTPSETUP_CMD		54
#define	GSM_HTTPACTION0_CMD	55
#define	GSM_HTTPSBREAK_CMD		56
#define	GSM_HTTPSBREAKEND_CMD	57
#define	GSM_HTTPSPARA_URL_CMD	58
#define	GSM_HTTPSPARA_PORT_CMD	59
#define	GSM_HTTPSPARA_CONNECT0_CMD		60
#define	GSM_HTTPSPARA_CONNECT1_CMD		61
#define	GSM_HTTP_CLOSE_CMD				62


#define	GSM_TX_OVER_CMD		255

#define	GSM_EVT_FF_LEN				20
#define	GSM_NO_EVT						0
//GSM Event
#define	GSM_INIT_EVT					1			
#define	GSM_CLOSE_EVT					2
#define	GSM_MQTT_STOP_EVT			3
//MQTT Event
#define	GSM_MQTT_TIMER_EVT		10
#define	GSM_MQTT_PWRON_EVT		11			//开机上报事件 
#define	GSM_MQTT_RLINK_EVT		12			//断网上报事件
#define	GSM_MQTT_OPENREQ_EVT	13			//仓位打开上报事件
#define	GSM_MQTT_OUT_EVT			14			//仓位取走上报事件
#define	GSM_MQTT_CLOSE_EVT		15			//仓位关闭上报事件
#define	GSM_MQTT_INIT_EVT			16
#define	GSM_MQTT_GETDATA_EVT	17
#define	GSM_MQTT_ONFAIL_EVT		18			//仓位打开失败上报事件
#define	GSM_MQTT_ONEREP_EVT		19			//上报单个节点信息事件
#define	GSM_MQTT_STATUS_EVT		20			//定时检测MQTT连接状态事件
#define	GSM_MQTT_GETDATA0_EVT	21
#define	GSM_MQTT_GETDATA1_EVT	22
#define	GSM_MQTT_GETDATA2_EVT	23
#define	GSM_MQTT_GETDATA3_EVT	24

//HTTP Event
#define	GSM_HTTP_ACTIVE_EVT			30
#define	GSM_HTTP_OPENREQ_EVT 		31
#define	GSM_HTTP_UPDATE_STATUS_EVT	32
#define	GSM_HTTPS_INIT_EVT			33
#define	GSM_HTTPS_GETDATA_EVT		34


#define	SN_ID_LEN			14
#define	ICCID_ID_LEN	20
#define	PASSWORD_LEN	16

#define	HTTP_UPDATE_STATUS_UPDATING		2
#define	HTTP_UPDATE_STATUS_FAIL	4

#define	GSM_TM_MQTT_STATUS00	(10*60*100)				//MQTT Link 服务器判断是否断开
#define	GSM_TM_MQTT_STATUS05	(1*60*100)				//MQTT Link 服务器判断是否断开
//#define	GSM_TM_MQTT_PUB		(10*100)

//#define	GSM_TIMER_TX_TIME		(10*100)			//
#define	GSM_TIMER_TX_TIME		(60*60*100)			//

#define	GSM_VERSION_ID00	0x00
#define	GSM_VERSION_ID05	0x05

extern	const	char	HTTP_ACTIVE_Tab[];
extern	const	char	HTTP_OPEN_Tab[];
extern	const	char	HTTP_UPDATE_STATUS_Tab[];
extern	const uint32_t RollingTable;
extern	const uint32_t Rom_VerTab;

extern	uint8_t Gsm_State;
extern	uint8_t	Gsm_Tx_State;
extern	uint8_t Gsm_Pwr_State;
extern	uint32_t	Tm_Gsm_Ini;
extern uint32_t Tm_Gsm_Dly;
extern	uint32_t	Tm_Tx_Dly;
extern uint8_t FGsm_Vol_On;
extern	uint8_t	Tm_Uart_Tx;
extern	uint8_t FBRate_Set;
extern	uint32_t	Tm_Gsm_Out;
extern	uint32_t	Tm_Gsm_Tx;
extern	uint32_t	Tm_Gsm_Ini;
extern	uint8_t FGSM_Trs_Inf;
extern	uint8_t	FGsm_ACK;
extern	uint8_t		Gsm_MQTT_Rept_Mode;			//MQTT Report Mode
extern	uint8_t		Gsm_Rept_Node;			//
extern	uint32_t	Gsm_Tx_TmOut;
extern	char			Device_ID[23];
extern	char			ICCID_ID[20];
extern	char			SN_ID[14];							
extern	char			Sign[32];								//
extern	char			SecKey[17];
extern	char			Rom_Version[6];					//Rom version
extern	char			Gsm_Rolling;
extern	char			Gsm_WState;

extern	char			Gsm_URx_Dt[800];
extern	char			Gsm_Rx_Dt[450];
extern	uint64_t	Timestamp;
extern	uint64_t	Timestamp_NLink;				//
extern	uint8_t		CSQ_Buf;								//信号强度
extern	uint8_t		Gsm_Pack_data[450];
extern	uint32_t	Out_Len;
extern	uint8_t		Sys_Task_Step;
extern	char			Http_Update_Web[90];
extern	uint8_t		FGsm_Init_OK;
extern	uint8_t		FGsm_MQTT_Tx_OK;
extern	uint8_t		FGsm_MQTT_Init_OK;
extern	uint8_t		FMQTT_NLink;
extern	uint8_t		FHTTP_Link;
extern	uint16_t		GsmEvent_Ff[GSM_EVT_FF_LEN];
extern	uint8_t		Wt_GsmEvent_Pt;
extern	uint8_t		Rd_GsmEvent_Pt;
extern	uint16_t		Gsm_Event;
extern	uint32_t		Gsm_Timer_Tx;
extern	uint8_t		Gsm_Node_Rpt_Step;
extern	uint8_t		Http_Update;
extern	uint8_t		FGsm_HTTP_Rx_OK;
extern	uint32_t	Http_Update_StrAddr;
extern	uint32_t	Http_Update_StpAddr;
extern	uint32_t	Http_Req_Tmout;							//
extern	uint8_t		Http_Req_Cnt;								//
extern	uint8_t		Http_Req_Cnt_Tmp;						//
extern	uint8_t		Http_Update_Status;					//
extern	uint32_t	Http_Update_Tmout;					//
extern	uint16_t	CheckSum;										//
extern	uint16_t	CheckSum_Buf1;							//
extern	uint16_t	CheckSum_Buf2;							//
extern	uint16_t	Gsm_MQTT_Tmlink;
extern	uint8_t		Gsm_Ver_ID;
extern	uint16_t 	IC_year;
extern	uint8_t		IC_mon;
extern	uint8_t		IC_day;
extern	uint8_t		IC_hour;
extern	uint8_t		IC_min;
extern	uint8_t		IC_sec;
/*_____ F U N C T I O N S __________________________________________________*/
extern 	void GSM_Task_Machine(void);
void	GSM_Idle_Handle(void);
void 	GSM_Power_Handle(void);
void 	GSM_Tx_Handle(void);
void	GSM_Net_Handle(void);
void	GSM_Trs_Handle(void);
void 	Gsm_Read_URC(void);
void 	Gsm_ChkNet_Status(void);
void 	Gsm_Send_TXT(void);
void	GSM_Ttansmit(char *p,uint8_t len,uint32_t Tm_Out,uint8_t Retry_Cnt);
void 	GSM_Ttansmit_Param(char *p,uint8_t len,uint32_t Tm_Out,uint8_t Retry_Cnt);
void 	GSM_Ttansmit_Data(char *p,uint8_t len,uint32_t Tm_Out,uint8_t Retry_Cnt);
void 	GSM_Ttansmit_HttpUpdate(char *p,uint8_t len,uint32_t Tm_Out,uint8_t Retry_Cnt);
void 	GSM_Ttansmit_Addr(char *p,uint8_t len,uint32_t Tm_Out,uint8_t Retry_Cnt,uint32_t addr);
void GSM_Ttansmit_Http(char *p,uint8_t len,uint32_t Tm_Out,uint8_t Retry_Cnt);

void Gsm_Pwr_Off_Ini(void);
void	Handle_Gsm_URx_Dt(void);

void	Parse_Gsm_Rx_Dt(uint32_t	len);
void	Gsm_Rx_Task(void);
void	Gsm_Tx_Task(void);
extern	void	Gsm_Tx_ChkTmout(void);
void	MQTT_Link_Init(void);
void	Gsm_Activate_Chk(void);
uint32_t	Set_timestamp(uint16_t year,uint8_t mon,uint8_t day,uint8_t hour,uint8_t min,uint8_t sec);
void	Time_convert(void);

extern	void	Init_Gsm_Report(void);
extern	void	Gsm_Report_Over(void);

void	Get_HTTP_Update_Data_Init(void);
void	Decode_HTTP_Server_Data(void);
void	Gsm_Tx_Ram_Init(void);
extern	void	Clear_Gsm_Ff(void);
#endif
