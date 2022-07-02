#include <stdio.h>
#include <stdlib.h>

#include "diag/Trace.h"

#include "H_Switch.h"


#include "GPIO.h"


extern  Switchcfg_t     Switchs[Switch_NUM];
static  SwitchState_t	Switch_State[Switch_NUM];

void Switch_Init(void)
{
	uint8_t Loc_SwitchIndx;
	GpioPincfg_t Switch_cfg;

	for(Loc_SwitchIndx = 0 ; Loc_SwitchIndx < Switch_NUM ; ++Loc_SwitchIndx)
	{
		Switch_cfg.port  = Switchs[Loc_SwitchIndx].Switch_Port;
		Switch_cfg.pin   = Switchs[Loc_SwitchIndx].Switch_Pin;
		Switch_cfg.mode  = Switchs[Loc_SwitchIndx].Switch_mode;
		Switch_cfg.speed = SPEED_LOW;
	    Switch_State[Loc_SwitchIndx] = Switch_NOT_PRESSED;
		Gpio_enuinitPinCfg(&Switch_cfg);
	}
}

SwitchState_t Switch_GetStatus(uint8_t Copy_SwitchNum)
{
	SwitchState_t Temp = Switch_State[Copy_SwitchNum];
	if(Switch_State[Copy_SwitchNum] == Switch_Released)
	{
		Switch_State[Copy_SwitchNum] = Switch_NOT_PRESSED;
	}
	return Temp;
}
void Switch_UpdateStatus(void)
{
	static uint8_t  prev[Switch_NUM];
	static uint8_t  counter[Switch_NUM];
	uint8_t         current[Switch_NUM];
	uint8_t         Loc_SwitchIndex;
	for(Loc_SwitchIndex = 0;Loc_SwitchIndex<Switch_NUM;++Loc_SwitchIndex)
	{
		Gpio_enuGetPinValue( (void *)(Switchs[Loc_SwitchIndex].Switch_Port) , Switchs[Loc_SwitchIndex].Switch_Pin , &current[Loc_SwitchIndex]);
		if(current[Loc_SwitchIndex] == prev[Loc_SwitchIndex])
		{
			++counter[Loc_SwitchIndex];
		}
		else
		{
			counter[Loc_SwitchIndex] = 0;
		}
		if(counter[Loc_SwitchIndex] == 5)
		{
			if(Switch_State[Loc_SwitchIndex] == Switch_PRESSED &&
					((current[Loc_SwitchIndex] ^ (Switchs[Loc_SwitchIndex].Switch_ActiveStatuse)) == Switch_NOT_PRESSED))
			{
				Switch_State[Loc_SwitchIndex] = Switch_Released;
			}
			else if(Switch_State[Loc_SwitchIndex] == Switch_Released)
			{
				// do nothing
			}
			else
			{
				Switch_State[Loc_SwitchIndex] = current[Loc_SwitchIndex]^(Switchs[Loc_SwitchIndex].Switch_ActiveStatuse);
			}
			counter[Loc_SwitchIndex]      = 0;
		}

		prev[Loc_SwitchIndex] = current[Loc_SwitchIndex];
	}
}

