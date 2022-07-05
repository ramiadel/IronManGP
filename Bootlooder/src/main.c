#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "BLD_Manger.h"
#include "Flash.h"
#include "SCB.h"
#include "RCC.h"
#include "GPIO.h"


uint32_t Get_MainStackPionter(void);

typedef void (*Notify_t)(void);


int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;
	trace_printf("Restart\n");
	if((APP_INFO->AppEntryPoint) == 0xffffffff)
	{
		trace_printf("Enter Flashing Sec\n");
		Flash_Init();
		BLDManger_UartInit();

		while(1)
		{
			BLDManger_FlashingTask();
		}
	}
	else
	{
		trace_printf("Run APP\n");
		Notify_t App = (Notify_t)*((uint32_t *)(APP_INFO->AppFirstAdd+4));
		if(App != NULL)
		{
			//SCB_ChangeINTVECTadd(APP_INFO->AppFirstAdd);
			asm("ldr r5,=0x20010000");
			asm("msr msp,r5");
			trace_printf("Msp = 0x%x\n",Get_MainStackPionter());
			Gpio_enuSetPintValue(GPIOB , 5 , 0);
			App();
		}
	}
	return 0;
}
uint32_t Get_MainStackPionter(void)
{
	register uint32_t result;
	asm volatile ("MRS %0, msp" : "=r" (result) );
	return result;
}
