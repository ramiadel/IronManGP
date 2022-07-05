#ifndef SCHED_H_
#define SCHED_H_



typedef void (*Runnablecbf_t)(void);
typedef struct
{
	Runnablecbf_t Cbf;
	uint32_t CyclicTimeMS;

}Runnable_t;


extern void Sched_Init(void);
extern void Sched_Start(void);
extern void Sched_Systickfunc(void);


#endif
