#ifndef H_SWITCH_H_
#define H_SWITCH_H_

#include "H_Switch_cfg.h"
typedef enum
{
	Switch_NOT_PRESSED = 1,
	Switch_PRESSED     = 0,
	Switch_Released    = 2
}SwitchState_t;



#define ACTIVE_LOW					0
#define ACTIVE_HIGH					1


typedef struct
{
	void* 		    Switch_Port;
	uint8_t	 		Switch_Pin;
	uint8_t  		Switch_mode;
	uint8_t  		Switch_ActiveStatuse;
	uint8_t	 		res;
}Switchcfg_t;




extern void          Switch_Init(void);
extern SwitchState_t Switch_GetStatus(uint8_t Copy_SwitchNum);
extern void 		 Switch_UpdateStatus(void);





#endif
