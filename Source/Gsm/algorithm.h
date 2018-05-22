#ifndef __ALGORITHM_H
#define __ALGORITHM_H

/*_____ I N C L U D E S ____________________________________________________*/
#include	"stdint.h"
/*_____ D E F I N I T I O N S ______________________________________________*/

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/
extern	void	Gsm_MQTT_Report_Payload_Init(uint8_t	mqtt_report_mode);
extern	void	Gsm_HTTP_Report_Payload_Init(void);
extern	void	Decode_MQTT_Server_Data(void);

extern	uint16_t	AsciitoInt(char *s);
extern	uint8_t	ChartoInt(char c);
extern	uint8_t	InttoAscii(uint8_t data);
uint8_t	Update_MQTT_Node_Report_Status(uint8_t node_num,uint8_t mode);
#endif
