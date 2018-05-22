/******************** (C) COPYRIGHT 2014 SONiX *******************************
* COMPANY:			SONiX
* DATE:				2015/03/17
* AUTHOR:			SA1
* IC:				SN32F700
* DESCRIPTION:	 	System state functions.
*____________________________________________________________________________
* REVISION	Date				User		Description
* 1.0			  2015/03/31	SA1			1. First release
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
#include 	<SN32F240.h>
#include	"Node_Task_Com.h"
#include	"Node_Task_Ctrl.h"
#include	"..\UserApp\Sys_Task_Machine.h"
#include	"..\Driver\GPIO\GPIO.h"
/*_____ D E F I N I T I O N S ______________________________________________*/
uint16_t	M_ON_Tm[NODE_TOTAL_NUM];					//电磁铁常开时间
uint16_t	M_PWM_Tm[NODE_TOTAL_NUM];					//电磁铁PWM开时间
uint16_t	M_ON_Init_Tm[NODE_TOTAL_NUM];			//电池铁刚打开状态时间
uint8_t		M_Status[NODE_TOTAL_NUM];					//节点电磁铁状态
uint8_t		LED_Toggle_Tm[NODE_TOTAL_NUM];
uint8_t		LED_Toggle_Cnt[NODE_TOTAL_NUM];
uint8_t		LED_Toggle_Tmtmp[NODE_TOTAL_NUM];
uint8_t		LED_Flash_Mode;
uint8_t		LED_Status;
uint8_t		LED_Update_Step;
/*_____ F U N C T I O N S __________________________________________________*/

