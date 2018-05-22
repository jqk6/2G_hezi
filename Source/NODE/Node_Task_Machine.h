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
#define	NODE_IN_STA 	1					//�ڵ��籦�ǲ���״̬
#define	NODE_OUT_STA 	0					//�ڵ��籦��ȡ��״̬
#define	NODE_ON_STA		1					//�ڵ��Ǵ򿪵�״̬
#define	NODE_OFF_STA	0					//�ڵ��ǹر�״̬
//==============================================================================
#define	NODE_NO_EVT			0				//û�нڵ��¼�
#define	NODE_IN_EVT			1				//�ڵ����
#define	NODE_OUT_EVT		2				//�ڵ�ȡ��
#define	NODE_ON_EVT			3				//�ڵ��
#define	NODE_OFF_EVT		4				//�ڵ�ر�
#define	NODE_ALL_EVT		5				//��ȡ���нڵ���Ϣ�¼�
#define	NODE_ON_REQ_EVT	6				//�������·�������
#define	NODE_ONFAIL_EVT	7				//�򿪽ڵ�ʧ���¼�
#define	NODE_ONEREP_EVT	8				//�����ڵ��ϱ��¼�
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
extern	uint16_t	Node_Close_Tm[NODE_TOTAL_NUM];			//�ڵ���룬�ӳ��ϱ�ʱ��
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
