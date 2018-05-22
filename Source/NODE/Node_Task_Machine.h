#ifndef __NODE_TASK_MACHINE_H
#define __NODE_TASK_MACHINE_H

/*_____ I N C L U D E S ____________________________________________________*/
#include	"stdint.h"
/*_____ D E F I N I T I O N S ______________________________________________*/
#define	NODE_TOTAL_NUM	8
#define	NODE_EVT_FF_LEN		20
#define	NODE1				0
#define	NODE2				1
#define	NODE3				2
#define	NODE4				3
#define	NODE5				4
#define	NODE6				5
#define	NODE7				6
#define	NODE8				7
//==============================================================================
#define	NODE_IN_STA 	1					//节点充电宝是插入状态
#define	NODE_OUT_STA 	0					//节点充电宝是取走状态
#define	NODE_ON_STA		1					//节点是打开的状态
#define	NODE_OFF_STA	0					//节点是关闭状态
//==============================================================================
#define	NODE_NO_EVT			0				//没有节点事件
#define	NODE_IN_EVT			1				//节点插入
#define	NODE_OUT_EVT		2				//节点取走
#define	NODE_ON_EVT			3				//节点打开
#define	NODE_OFF_EVT		4				//节点关闭
#define	NODE_ALL_EVT		5				//读取所有节点信息事件
#define	NODE_ON_REQ_EVT	6				//服务器下发打开请求
#define	NODE_ONFAIL_EVT	7				//打开节点失败事件
#define	NODE_ONEREP_EVT	8				//单个节点上报事件
//==============================================================================
#define	TM_LED_PWRON		(50)			//base time10ms*n
#define	TM_LED_NOR			(24)			//base time10ms*n
#define	TM_LED_FLASH_NONET	(10)	//
#define	TM_LED_FLASH_NOMQTT	(30)
#define	TM_LED_FLASH_NETMQTT (200)
#define	TM_LED_RUN			(20)
#define	TM_LED_OUT			(250) 		//NOR*n
#define	TM_S_ONOFF			60				//
#define	TM_ONFAIL_FLASH	(100)			//1S
#define	CNT_ONFIAL_FLASH	10
//==============================================================================
#define	M_OPEN		0
#define	M_CLOSE		1

#define	M_OK			0
#define	M_FAIL		1
typedef struct {
	uint8_t		Node_Status;
	uint8_t		Node_AD_Status;
	char			Bat_ID[17];
	char			Bat_Status[2];
	char			Bat_Vol[5];
	uint8_t		M_State;
}
NODE_INFORMATION;

/*_____ D E C L A R A T I O N S ____________________________________________*/
extern	uint8_t		W_Node_Num;
extern	uint16_t		W_Node_Event;
extern	uint8_t		Node_Burp_Tm;
extern	NODE_INFORMATION	Node_Infor[8];
extern	uint8_t		Node_Com_Tm;
extern	uint8_t		Wt_NodeEvent_Pt;
extern	uint8_t		Rd_NodeEvent_Pt;
extern	uint16_t	Node_Close_Tm[NODE_TOTAL_NUM];			//节点插入，延迟上报时间
/*_____ F U N C T I O N S __________________________________________________*/
extern	void	Node_Task_Machine(void);
extern	void	Clear_Node_Ff(void);
extern	void	Node_Link_State_Init(void);
extern	void	Wt_NodeEvent_Ff(uint16_t	node_event);
extern	uint16_t	Rd_NodeEvent_Ff(void);
extern	void	Initial_Node_Handle(void);
extern	void	Node_Out_Handle(void);
extern	void	Node_On_Initial(void);
extern	void	Node_Off_Initial(void);
extern	void	Clear_Node_State(uint8_t Node_Num);
extern	void	Node_Com_Over(void);
extern	void	Node_AD_Over(void);
extern	void	Initial_Node_Out_Handle(void);
extern	void	Initial_Node_Onreq_Handle(void);
extern	void	Initial_Node_On_Handle(void);
extern	void	Clear_Node_State1(uint8_t node_Num);
extern	void	Clear_Node_BatID(uint8_t node_num);
#endif
