#define F_CPU 16000000UL  // Define CPU clock as 16 MHz
#include <avr/io.h>        // Include standard I/O library for AVR
#include <util/delay.h>    // Include delay library
#include "liquid_crystal_i2c.h"
#include "i2c_master.h"
#include <stdio.h>


int main(void)
{
	
	DDRB |= (1 << PB5);
	DDRD &=(1<<2);
	int x =5;
	char x_str[30];
	i2c_master_init(I2C_SCL_FREQUENCY_100);					//SET LCD TO I2C0 PINS
	LiquidCrystalDevice_t device = lq_init(0x27, 20, 4, LCD_5x8DOTS);
	lq_turnOnBacklight(&device); // simply turning on the backlight
	// moving cursor to the next line
	while (1)
	{
		// Turn on the LED
		
		
		if (PIND & (1 << 2)) {
			while(PIND & (1 << 2)) {}
			x+=1;
			sprintf(x_str, "%d", x);
			lq_print(&device, x_str);
			lq_setCursor(&device, 1, 0);
		}
		
		
		
		
		
	}
}