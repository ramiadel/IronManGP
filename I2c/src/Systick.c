/*===========================================================================*/
/* Project:  WeAct Black Pill V1.2 board Sample Software                     */
/* Module :  RCC.c                                                           */
/* Version:  V1.00                                                           */
/*===========================================================================*/
/* History:                                                                  */
/*              V1.00: Initial version                                       */
/*                                                                           */
/*===========================================================================*/
/*                                                                           */
/* Source code for the system timer, SysTick configurations                  */
/*                                                                           */
/*===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "Systick.h"
#include "Systick_prv.h"
#include "Systick_cfg.h"

static Notify_t Cbf;


void Systick_Init(void)
{
	/*Bit 2 CLKSOURCE: Clock source selection 0: AHB/8 */
	SYSTICK_BASE->STK_CTRL &= ~(1u<<2);
	SYSTICK_BASE->STK_CTRL |= SYSTICK_TICKINT;
}
void Systick_SetTimeMs(uint32_t Copy_TickMs)
{
	uint32_t Loc_NumOfClock = Copy_TickMs * SYSTICK_CLKSOURCE / 8 ;
	if(Loc_NumOfClock >0x00FFFFFF)
	{
		Loc_NumOfClock = 0x00FFFFFF;
	}
	SYSTICK_BASE->STK_LOAD = Loc_NumOfClock;

}

void Systick_Start(void)
{
	/*enable counter bit 0*/
	SYSTICK_BASE->STK_CTRL |= 1;
}

void Systick_RegisterCbf(Notify_t Add_cbf)
{
	if(Add_cbf != NULL)
	{
		Cbf = Add_cbf;
	}
}


void SysTick_Handler(void)
{
	if(Cbf!=NULL)
	{
		Cbf();
	}
}







