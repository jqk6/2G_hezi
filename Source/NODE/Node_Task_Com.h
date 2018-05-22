#ifndef __NODE_TASK_COM_H
#define __NODE_TASK_COM_H

/*_____ I N C L U D E S ____________________________________________________*/
#include	"stdint.h"
/*_____ D E F I N I T I O N S ______________________________________________*/



/*_____ D E C L A R A T I O N S ____________________________________________*/
extern	uint8_t		FNode_Com_Enb;
extern	uint8_t		Com_Burp_Tm;
extern	uint8_t		FCom_Ack;
extern	uint8_t		FNode_Com_Str;
extern	uint8_t		FNode_Com_Stp;
extern	uint8_t		Node_Com_Err;
extern	uint8_t		Node_Com_Sta;
extern	uint8_t		Node_Com_Step;
extern	uint8_t		Com_Uart_Tmout;
/*_____ F U N C T I O N S __________________________________________________*/

extern	void	Node_Task_Com(void);
extern	void	Node_Com_Tx(uint8_t	Node_Num);
extern	void	Node_Com_Rx(uint8_t	Node_Num);
void	Node_Chk_Com(uint8_t	Node_Num);
uint8_t	Check_Sum(uint8_t *p,uint8_t len);



#endif
