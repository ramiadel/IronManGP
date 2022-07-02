#include <stdio.h>
#include <stdlib.h>

#include "H_Switch.h"
#include "Adafruit.h"
#include "App.h"
#include "BLD_Manger.h"
#include "GPIO.h"


typedef enum
{
	Close,
	Open

}Mk5State_t;

#define SERVO_0_OPEN				300
#define SERVO_0_CLOSE				90

#define SERVO_1_OPEN				95
#define SERVO_1_CLOSE				290


#define SERVO_2_OPEN				120
#define SERVO_2_CLOSE				270

#define SERVO_3_OPEN				320
#define SERVO_3_CLOSE				180

#define SERVO_4_OPEN				130
#define SERVO_4_CLOSE				290

#define SERVO_5_OPEN				90
#define SERVO_5_CLOSE				290

#define SERVO_6_OPEN				300
#define SERVO_6_CLOSE				90

#define SERVO_7_OPEN				130
#define SERVO_7_CLOSE				290

#define SERVO_8_OPEN				400
#define SERVO_8_CLOSE				130

#define SERVO_9_OPEN				130
#define SERVO_9_CLOSE				400



static void MK5_Sec(Mk5State_t State);

void MK5_Init(void)
{
	/*Green*/
	GpioPincfg_t Green_Led;
	Green_Led.mode  = MODE_OUTPUT_PUSH_PULL;
	Green_Led.port  = GPIOB;
	Green_Led.pin   = 6;
	Green_Led.speed = SPEED_MEDIUM;
	Gpio_enuinitPinCfg(&Green_Led);
	MK5_Sec(Close);
}


void MK5(void)
{
	static Mk5State_t State = Close;
	uint8_t Mk5_RetState = Get_Mk5UartState();
	if(Switch_GetStatus(Switch_MK5) == Switch_Released)
	{
		State ^=1;
		MK5_Sec(State);
	}
	else if(Mk5_RetState == 'O')
	{
		State = Open;
		MK5_Sec(Open);
	}
	else if(Mk5_RetState == 'C')
	{
		State = Close;
		MK5_Sec(Close);
	}
}


static void MK5_Sec(Mk5State_t State)
{
	if(State == Open)
	{
		Gpio_enuSetPintValue(GPIOB , 5 , 0);

		Adafruit_SetPWM(0 , 0 , SERVO_0_OPEN);

		Adafruit_SetPWM(10 , 0 , SERVO_1_OPEN);

		Adafruit_SetPWM(12 , 0 , SERVO_2_OPEN);

		Adafruit_SetPWM(3 , 0 , SERVO_3_OPEN);

		Adafruit_SetPWM(4 , 0 , SERVO_4_OPEN);

		Adafruit_SetPWM(5 , 0 , SERVO_5_OPEN);

		Adafruit_SetPWM(6 , 0 , SERVO_6_OPEN);

		Adafruit_SetPWM(7 , 0 , SERVO_7_OPEN);

		Adafruit_SetPWM(8 , 0 , SERVO_8_OPEN);

		Adafruit_SetPWM(9 , 0 , SERVO_9_OPEN);
	}
	else
	{
		Adafruit_SetPWM(0 , 0 , SERVO_0_CLOSE);

		Adafruit_SetPWM(10 , 0 , SERVO_1_CLOSE);

		Adafruit_SetPWM(12 , 0 , SERVO_2_CLOSE);

		Adafruit_SetPWM(3 , 0 , SERVO_3_CLOSE);

		Adafruit_SetPWM(4 , 0 , SERVO_4_CLOSE);

		Adafruit_SetPWM(5 , 0 , SERVO_5_CLOSE);

		Adafruit_SetPWM(6 , 0 , SERVO_6_CLOSE);

		Adafruit_SetPWM(7 , 0 , SERVO_7_CLOSE);

		Adafruit_SetPWM(8 , 0 , SERVO_8_CLOSE);

		Adafruit_SetPWM(9 , 0 , SERVO_9_CLOSE);

		Gpio_enuSetPintValue(GPIOB , 5 , GPIO_HIGH);
	}
}


//Servo_State = 1;
/*Lservo = SERVO_2_CLOSE;
Hservo = SERVO_3_CLOSE;
while(Servo_State == 1)
{
	Adafruit_SetPWM(2 , 0 , Lservo--);
	//while(Count--);
	Adafruit_SetPWM(3 , 0 , Hservo++);
	if((Lservo <= SERVO_2_OPEN) && (Hservo >= SERVO_3_OPEN))
	{
		Servo_State = 0;
	}
}*/

