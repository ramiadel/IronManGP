#ifndef I2C_H_
#define I2C_H_

typedef struct
{
	uint32_t I2C_CR1;
	uint32_t I2C_CR2;
	uint32_t I2C_OAR1;
	uint32_t I2C_OAR2;
	uint32_t I2C_DR;
	uint32_t I2C_SR1;
	uint32_t I2C_SR2;
	uint32_t I2C_CCR;
	uint32_t I2C_TRISE;
	uint32_t I2C_FLTR;
}I2CReg_t;


#define I2C1				 (void*)0x40005400
#define I2C2				 (void*)0x40005800
#define I2C3				 (void*)0x40005C00

typedef struct
{
	void * I2c_Channel;
	uint32_t InterfaceAddress;
	uint32_t ClockFrequency;
}I2cCfg_t;


typedef enum
{
	I2C_OK,
	I2C_NOK,
	I2C_NULL,
	I2C_Busy

}I2CReturnState_t;

/*---bit defection for I2C Control register 1---*/
#define I2C_CR1_PE							(uint32_t)0x1u
#define I2C_CR1_SMBUS                       (uint32_t)0x2u
#define I2C_CR1_SMBTYPE                     (uint32_t)0x8u
#define I2C_CR1_ENARP                       (uint32_t)0x10u
#define I2C_CR1_ENPEC                       (uint32_t)0x20u
#define I2C_CR1_ENGC                        (uint32_t)0x40u
#define I2C_CR1_NOSTRETCH                   (uint32_t)0x80u
#define I2C_CR1_START                       (uint32_t)0x100u
#define I2C_CR1_STOP                        (uint32_t)0x200u
#define I2C_CR1_ACK                         (uint32_t)0x400u
#define I2C_CR1_POS                         (uint32_t)0x800u
#define I2C_CR1_PEC                         (uint32_t)0x1000u
#define I2C_CR1_ALERT                       (uint32_t)0x2000u
#define I2C_CR1_SWRST                       (uint32_t)0x8000u

/*---bit defection for I2C Control register 2---*/
#define I2C_CR2_FREQ						(uint32_t)0x3fu
#define I2C_CR2_FREQ0						(uint32_t)0x1u
#define I2C_CR2_FREQ1                       (uint32_t)0x2u
#define I2C_CR2_FREQ2                       (uint32_t)0x4u
#define I2C_CR2_FREQ3                       (uint32_t)0x8u
#define I2C_CR2_FREQ4                       (uint32_t)0x10u
#define I2C_CR2_FREQ5                       (uint32_t)0x20u
#define I2C_CR2_ITERREN                     (uint32_t)0x100u
#define I2C_CR2_ITEVTEN                     (uint32_t)0x200u
#define I2C_CR2_ITBUFEN                     (uint32_t)0x400u
#define I2C_CR2_DMAEN                       (uint32_t)0x800u
#define I2C_CR2_LAST                        (uint32_t)0x1000u

/*---bit defection for I2C Own address register 1---*/
#define I2C_OAR1_ADD0						(uint32_t)0x1u
#define I2C_OAR1_ADD8						(uint32_t)0x100u
#define I2C_OAR1_ADD9                       (uint32_t)0x200u
#define I2C_OAR1_ADDMODE					(uint32_t)0x80000000u

/*---bit defection for I2C Status register 1---*/
#define I2C_SR1_SB							(uint32_t)0x1u
#define I2C_SR1_ADDR						(uint32_t)0x2u
#define I2C_SR1_BTF                         (uint32_t)0x4u
#define I2C_SR1_ADD10                       (uint32_t)0x8u
#define I2C_SR1_STOPF                       (uint32_t)0x10u
#define I2C_SR1_RxNE                        (uint32_t)0x40u
#define I2C_SR1_TxE                         (uint32_t)0x80u
#define I2C_SR1_BERR                        (uint32_t)0x100u
#define I2C_SR1_ARLO                        (uint32_t)0x200u
#define I2C_SR1_AF                          (uint32_t)0x400u
#define I2C_SR1_OVR                         (uint32_t)0x800u
#define I2C_SR1_PECERR                      (uint32_t)0x1000u
#define I2C_SR1_TIMEOUT                     (uint32_t)0x4000u
#define I2C_SR1_SMBALERT                    (uint32_t)0x8000u

/*---bit defection for I2C Status register 2---*/
#define I2C_SR2_MSL							(uint32_t)0x1u
#define I2C_SR2_BUSY                        (uint32_t)0x2u
#define I2C_SR2_TRA                         (uint32_t)0x4u
#define I2C_SR2_GENCALL                     (uint32_t)0x10u
#define I2C_SR2_SMBDEFAULT                  (uint32_t)0x20u
#define I2C_SR2_SMBHOST                     (uint32_t)0x40u
#define I2C_SR2_DUALF                       (uint32_t)0x80u


/*---bit defection for I2C Clock control register---*/
#define I2C_CCR_DUTY     					(uint32_t)0x4000u
#define I2C_CCR_FS                          (uint32_t)0x8000u
#define I2C_CCR_CCR							(uint32_t)0xFFFu


#define I2C_SPEED_STANDARD(__PCLK__, __SPEED__)            (((((__PCLK__)/((__SPEED__) << 1U)) & I2C_CCR_CCR) < 4U)? 4U:((__PCLK__) / ((__SPEED__) << 1U)))

#define I2C_RISE_TIME(__FREQRANGE__, __SPEED__)            (((__SPEED__) <= 100000U) ? ((__FREQRANGE__) + 1U) : ((((__FREQRANGE__) * 300U) / 1000U) + 1U))

extern I2CReturnState_t I2c_Init(I2cCfg_t* Add_I2c);

extern I2CReturnState_t I2c_TransmitBlocking(I2cCfg_t* Add_I2c ,uint16_t DevAddress, uint8_t *pData, uint16_t Size,uint32_t TimeOut);

extern I2CReturnState_t I2c_RecieveBlockingByte(I2cCfg_t* Add_I2c ,uint16_t DevAddress, uint8_t MemAddress , uint8_t *pData , uint32_t TimeOut);

















#endif
