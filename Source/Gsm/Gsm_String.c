/******************** (C) COPYRIGHT 2014 SONiX *******************************
* COMPANY:			SONiX
* DATE:					2015/03/17
* AUTHOR:				SA1
* IC:						SN32F700
* DESCRIPTION:	Timer related functions.
*____________________________________________________________________________
* REVISION	Date				User		Description
* 1.0				2015/03/31	 SA1		1. First release
*
*____________________________________________________________________________
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS TIME TO MARKET.
* SONiX SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL 
* DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH SOFTWARE
* AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN 
* IN CONNECTION WITH THEIR PRODUCTS.
*****************************************************************************/
/*_____ I N C L U D E S ____________________________________________________*/
#include	"GSM_String.h"
#include	"string.h"
/*_____ D E F I N I T I O N S ______________________________________________*/
/*_____ D E C L A R A T I O N S ____________________________________________*/


/*_____ F U N C T I O N S __________________________________________________*/

/*****************************************************************************
* Function		: isOK
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
int isOK(char*str)
{
    if(match(str, "OK"))
        return MATCH;
    else
        return NO_MATCH;
}

int	isRDY(char*str)
{
    if(match(str, "RDY"))
        return MATCH;
    else
        return NO_MATCH;
}
/*****************************************************************************
* Function		: isCPIN
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
int isCPIN(char*str)
{
	if(match(str, "+CPIN: READY"))
        return MATCH;
    else
        return NO_MATCH;
}
/*****************************************************************************
* Function		: isCREG
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
int isCREG(char*str)
{
	if(match(str, "+CREG: 0,1"))
        return MATCH;
    else
        return NO_MATCH;
}
/*****************************************************************************
* Function		: isCGATT
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
int isCGATT(char*str)
{
	if(match(str, "+CGATT: 1"))
        return MATCH;
    else
        return NO_MATCH;
}
/*****************************************************************************
* Function		: isCGATT
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
int isCIFSR(char*str)
{
	if(match(str, "0.0.0.0"))
        return MATCH;
    else
        return NO_MATCH;
}
/*****************************************************************************
* Function		: isCGATT
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
int isCONNECT(char*str)
{
	if(match(str, "CONNECT OK"))
        return MATCH;
    else
        return NO_MATCH;
}

int	isALREADYCONNECT(char *str)
{
	if(match(str, "ALREADY CONNECT"))
        return MATCH;
    else
        return NO_MATCH;
}

int isPUBACK(char*str)
{
	if(match(str, "PUBACK"))
        return MATCH;
    else
        return NO_MATCH;
}

int	isPUBID(char *str)
{
	if(match(str, "+MPUB:ID:"))
        return MATCH;
    else
        return NO_MATCH;
}
/*****************************************************************************
* Function		: isCONNACK
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
int isCONNACK(char*str)
{
	if(match(str, "CONNACK OK"))
        return MATCH;
    else
        return NO_MATCH;
}
/*****************************************************************************
* Function		: isMSUB
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
int isMSUB0(char*str)
{
	if(match(str, "+MSUB:0"))
        return MATCH;
    else
        return NO_MATCH;
}

int isMSUB1(char*str)
{
	if(match(str, "+MSUB:1"))
        return MATCH;
    else
        return NO_MATCH;
}

int isMSUB2(char*str)
{
	if(match(str, "+MSUB:2"))
        return MATCH;
    else
        return NO_MATCH;
}

int isMSUB3(char*str)
{
	if(match(str, "+MSUB:3"))
        return MATCH;
    else
        return NO_MATCH;
}

/*****************************************************************************
* Function		: isMSUB
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
int	isMSUB(char *str)
{
	if(match(str, "+MSUB:\"device/in"))
        return MATCH;
    else
        return NO_MATCH;
}
/*****************************************************************************
* Function		: isPDP
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
int	isPDP(char *str)
{
	if(match(str, "+PDP: DEACT"))
        return MATCH;
    else
        return NO_MATCH;
}

int	isGSMVER05(char *str)
{
	if(match(str, "+ISLKVRSCAN: L206v01.01b14.05"))
        return MATCH;
    else
        return NO_MATCH;
}

int	isGSMVER00(char *str)
{
	if(match(str, "+ISLKVRSCAN: L206v01.01b14.00"))
        return MATCH;
    else
        return NO_MATCH;
}

int	isMQTTSTATU(char *str)
{
	if(match(str, "+MQTTSTATU:"))
        return MATCH;
    else
        return NO_MATCH;
}

int	isPINGRESP(char *str)
{
	if(match(str, "+MQTT:NOT RECEIVE PINGRESP"))
        return MATCH;
    else
        return NO_MATCH;
}
/*****************************************************************************
* Function		: isCLOSE
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
int	isCLOSED(char *str)
{
	if(match(str, "CLOSED"))
        return MATCH;
    else
        return NO_MATCH;
}
/*****************************************************************************
* Function		: isCLOSEOK
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
int	isCLOSEOK(char *str)
{
	if(match(str, "CLOSE OK"))
        return MATCH;
    else
        return NO_MATCH;
}
/*****************************************************************************
* Function		: isHTTPRECV
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
int	isHTTPRECV(char *str)
{
	if(match(str, "+HTTPRECV"))
        return MATCH;
    else
        return NO_MATCH;
}

int	isContentLength(char *str)
{
	if(match(str, "Content-Length:"))
       return MATCH;
    else
       return NO_MATCH;
}
/*****************************************************************************
* Function		: isERROR
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
int	isERROR(char *str)
{
	if(match(str, "ERROR"))
			 return MATCH;
		else
			 return NO_MATCH;
}
/*****************************************************************************
* Function		: isHTTPCLOSED
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
int	isHTTPCLOSED(char *str)
{
	if(match(str, "+HTTPCLOSED:HTTP Link Close"))
			 return MATCH;
		else
			 return NO_MATCH;
}

int	isPOWD(char *str)
{
	if(match(str, "NORMAL POWER DOWN"))
			 return MATCH;
		else
			 return NO_MATCH;
}

/*****************************************************************************
* Function		: match
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
char *match(char *str, char *target)
{
	char *psrc,*pdst;
	unsigned int mcount = 0;
	unsigned int mmax = strlen(target);

	psrc = str;
	pdst = target;

	while(*psrc != '\0' && *pdst != '\0')
	{
		if(*psrc == *pdst)
		{
			mcount++;
			psrc++;
			pdst++;
			if(mcount==mmax){
				return psrc;
			}
		}
		else{
		   pdst = target;
		   mcount = 0;
			psrc++;
		}		
	}

	return (char*)0;
}
