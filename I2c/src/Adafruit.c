#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "I2c.h"
#include "GPIO.h"
#include "RCC.h"

#include "Adafruit.h"


static I2cCfg_t Glop_I2c;

static void MK5_Delay(uint32_t Count);

void Adafruit_Init(void)
{
	uint32_t Timeout = 1000;


	GpioPincfg_t I2C1_SCL;
	I2C1_SCL.mode = MODE_ALT_FUNC_OPEN_DRAIN_PULL_UP;
	I2C1_SCL.pin = 8;
	I2C1_SCL.port = GPIOB;
	I2C1_SCL.speed = SPEED_VERY_HIGH;
	Gpio_enuinitPinCfg(&I2C1_SCL);
	Gpio_enuSetAF(GPIOB , 8 , GPIO_AF4);

	GpioPincfg_t I2C1_SDA;
	I2C1_SDA.mode = MODE_ALT_FUNC_OPEN_DRAIN_PULL_UP;
	I2C1_SDA.pin = 9;
	I2C1_SDA.port = GPIOB;
	I2C1_SDA.speed = SPEED_VERY_HIGH;
	Gpio_enuinitPinCfg(&I2C1_SDA);
	Gpio_enuSetAF(GPIOB , 9 , GPIO_AF4);

	/*Init i2c*/
	Glop_I2c.I2c_Channel = I2C1;
	Glop_I2c.ClockFrequency = 400000;
	Glop_I2c.InterfaceAddress = 0;
	I2c_Init(&Glop_I2c);

	/*Init */
	Adafruit_Write8(PCA9685_MODE1,MODE1_RESTART);
	while(Timeout--);
	trace_printf("%d\n",Adafruit_Read8(PCA9685_MODE1));

}
void Adafruit_SetPWMFreq(uint32_t Freq)
{
	uint8_t Prescale , Oldmode , Newmode;
	//volatile uint8_t Test;

	uint32_t Prescalevel = 25000000 , Timeout = 1000;
	Freq = (Freq * 9) / 10;
	Prescalevel /= 4096;
	Prescalevel /= Freq;
	Prescalevel -= 1;

	Prescale = (uint8_t)Prescalevel;
	Oldmode = Adafruit_Read8(PCA9685_MODE1);

	Newmode = (uint8_t)((Oldmode & ~MODE1_RESTART) | MODE1_SLEEP);     // sleep
	Adafruit_Write8(PCA9685_MODE1, Newmode);                         // go to sleep
	Adafruit_Write8(PCA9685_PRESCALE, Prescale);            // set the prescaler
    while(Timeout--);
    Adafruit_Write8(PCA9685_MODE1, Oldmode | MODE1_RESTART | MODE1_AI);
}
void Adafruit_SetPWM(uint8_t num, uint16_t on, uint16_t off)
{
	uint8_t Arr_data[] = {(uint8_t)(LED0_ON_L + 4 * num) , (uint8_t)(on & 0xff) , (uint8_t)((on >> 8)) ,
			(uint8_t)(off & 0xff) , (uint8_t)((off >> 8))};
	I2c_TransmitBlocking(&Glop_I2c , 0x80 , Arr_data , 5 , 10000);
	MK5_Delay(1000);
}


uint8_t Adafruit_Read8(uint8_t Addr)
{
	uint8_t Data;
	I2c_RecieveBlockingByte(&Glop_I2c , 0x80 , Addr , &Data, 10000);
	return Data;
}
void Adafruit_Write8(uint8_t Addr, uint8_t Data)
{
	uint8_t Arr_data[] = {Addr , Data};
	I2c_TransmitBlocking(&Glop_I2c , 0x80 , Arr_data , 2 , 10000);
}

void Adafruit_SetServoAngle(uint8_t num,uint16_t Angel)
{
	Adafruit_SetPWM(num , 0 , (uint16_t)(((2222 * Angel) / 1000) + 80));
}


static void MK5_Delay(uint32_t Count)
{
	while(Count--)
	{
		asm("Nop");
	}
}






