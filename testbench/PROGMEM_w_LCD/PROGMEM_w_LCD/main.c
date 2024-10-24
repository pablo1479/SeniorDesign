/*
 * PROGMEM_w_LCD.c
 *
 * Created: 10/23/2024 12:52:44 AM
 * Author : david
 */ 


#define F_CPU 16000000UL  // Define CPU clock as 16 MHz
#include <avr/io.h>        // Include standard I/O library for AVR
#include <util/delay.h>    // Include delay library
#include "liquid_crystal_i2c.h"
#include "i2c_master.h"
#include <stdio.h>

#include <avr/pgmspace.h> //Include flash memory library


int main(void)
{
	
	i2c_master_init(I2C_SCL_FREQUENCY_100);					//SET LCD TO I2C0 PINS
	LiquidCrystalDevice_t device = lq_init(0x27, 20, 4, LCD_5x8DOTS);
	
	lq_setCursor(&device, 1, 0);		// moving cursor to the next line
	lq_turnOnBacklight(&device);
	const char test[]  PROGMEM = "Hello World";
	char buffer[11];				//if error maybe fix this?
	strcpy_P(buffer, test);			//if this works try putting this in one line
	lq_print(&device, buffer);
	while (1)
	{
		
		
		
		
		
		
	}
}

