#include <stdio.h>
#include <stdlib.h>

#include "diag/Trace.h"

#include "H_Switch.h"
#include "GPIO.h"



/*

---mode input---

MODE_INPUT_PULL_UP
MODE_INPUT_PULL_DOWN

*/
Switchcfg_t Switchs[Switch_NUM]=
{
		[Switch_MK5]=
		{
				.Switch_Port          = GPIOA,
				.Switch_Pin			  = 0,
				.Switch_mode          = MODE_INPUT_PULL_UP,
				.Switch_ActiveStatuse = ACTIVE_LOW,
		},
		/*[Switch_DOWEN]=
		{
				.Switch_Port  = GPIOA,
				.Switch_State = 0,
				.Switch_mode  = MODE_INPUT_PULL_DOWN,
				.Switch_ActiveStatuse = ACTIVE_HIGH,
		},
		[Switch_RIGHT]=
		{
				.Switch_Port  =  GPIOA,
				.Switch_State =  1,
				.Switch_mode  =  MODE_INPUT_PULL_UP,
				.Switch_ActiveStatuse = ACTIVE_LOW,
		},
		[Switch_LEFT]=
		{
				.Switch_Port  =  GPIOA,
				.Switch_State =  2,
				.Switch_mode  =  MODE_INPUT_PULL_UP,
				.Switch_ActiveStatuse = ACTIVE_LOW,
		},*/

};
