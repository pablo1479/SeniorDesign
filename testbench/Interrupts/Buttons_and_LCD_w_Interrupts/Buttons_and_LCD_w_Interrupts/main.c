/*
 * Buttons_with_Interrupts.c
 *
 * Created: 10/22/2024 4:29:07 PM
 * Author : david
 */ 
#define F_CPU 16000000UL  // Define CPU clock as 16 MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/io.h>        // Include standard I/O library for AVR
#include <util/delay.h>    // Include delay library
#include <stdio.h>
#include "liquid_crystal_i2c.h"
#include "i2c_master.h"

volatile uint8_t buttonPressed = 0;			//volatile keywords allows variables to be changed using interrupts


ISR(PCINT2_vect) {
	// Check if button is pressed (PD2 is LOW)
	if (PIND & (1 << 2)) {
		while(PIND & (1 << 2)) {}				//busy while loop for "debouncing"
		buttonPressed = 1;
		
		
	}
	
}

		

int main(void)
{
    /* Replace with your application code */
	DDRD &= (1<<2);		//sets pin D2 to input
	
	 PCICR |= (1 << PCIE2);  // Enable PCINT16-23 group interrupt
	 PCMSK2 |= (1 << PCINT18);  // Enable interrupt for PD2 (PCINT18)
	 
	 char x_str[4];		//allegedly i can make this 4
	 uint8_t x = 0;
	 
	 i2c_master_init(I2C_SCL_FREQUENCY_100);			//SET LCD TO I2C0 PINS
	 LiquidCrystalDevice_t device = lq_init(0x27, 20, 4, LCD_5x8DOTS);
	 lq_turnOnBacklight(&device);
	 
	 
	 
	 sei(); //enables global interrupts
    while (1) 
    {	
		if(buttonPressed){
			sprintf(x_str, "%d", x);
			lq_print(&device, x_str);
			lq_setCursor(&device, 1, 0);
			buttonPressed =0;
		}
		
    }
}

