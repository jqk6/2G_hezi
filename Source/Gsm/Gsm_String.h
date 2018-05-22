#ifndef __GSM_STRING_H
#define __GSM_STRING_H
/*_____ I N C L U D E S ____________________________________________________*/
/*_____ D E F I N I T I O N S ______________________________________________*/

#define MATCH		1
#define NO_MATCH	0
#define	OK		0x01
#define	Fail	0xFF

extern int	isRDY(char*str);
int isQCCID(char*str);
int isSNEDOK(char*str);
int isRMC(char*str);
int	isCONNECT(char*str);
int is3E(char*str);
int isOK(char*str);
int	isERROR(char*str);
int isRDY(char*str);
int isCFUN1(char*str);
int isCPIN(char*str);
extern	int isCREG(char*str);
int isCGATT(char*str);
int isCallReady(char*str);
int isSMSReady(char*str);
char *match(char *str, char *target);
extern	int isCIFSR(char*str);
extern	int isCONNECT(char*str);
extern	int isCONNACK(char*str);
extern	int isMSUB0(char*str);
extern	int isMSUB1(char*str);
extern	int isMSUB2(char*str);
extern	int isMSUB3(char*str);
extern	int	isMSUB(char *str);
extern	int	isHTTPRECV(char *str);
extern	int	isContentLength(char *str);
extern	int	isPDP(char *str);
extern	int	isCLOSED(char *str);
extern	int	isERROR(char *str);
extern	int	isHTTPCLOSED(char *str);
extern	int	isPOWD(char *str);
extern	int	isALREADYCONNECT(char *str);
extern	int	isCLOSEOK(char *str);
extern	int isPUBACK(char*str);
extern	int	isGSMVER05(char *str);
extern	int	isGSMVER00(char *str);
extern	int	isMQTTSTATU(char *str);
extern	int	isPINGRESP(char *str);
extern	int	isPUBID(char *str);
#endif
