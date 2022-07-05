#ifndef SYSTICK_H_
#define SYSTICK_H_


typedef void (*Notify_t)(void);



void Systick_Init(void);
void Systick_SetTimeMs(uint32_t Copy_TickMs);
void Systick_RegisterCbf(Notify_t Add_cbf);
void Systick_Start(void);
void SysTick_Handler (void);



#endif
