#ifndef __NODE_TASK_CTRL_H
#define __NODE_TASK_CTRL_H

/*_____ I N C L U D E S ____________________________________________________*/
#include	"stdint.h"
#include	"Node_Task_Machine.h"
/*_____ D E F I N I T I O N S ______________________________________________*/
#define	TM_OFFREP_DELAY		200										//插入关闭上报delay时间
/*_____ D E C L A R A T I O N S ____________________________________________*/
extern	uint16_t	M_ON_Tm[NODE_TOTAL_NUM];
extern	uint16_t	M_PWM_Tm[NODE_TOTAL_NUM];
extern	uint16_t	M_ON_Init_Tm[NODE_TOTAL_NUM];	
extern	uint8_t		M_Status[NODE_TOTAL_NUM];				//节点电磁铁状态
extern	uint8_t		LED_Toggle_Tm[NODE_TOTAL_NUM];
extern	uint8_t		LED_Toggle_Cnt[NODE_TOTAL_NUM];
extern	uint8_t		LED_Toggle_Tmtmp[NODE_TOTAL_NUM];
extern	uint8_t		LED_Status;
extern	uint8_t		LED_Update_Step;
/*_____ F U N C T I O N S __________________________________________________*/

extern	void	Node_Task_Ctrl(void);
extern	void	S_ON_Out(uint8_t	Node_Num);
extern	void	S_OFF_Out(uint8_t	Node_Num);
extern	void	M_ON_Out(uint8_t	Node_Num);
extern	void	M_OFF_Out(uint8_t	Node_Num);
extern	void	M_Toggle_Out(uint8_t Node_Num);
extern	void	M_Tm_Task(void);

extern	void	LED_ON_Out(uint8_t	Node_Num);
extern	void	LED_OFF_Out(uint8_t	Node_Num);
extern	void	LED_Toggle_Out(uint8_t node_num);
extern	void	M_ON_PWM(uint8_t Node_Num);
extern	void	M_OFF_PWM(uint8_t Node_Num);

extern	void	IN_Control_Init(void);
extern	void	OUT_Control_Init(void);
extern	void	ON_Control_Init(void);
extern	void	Close_N_M(uint8_t num);
extern	void	Close_LED_Flash(uint8_t num);

extern	void	M_OFF_Init(uint8_t node_num);
extern	void	M_ON_Init(uint8_t node_num);

extern	void	LED_On_Init(uint8_t node_num);
extern	void	LED_Off_Init(uint8_t node_num);
extern	void	LED_Flash_Init(uint8_t node_num,uint8_t	time,uint8_t cnt);
extern	void	LED_ALLFlash_Init(uint8_t num,uint8_t toggle_tm);
extern	void	Init_M_S_LED(void);
extern	void	LED_Run_Init(void);
extern	void	Close_N_S(uint8_t num);
#endif
