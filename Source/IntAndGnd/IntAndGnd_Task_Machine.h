#ifndef __INTANDGND_TASK_MACHINE_H
#define __INTANDGND_TASK_MACHINE_H

/*_____ I N C L U D E S ____________________________________________________*/
#include	"stdint.h"
/*_____ D E F I N I T I O N S ______________________________________________*/

/*_____ D E C L A R A T I O N S ____________________________________________*/
extern	uint8_t	FTm_10ms;
/*_____ F U N C T I O N S __________________________________________________*/
void	Gsm_Tm_Task(void)	;
void	M_Tm_Task(void);
void	Com_Tm_Task(void);
void	LED_Tm_Task(void);
void	Time_Task(void);
extern	void IntAndGnd_Task_Machine(void);


#endif
