/*===========================================================================*/
/* Project:  WeAct Black Pill V1.2 Sample Software                           */
/* Module :  RCC.h                                                           */
/* Version:  V1.00                                                           */
/*===========================================================================*/
/* History:                                                                  */
/*              V1.00: Initial version                                       */
/*                                                                           */
/*===========================================================================*/
/*                                                                           */
/*  Header file for the Systick.c source code                                */
/*                                                                           */
/*===========================================================================*/
#ifndef SYSTICK_PRV_H_
#define SYSTICK_PRV_H_


/*---- structure for SysTick Registers  --------------------*/
typedef struct
{
	uint32_t	STK_CTRL;
	uint32_t	STK_LOAD;
	uint32_t	STK_VAL;
	uint32_t	STK_CALIB;
}SystickReg_t;

/*--STK base address---*/
#define	SYSTICK_BASE		                ((volatile SystickReg_t *) 0xE000E010u)
#define	SYSTICK_HSI									16000u
#define	SYSTICK_HSE									25000u
#define	SYSTICK_EXCEPTION							0b10u
#define	SYSTICK_NO_EXCEPTION						0b0u
#define SYSTICK_AHB_8								0b100u



#endif
