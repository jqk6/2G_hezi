#ifndef __SYS_STATE_MACHINE_H
#define __SYS_STATE_MACHINE_H
/*_____ I N C L U D E S ____________________________________________________*/
#include	"stdint.h"
/*_____ D E F I N I T I O N S ______________________________________________*/
#define	SYS_PWRON_STA			0
#define	SYS_NORMAL_STA		1
#define	SYS_UPDATE_STA		2
#define	SYS_NONET_STA			3


/*_____ D E C L A R A T I O N S ____________________________________________*/
extern	uint8_t		Sys_State;
extern	uint8_t		Sys_NodeTask_Step;
extern	uint8_t		Sys_GsmTask_Step;
extern	uint8_t		Sys_Node_Cnt;
extern	uint8_t		Sys_Power_On;
extern	uint32_t	Rolling_Code;
extern	uint8_t		Sys_Node_Evt_Str;
/*_____ F U N C T I O N S __________________________________________________*/
extern	void	PFPA_Init(void);
extern	void Sys_Init(void);
void 	Sysclk_Init(void);
extern 	void Sys_Task_Machine(void);
void	Sys_Power_On_State(void);
void	Sys_Normal_State(void);
void 	Premain(void);
void	Read_AuthKey(void);
void	Initial_Node_Event(void);
void	Initial_Sys_Normal(void);

void			Wt_GsmEvent_Ff(uint16_t	gsm_event);
uint16_t	Rd_GsmEvent_Ff(void);
extern	void	Sys_Save_Flash(void);
extern	void	Return_Status(void);
#endif
