#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "RCC.h"
#include "Adafruit.h"
#include "sched.h"
#include "H_Switch.h"
#include "BLD_Manger.h"
#include "Flash.h"
#include "App.h"


int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;
	Flash_Init();
	RCC_EnablePriphralCLK(RCC_AHB1ENR_GPIOAEN);
	RCC_EnablePriphralCLK(RCC_APB1ENR_I2C1EN);
	RCC_EnablePriphralCLK(RCC_AHB1ENR_GPIOBEN);
	/*---Enable clock for UART1,GPIOA---*/
	RCC_EnablePriphralCLK(RCC_APB2ENR_USART1EN);
	//RCC_EnablePriphralCLK(RCC_AHB1ENR_GPIOAEN);

	BLDManger_UartInit();

	MK5_Init();

	Switch_Init();

	Adafruit_Init();

	Adafruit_SetPWMFreq(50);

	Sched_Init();
	Sched_Start();


	return 0;
}

