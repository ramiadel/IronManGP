#ifndef ADAFRUIT_H_
#define ADAFRUIT_H_





#define PCA9685_SUBADR1       			0x2
#define PCA9685_SUBADR2 				0x3
#define PCA9685_SUBADR3 				0x4

#define PCA9685_MODE1 					0x0
#define PCA9685_PRESCALE			    0xFE

#define LED0_ON_L 						0x6
#define LED0_ON_H 						0x7
#define LED0_OFF_L 						0x8
#define LED0_OFF_H 						0x9

#define ALLLED_ON_L 					0xFA
#define ALLLED_ON_H 					0xFB
#define ALLLED_OFF_L 					0xFC
#define ALLLED_OFF_H 					0xFD

#define MODE1_ALLCAL 					0x01  /**< respond to LED All Call I2C-bus address */
#define MODE1_SUB3 						0x02    /**< respond to I2C-bus subaddress 3 */
#define MODE1_SUB2 						0x04    /**< respond to I2C-bus subaddress 2 */
#define MODE1_SUB1 						0x08    /**< respond to I2C-bus subaddress 1 */
#define MODE1_SLEEP 					0x10   /**< Low power mode. Oscillator off */
#define MODE1_AI 						0x20      /**< Auto-Increment enabled */
#define MODE1_EXTCLK 					0x40  /**< Use EXTCLK pin clock */
#define MODE1_RESTART 					0x80 /**< Restart enabled */



extern void    Adafruit_Init(void);
extern void    Adafruit_SetPWMFreq(uint32_t freq);
extern void    Adafruit_SetPWM(uint8_t num, uint16_t on, uint16_t off);
extern uint8_t Adafruit_Read8(uint8_t Addr);
extern void    Adafruit_Write8(uint8_t Addr, uint8_t Data);
extern void Adafruit_SetServoAngle(uint8_t num,uint16_t Angel);








#endif
