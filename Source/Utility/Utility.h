#ifndef __SN32F240_UTILITY_H
#define __SN32F240_UTILITY_H

/*_____ I N C L U D E S ____________________________________________________*/
#include <SN32F240.h>

/*_____ D E F I N I T I O N S ______________________________________________*/


/*_____ M A C R O S ________________________________________________________*/



/*_____ D E C L A R A T I O N S ____________________________________________*/
extern	void UT_DelayNx2us (void);
void UT_DelayNx10us (uint32_t N);
void UT_DelayNms (uint32_t N);

#endif	/*__SN32F240_UTILITY_H*/
