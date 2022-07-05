#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "I2c.h"
#include "I2c_cfg.h"


typedef enum
{
	Start,
	SlaveAdd,
	MemAdd,
	Rstart,
	RslaveAdd,
	ReadMem

}I2c_RecieveBlockingByte_t;

I2CReturnState_t I2c_Init(I2cCfg_t* Add_I2c)
{
	I2CReturnState_t ReturnState = I2C_OK;
	if(Add_I2c == NULL)
	{
		ReturnState = I2C_NULL;
	}
	else
	{
		/*Disable I2c*/
		((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CR1 &=~ I2C_CR1_PE;

		/*Set Peripheral clock frequency*/
		((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CR2 = (uint32_t)(I2C_CLK/1000000U);

		/*Set Rise Time*/
		((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_TRISE = I2C_RISE_TIME((uint32_t)(I2C_CLK/1000000U) , Add_I2c->ClockFrequency);

		/*Set Speed*/
		((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CCR = I2C_SPEED_STANDARD(I2C_CLK , Add_I2c->ClockFrequency);

		/*Set own address 7-bit*/
		((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_OAR1 = (uint32_t)((uint32_t)0x00004000 | Add_I2c->InterfaceAddress);

		/*Enable I2c*/
		((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CR1 |= I2C_CR1_PE;
	}
	return ReturnState;
}


I2CReturnState_t I2c_TransmitBlocking(I2cCfg_t* Add_I2c ,uint16_t DevAddress, uint8_t *pData, uint16_t Size,uint32_t TimeOut)
{
	I2CReturnState_t ReturnState = I2C_OK;
	uint32_t index;
	uint32_t Temp_TimeOut;
	if(Add_I2c == NULL || pData == NULL)
	{
		ReturnState = I2C_NULL;
	}
	else
	{
		/*1- Start Condition*/
		((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CR1 |= I2C_CR1_START;
		Temp_TimeOut = TimeOut;
		while((--Temp_TimeOut) && ((((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_SB) == 0));
		if(!(((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_SB))
		{
			ReturnState = I2C_NOK;
		}
		else
		{
			Temp_TimeOut = TimeOut;
			/*2- Send Slave Address*/
			((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_DR =  ((uint8_t)((DevAddress) & (~I2C_OAR1_ADD0))) ;
			while(((((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_ADDR) == 0) && (--Temp_TimeOut));
			if(!((((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_ADDR)))
			{
				ReturnState = I2C_NOK;
			}
			else
			{
				Temp_TimeOut = ((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR2;
				/*3- Send Data*/
				for(index = 0; index < Size ; index++)
				{
					((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_DR = pData[index];
					Temp_TimeOut = TimeOut;
					while((--Temp_TimeOut) && (((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_TxE)==0);
					if(!((((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_TxE)))
					{
						ReturnState = I2C_NOK;
						break;
					}
				}
				Temp_TimeOut = TimeOut;
				while((--Temp_TimeOut) && (((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_BTF)==0);
				if(!((((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_BTF)))
				{
					ReturnState = I2C_NOK;
				}
				else
				{
					/*4- Generate Stop condition */
					((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CR1 |= I2C_CR1_STOP;
				}
			}
		}
	}
	return ReturnState;
}


I2CReturnState_t I2c_RecieveBlockingByte(I2cCfg_t* Add_I2c ,uint16_t DevAddress, uint8_t MemAddress , uint8_t *pData , uint32_t TimeOut)
{
	I2CReturnState_t ReturnState = I2C_OK;
	uint32_t Temp_TimeOut;
	volatile I2c_RecieveBlockingByte_t RecieveSeq = Start;
	if(Add_I2c == NULL || pData == NULL)
	{
		ReturnState = I2C_NULL;
	}
	else
	{
		while(RecieveSeq <= ReadMem)
		{
			switch(RecieveSeq)
			{
			case Start:
				/* Enable Acknowledge */
				((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CR1 |= I2C_CR1_ACK;
				/*1- Start Condition*/
				((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CR1 |= I2C_CR1_START;

				Temp_TimeOut = TimeOut;
				while((--Temp_TimeOut) && (((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_SB) == 0);
				if(!(((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_SB))
				{
					((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CR1 |= I2C_CR1_STOP;
					RecieveSeq = ReadMem + 1;
					ReturnState = I2C_NOK;
				}
				else
				{
					RecieveSeq = SlaveAdd;
				}
				break;
			case SlaveAdd:
				/*2- Send Slave Address*/
				((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_DR = ((uint8_t)((DevAddress) & (~I2C_OAR1_ADD0)));
				/* Wait until ADDR flag is set */
				Temp_TimeOut = TimeOut;
				while((--Temp_TimeOut) && (((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_ADDR) == 0);
				if(!((((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_ADDR)))
				{
					((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CR1 |= I2C_CR1_STOP;
					RecieveSeq = ReadMem + 1;
					ReturnState = I2C_NOK;
				}
				else
				{
					RecieveSeq = MemAdd;
				}
				break;
			case MemAdd:
				/*clear I2C_SR1_ADDR*/
				Temp_TimeOut = ((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1;
				Temp_TimeOut = ((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR2;
				/* Wait until TXE flag is set */
				Temp_TimeOut = TimeOut;
				while((--Temp_TimeOut) && (((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_TxE) == 0);
				if(!((((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_TxE)))
				{
					((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CR1 |= I2C_CR1_STOP;
					RecieveSeq = ReadMem + 1;
					ReturnState = I2C_NOK;
				}
				else
				{
					/* Send Memory Address */
					((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_DR = MemAddress;
					/* Wait until TXE flag is set */
					Temp_TimeOut = TimeOut;
					while((--Temp_TimeOut) && (((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_TxE)==0);
					if(!((((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_TxE)))
					{
						ReturnState = I2C_NOK;
					}
					else
					{
						RecieveSeq = Rstart;
					}
				}
				break;
			case Rstart:
				/* Generate Restart */
				((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CR1 |= I2C_CR1_START;
				Temp_TimeOut = TimeOut;
				while((--Temp_TimeOut) && (((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_SB)==0);
				if(!(((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_SB))
				{
					((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CR1 |= I2C_CR1_STOP;
					RecieveSeq = ReadMem + 1;
					ReturnState = I2C_NOK;
				}
				else
				{
					RecieveSeq = RslaveAdd;
				}
				break;
			case RslaveAdd:
				/*2- Send Slave Address*/
				((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_DR = ((uint8_t)((DevAddress) | I2C_OAR1_ADD0));
				/* Wait until ADDR flag is set */
				Temp_TimeOut = TimeOut;
				while((--Temp_TimeOut) && (((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_ADDR) == 0);
				if(!((((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_ADDR)))
				{
					((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CR1 |= I2C_CR1_STOP;
					RecieveSeq = ReadMem + 1;
					ReturnState = I2C_NOK;
				}
				else
				{
					RecieveSeq = ReadMem;
				}
				break;
			case ReadMem:
				/* Disable Acknowledge */
				((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CR1 &=~ I2C_CR1_ACK;
				/* Clear ADDR flag */
				Temp_TimeOut = ((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1;
				Temp_TimeOut = ((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR2;
				/* Generate Stop */
				((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CR1 |= I2C_CR1_STOP;
				/* Wait until RXNE flag is set */
				Temp_TimeOut = TimeOut;
				while((--Temp_TimeOut) && (((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_RxNE) == 0);
				if(!((((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_SR1 & I2C_SR1_RxNE)))
				{
					//((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_CR1 |= I2C_CR1_STOP;
					RecieveSeq = ReadMem + 1;
					ReturnState = I2C_NOK;
				}
				else
				{
					*pData = (uint8_t)((volatile I2CReg_t*) Add_I2c->I2c_Channel)->I2C_DR;
					RecieveSeq = ReadMem + 1;
				}
				break;
			}
		}
	}
	trace_printf("End Res\n");
	return ReturnState;
}













