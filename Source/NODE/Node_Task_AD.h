#ifndef __NODE_TASK_AD_H
#define __NODE_TASK_AD_H

/*_____ I N C L U D E S ____________________________________________________*/
#include	"stdint.h"
#include	"Node_Task_Machine.h"
/*_____ D E F I N I T I O N S ______________________________________________*/
#define	BAT_SHORT		1
#define	BAT_NSHORT	0

#define	AIN0	0
#define	AIN1	1
#define	AIN2	2
#define	AIN3	3
#define	AIN4	4
#define	AIN5	5
#define	AIN6	6
#define	AIN7	7
#define	AIN8	8
#define	AIN9	9
#define	AIN10	10
/*_____ D E C L A R A T I O N S ____________________________________________*/
extern	uint8_t		FNode_AD_Enb;
extern	uint8_t		FNode_AD_Str;
extern	uint8_t		FNode_AD_Stp;
extern	uint8_t		AD_Burp_SCnt;
extern	uint8_t		AD_Burp_NSCnt;
/*_____ F U N C T I O N S __________________________________________________*/

extern	void	Node_Task_AD(void);
void	Node_Rd_AD(uint8_t	Node_Num);
uint32_t	Rd_AD_Dt(uint8_t	Node_Num);
#endif
