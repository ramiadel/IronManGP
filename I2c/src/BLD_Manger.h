#ifndef BLD_MANGER_H_
#define BLD_MANGER_H_


typedef struct
{
	uint32_t AppFirstAdd;
	uint32_t AppEntryPoint;
	uint32_t AppSize;
	uint32_t App_ApdateReq;
}App_t;

#define MARK_ADD						        0x08004000u

#define APP_INFO								((volatile App_t *) MARK_ADD)

extern void BLDManger_UartInit(void);
extern void RXBuffCblt(void);
extern uint8_t Get_Mk5UartState(void);

#endif
