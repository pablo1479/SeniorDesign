/*
* I2CMasterDevice.c
*
* Created: 12/9/2018 10:53:45 AM
*  Author: Denis Goriachev
*/

#ifndef  F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/twi.h>
#include <stdint.h>

#include "i2c_master.h"

#define I2C_SCL_FREQUENCY_PRESCALER 1
#define I2C_TWBR_VALUE 

#define I2C_PRESCALER_MASK 0xF8

#define MCP4725_ADDR 0x62 // MCP4725 I2C address

void i2c_master_init(unsigned long frequency)
{
	TWSR1 = 0;
	TWBR1 = (uint8_t)((((F_CPU / frequency) / I2C_SCL_FREQUENCY_PRESCALER) - 16 ) / 2);
}

uint8_t i2c_master_start(uint8_t address, uint8_t mode)
{
	uint8_t   twst;

	// reset control register
	TWCR1 = 0;

	// transmit START condition
	TWCR1 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	// wait for end of transmission
	while( !(TWCR1 & (1<<TWINT)) );
	
	// check if the start condition was successfully transmitted. Mask prescaler bits.
	twst = TWSR1 & TW_STATUS_MASK & I2C_PRESCALER_MASK;
	if ( (twst != TW_START) && (twst != TW_REP_START))
	{
		return I2C_STATUS_ERROR_START_WAS_NOT_ACCEPTED;
	}
	
	// load shifted slave address into data register with specified mode
	TWDR1 = (address << 1) | mode;

	// start transmission of address
	TWCR1 = (1<<TWINT) | (1<<TWEN);

	// wait for end of transmission
	while( !(TWCR1 & (1<<TWINT)) );
	
	// check if the device has acknowledged the READ / WRITE mode
	twst = TWSR1 & TW_STATUS_MASK & I2C_PRESCALER_MASK;
	if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) )
	{
		return I2C_STATUS_ERROR_TRANSMIT_OR_READ_WAS_NOT_ACKNOWLEDGED;
	}
	
	return I2C_STATUS_SUCCESS;
};

uint8_t i2c_master_startWait(uint8_t address, uint8_t mode)
{
	uint8_t twst;

	while (1)
	{
		// send START condition
		TWCR1 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
		
		// wait until transmission completed
		while(!(TWCR1 & (1<<TWINT)));
		
		// check value of TWI Status Register. Mask prescaler bits.
		twst = TWSR0 & TW_STATUS_MASK & I2C_PRESCALER_MASK;
		if ( (twst != TW_START) && (twst != TW_REP_START)) continue;
		
		// send device address
		TWDR1 = (address << 1) | mode;
		TWCR1 = (1<<TWINT) | (1<<TWEN);
		
		// wail until transmission completed
		while(!(TWCR1 & (1<<TWINT)));
		
		// check value of TWI Status Register. Mask prescaler bits.
		twst = TWSR1 & TW_STATUS_MASK & I2C_PRESCALER_MASK;
		if ( (twst == TW_MT_SLA_NACK )||(twst ==TW_MR_DATA_NACK) )
		{
			/* device busy, send stop condition to terminate write operation */
			TWCR1 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
			
			// wait until stop condition is executed and bus released
			while(TWCR1 & (1<<TWSTO));
			
			continue;
		}
		break;
	}

	return I2C_STATUS_SUCCESS;
};

uint8_t i2c_master_write(uint8_t data)
{
	uint8_t twst;

	// put data into data register
	TWDR1 = data;

	// start transmission of data
	TWCR1 = (1<<TWINT) | (1<<TWEN);

	// wait for end of transmission
	while( !(TWCR1 & (1<<TWINT)) );
	
	// check value of TWI Status Register. Mask prescaler bits
	twst = TWSR1 & TW_STATUS_MASK & I2C_PRESCALER_MASK;

	// If Slave device not acknowledged transmission - returns an error
	if( twst != TW_MT_DATA_ACK)
	{
		return I2C_STATUS_ERROR_TRANSMIT_NOT_ACKNOWLEDGED;
	}
	
	return I2C_STATUS_SUCCESS;
}

uint8_t i2c_master_readAck(void)
{
	// start TWI module and acknowledge data after reception
	TWCR1 = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);

	// wait for end of transmission
	while( !(TWCR1 & (1<<TWINT)) );

	return TWDR1;
}

uint8_t i2c_master_readNack(void)
{
	// start receiving without acknowledging reception
	TWCR1 = (1<<TWINT) | (1<<TWEN);

	// wait for end of transmission
	while( !(TWCR1 & (1<<TWINT)) );

	// return received data from TWDR
	return TWDR1;
}

uint8_t i2c_master_send(uint8_t address, uint8_t* data, uint16_t length)
{
	// starts the transmitting
	uint8_t status = i2c_master_start(address, I2C_WRITE);

	// in case of error - returns status code
	if (status) return status;
	
	for (uint16_t i = 0; i < length; i++)
	{
		status = i2c_master_write(data[i]);
		if (status) return status;
	}
	
	i2c_master_stop();
	
	return I2C_STATUS_SUCCESS;
}

uint8_t i2c_master_sendByte(uint8_t address, uint8_t data)
{
	uint8_t status = i2c_master_start(address, I2C_WRITE);
	if (status) return status;

	status = i2c_master_write(data);
	if (status) return status;

	i2c_master_stop();
	
	return I2C_STATUS_SUCCESS;
};

uint8_t i2c_master_receive(uint8_t address, uint8_t* data, uint16_t length)
{
	uint8_t status = i2c_master_start(address, I2C_READ);
	if (status) return status;
	
	for (uint16_t i = 0; i < (length-1); i++)
	{
		data[i] = i2c_master_readAck();
	}
	data[(length-1)] = i2c_master_readNack();
	
	i2c_master_stop();
	
	return I2C_STATUS_SUCCESS;
}

void i2c_master_stop(void)
{
	TWCR1 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
};


//																	DAC FUNCTIONS

void I2C_Init(void) {
	// Set the bit rate for 400 kHz I2C
	TWSR0 = 0x00;  // Prescaler set to 1
	TWBR0 = ((F_CPU / SCL_CLOCK) - 16) / 2;  // Set bit rate register for 400kHz
}

void I2C_Start(void) {
	TWCR0 = (1<<TWSTA) | (1<<TWEN) | (1<<TWINT);  // Send start condition
	while (!(TWCR0 & (1<<TWINT)));  // Wait for transmission to complete
}

// I2C Stop condition
void I2C_Stop(void) {
	TWCR0 = (1<<TWSTO) | (1<<TWINT) | (1<<TWEN);  // Send stop condition
}

// I2C Write byte
void I2C_Write(uint8_t data) {
	TWDR0 = data;  // Load data to data register
	TWCR0 = (1<<TWINT) | (1<<TWEN);  // Start transmission
	while (!(TWCR1 & (1<<TWINT)));  // Wait for transmission to complete
}

void MCP4725_SetValue(uint16_t value) {
	I2C_Start();
	I2C_Write(MCP4725_ADDR << 1);  // Write address with write bit (0)
	I2C_Write((value >> 8) & 0x0F);  // Send upper data bits (D11-D8)
	I2C_Write(value & 0xFF);  // Send lower data bits (D7-D0)
	I2C_Stop();
}