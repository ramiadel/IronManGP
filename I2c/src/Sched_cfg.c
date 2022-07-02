#include <stdio.h>
#include <stdlib.h>

#include "H_Switch.h"
#include "App.h"

#include "Sched.h"
#include "Sched_cfg.h"



const Runnable_t runnables[]=
{
		[0]=
		{
				.Cbf    	  =  Switch_UpdateStatus,
				.CyclicTimeMS =  5,
		},
		[1]=
		{
				.Cbf           = MK5,
				.CyclicTimeMS  = 20,
		}
};