/*****************************************************************************
* Function		: Close_N_M
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: 
*****************************************************************************/
void	Close_N_M(uint8_t num)
{
	uint8_t i;
	for(i=0;i<num;i++)
	{
		M_OFF_Init(i);
	}
}
/*****************************************************************************
* Function		: Close_N_S
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: 
*****************************************************************************/
void	Close_N_S(uint8_t num)
{
	uint8_t i;
	for(i=0;i<num;i++)
	{
		S_OFF_Out(i);
	}
}
/*****************************************************************************
* Function		: LED_ALLFlash_Init
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: 
*****************************************************************************/
void	LED_ALLFlash_Init(uint8_t num,uint8_t toggle_tm)
{	uint8_t i;
	for(i=0;i<num;i++)
	{
		LED_ON_Out(i);
		LED_Toggle_Tm[i] =toggle_tm;
		LED_Toggle_Cnt[i] =0;
	}
}
/*****************************************************************************
* Function		: LED_On_Init
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: 
*****************************************************************************/
void	LED_On_Init(uint8_t node_num)
{	
	LED_Toggle_Tm[node_num] =0;
	LED_Toggle_Cnt[node_num] =0;
	LED_Toggle_Tmtmp[node_num] =0;
	LED_ON_Out(node_num);
}
/*****************************************************************************
* Function		: LED_Off_Init
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: 
*****************************************************************************/
void	LED_Off_Init(uint8_t node_num)
{	
	LED_Toggle_Tm[node_num] =0;
	LED_Toggle_Cnt[node_num] =0;
	LED_Toggle_Tmtmp[node_num] =0;
	LED_OFF_Out(node_num);
}
/*****************************************************************************
* Function		: LED_Flash_Init
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: 
*****************************************************************************/
void	LED_Flash_Init(uint8_t node_num,uint8_t	time,uint8_t cnt)
{	
	LED_Toggle_Tm[node_num] =time;										//
	LED_Toggle_Tmtmp[node_num] =time;									//
	LED_Toggle_Cnt[node_num] =cnt;										//
}
/*****************************************************************************
* Function		: LED_Run_Init
* Description	: 在远程更新状态，初始化LED跑马
* Input				: None
* Output			: None
* Return			: None
* Note				: 
*****************************************************************************/
void	LED_Run_Init(void)
{
	uint8_t i;
	LED_Status =0;
	LED_Update_Step =0;
	for(i=0;i<NODE_TOTAL_NUM;i++)
	{
		LED_Toggle_Cnt[i] =0;
		LED_Toggle_Tm[i] =TM_LED_RUN;
	}
	LED_ON_Out(NODE1);
}
/*****************************************************************************
* Function		: Init_M_S_LED
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: 
*****************************************************************************/
void	Init_M_S_LED(void)
{	uint8_t i;
	LED_Status =0;																//
	for(i=0;i<NODE_TOTAL_NUM;i++)
	{
		LED_Toggle_Tm[i] =0;												//
		LED_Toggle_Tmtmp[i]=0;											//
		LED_Toggle_Cnt[i] =0;												//
		if(Node_Infor[i].Node_Status ==NODE_OUT_STA)//
		{
			S_OFF_Out(i);															//关闭充电
			M_ON_Init(i);															//打开电池铁
		}
		else
		{
			S_ON_Out(i);															//打开充电
			M_OFF_Init(i);														//关闭电池铁
			LED_Status |=(1<<i);											//打开LED
		}
	}
}
/*****************************************************************************
* Function		: CT16B1_IRQHandler
* Description	: 刷新LED中断 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
__irq void CT16B1_IRQHandler(void)
{
	SN_GPIO0->BCLR |=(1<<14)|(1<<15);
	switch(LED_Flash_Mode)
	{
		case 0:
		SN_GPIO0->BSET |=1<<15;	
		if((LED_Status &0x01) ==0x00)
		{
			SN_GPIO1->BSET |=1<<2;
		}
		else
		{
				SN_GPIO1->BCLR |=1<<2;	
		}
		if((LED_Status &0x02) ==0x00)
		{
			SN_GPIO1->BSET |=1<<3;
		}
		else
		{
				SN_GPIO1->BCLR |=1<<3;	
		}		
		if((LED_Status &0x04) ==0x00)
		{
			SN_GPIO1->BSET |=1<<4;
		}
		else
		{
			SN_GPIO1->BCLR |=1<<4;	
		}
		if((LED_Status &0x08) ==0x00)
		{
			SN_GPIO1->BSET |=1<<5;
		}
		else
		{
			SN_GPIO1->BCLR |=1<<5;	
		}		
		LED_Flash_Mode =1;
		break;
		
		case 1:
		SN_GPIO0->BSET |=1<<14;	
		if((LED_Status &0x10) ==0x00)
		{
			SN_GPIO1->BSET |=1<<2;
		}
		else
		{
				SN_GPIO1->BCLR |=1<<2;	
		}
		if((LED_Status &0x20) ==0x00)
		{
			SN_GPIO1->BSET |=1<<3;
		}
		else
		{
				SN_GPIO1->BCLR |=1<<3;	
		}		
		if((LED_Status &0x40) ==0x00)
		{
			SN_GPIO1->BSET |=1<<4;
		}
		else
		{
			SN_GPIO1->BCLR |=1<<4;	
		}
		if((LED_Status &0x80) ==0x00)
		{
			SN_GPIO1->BSET |=1<<5;
		}
		else
		{
			SN_GPIO1->BCLR |=1<<5;	
		}		
		LED_Flash_Mode =0;
		break;
		default:
		break;
	}
	if(Com_Uart_Tmout!=0)	Com_Uart_Tmout--;
	SN_CT16B1->IC =0xfff;
}
/*****************************************************************************
* Function		: S_ON_Out
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	S_ON_Out(uint8_t	node_num)
{
	switch(node_num)
	{
		case NODE1:	S1_ON	break;
		case NODE2:	S2_ON	break;	
		case NODE3:	S3_ON	break;	
		case NODE4:	S4_ON	break;	
		case NODE5:	S5_ON	break;	
		case NODE6:	S6_ON	break;	
		case NODE7:	S7_ON	break;	
		case NODE8:	S8_ON	break;			
		default:	break;
	}
}
/*****************************************************************************
* Function		: S_OFF_Out
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	S_OFF_Out(uint8_t	node_num)
{
	switch(node_num)
	{
		case NODE1:	S1_OFF	break;
		case NODE2:	S2_OFF	break;	
		case NODE3:	S3_OFF	break;	
		case NODE4:	S4_OFF	break;	
		case NODE5:	S5_OFF	break;	
		case NODE6:	S6_OFF	break;	
		case NODE7:	S7_OFF	break;	
		case NODE8:	S8_OFF	break;			
		default:	break;
	}
}
/*****************************************************************************
* Function		: M_ON_Out
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	M_ON_Out(uint8_t	node_num)
{
	switch(node_num)
	{
		case NODE1:	M1_ON	break;
		case NODE2:	M2_ON	break;	
		case NODE3:	M3_ON	break;	
		case NODE4:	M4_ON	break;	
		case NODE5:	M5_ON	break;	
		case NODE6:	M6_ON	break;	
		case NODE7:	M7_ON	break;	
		case NODE8:	M8_ON	break;			
		default:	break;
	}
}
/*****************************************************************************
* Function		: M_OFF_Out
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	M_OFF_Out(uint8_t	node_num)
{
	switch(node_num)
	{
		case NODE1:	M1_OFF	break;
		case NODE2:	M2_OFF	break;	
		case NODE3:	M3_OFF	break;	
		case NODE4:	M4_OFF	break;	
		case NODE5:	M5_OFF	break;	
		case NODE6:	M6_OFF	break;	
		case NODE7:	M7_OFF	break;	
		case NODE8:	M8_OFF	break;			
		default:	break;
	}
}
/*****************************************************************************
* Function		: M_ON_PWM
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	M_ON_PWM(uint8_t node_num)
{
	switch(node_num)
	{
		case NODE1:	M1_PWM =1;	break;
		case NODE2:	M2_PWM =1;	break;	
		case NODE3:	M3_PWM =1;	break;	
		case NODE4:	M4_PWM =1;	break;	
		case NODE5:	M5_PWM =1;	break;	
		case NODE6:	M6_PWM =1;	break;	
		case NODE7:	M7_PWM =1;	break;	
		case NODE8:	M8_PWM =1;	break;			
		default:	break;
	}
}
/*****************************************************************************
* Function		: M_OFF_PWM
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	M_OFF_PWM(uint8_t node_num)
{
	switch(node_num)
	{
		case NODE1:	M1_PWM =0;	break;
		case NODE2:	M2_PWM =0;	break;	
		case NODE3:	M3_PWM =0;	break;	
		case NODE4:	M4_PWM =0;	break;	
		case NODE5:	M5_PWM =0;	break;	
		case NODE6:	M6_PWM =0;	break;	
		case NODE7:	M7_PWM =0;	break;	
		case NODE8:	M8_PWM =0;	break;			
		default:	break;
	}
}
/*****************************************************************************
* Function		: LED_ON_Out
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	LED_ON_Out(uint8_t	node_num)
{
	switch(node_num)
	{
		case NODE1:	LED_Status |=0x01;	break;
		case NODE2:	LED_Status |=0x02;	break;	
		case NODE3:	LED_Status |=0x04;	break;	
		case NODE4:	LED_Status |=0x08;	break;	
		case NODE5:	LED_Status |=0x10;	break;	
		case NODE6:	LED_Status |=0x20;	break;	
		case NODE7:	LED_Status |=0x40;	break;	
		case NODE8:	LED_Status |=0x80;	break;			
		default:	break;
	}
}
/*****************************************************************************
* Function		: LED_OFF_Out
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	LED_OFF_Out(uint8_t	node_num)
{
	switch(node_num)
	{
		case NODE1:	LED_Status &=~0x01;	break;
		case NODE2:	LED_Status &=~0x02;	break;	
		case NODE3:	LED_Status &=~0x04;	break;	
		case NODE4:	LED_Status &=~0x08;	break;	
		case NODE5:	LED_Status &=~0x10;	break;	
		case NODE6:	LED_Status &=~0x20;	break;	
		case NODE7:	LED_Status &=~0x40;	break;	
		case NODE8:	LED_Status &=~0x80;	break;			
		default:	break;
	}
}
/*****************************************************************************
* Function		: LED_Toggle_Out
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	LED_Toggle_Out(uint8_t node_num)
{
	switch(node_num)
	{
		case NODE1:	LED_Status ^=0x01;	break;
		case NODE2:	LED_Status ^=0x02;	break;	
		case NODE3:	LED_Status ^=0x04;	break;	
		case NODE4:	LED_Status ^=0x08;	break;	
		case NODE5:	LED_Status ^=0x10;	break;	
		case NODE6:	LED_Status ^=0x20;	break;	
		case NODE7:	LED_Status ^=0x40;	break;	
		case NODE8:	LED_Status ^=0x80;	break;			
		default:	break;	
	}
}
/*****************************************************************************
* Function		: M_OFF_Init
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	M_OFF_Init(uint8_t node_num)
{
	M_OFF_PWM(node_num);										//关闭pwm功能
	M_OFF_Out(node_num);										//电磁铁关闭
	M_ON_Tm[node_num] =0;										//清常开时间
	M_PWM_Tm[node_num] =0;									//清pwm时间
	M_Status[node_num] =M_CLOSE;						//电磁铁状态为关闭
	M_ON_Init_Tm[node_num] =0;							//
}
/*****************************************************************************
* Function		: M_ON_Init
* Description	: 
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void	M_ON_Init(uint8_t node_num)
{
	M_OFF_PWM(node_num);										//关闭pwm功能	
	M_ON_Out(node_num);											//电磁铁打开	
	M_PWM_Tm[node_num] =0;									//清pwm时间
	M_ON_Tm[node_num] =200;									//初始化打开电池铁时间
	M_Status[node_num] =M_OPEN;							//电磁铁状态为打开
	M_ON_Init_Tm[node_num] =2000;						//刚打开20S,是2S开,2SPWM
}

