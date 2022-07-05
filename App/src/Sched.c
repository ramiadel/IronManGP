#include <stdio.h>
#include <stdlib.h>

#include "Sched.h"
#include "Sched_cfg.h"
#include "diag/Trace.h"
#include "SysTick.h"

extern const Runnable_t runnables[];
uint8_t Glop_SchedStart = 0;
static void sched(void);
void Sched_Init(void)
{
	Systick_Init();
	Systick_RegisterCbf(Sched_Systickfunc);
	Systick_SetTimeMs(SCHED_TICK_TIME);

}
void Sched_Start(void)
{
	Systick_Start();
	while(1)
	{
		if(Glop_SchedStart==1)
		{
			Glop_SchedStart=0;
			sched();
		}
	}
}

void Sched_Systickfunc(void)
{
	Glop_SchedStart = 1 ;
}
static void sched(void)
{
	static uint32_t time;
	uint8_t indx;
	if(time == 0)
	{
		trace_printf("time = 0");
		time = SCHED_TICK_TIME;
	}
	for(indx=0 ; indx < SCHED_RUNNABLE_NUM ;++indx)
	{
		if((time % runnables[indx].CyclicTimeMS) == 0)
		{
			(runnables[indx].Cbf)();
		}
	}
	time += SCHED_TICK_TIME;

}
