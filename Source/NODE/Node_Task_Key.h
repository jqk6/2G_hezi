#ifndef __NODE_TASK_KEY_H
#define __NODE_TASK_KEY_H
/*_____ I N C L U D E S ____________________________________________________*/
#include	"stdint.h"
#include	"Node_Task_Machine.h"
/*_____ D E F I N I T I O N S ______________________________________________*/

/*_____ D E C L A R A T I O N S ____________________________________________*/
extern	uint8_t	Node_Key_In[NODE_TOTAL_NUM];
extern	uint8_t	Node_Key_Chk[NODE_TOTAL_NUM];
extern	uint8_t	Node_Key_Cvt[NODE_TOTAL_NUM];
extern	uint8_t	Node_Key_Old[NODE_TOTAL_NUM];
/*_____ F U N C T I O N S __________________________________________________*/

extern	void	Node_Task_Key(void);


#endif
