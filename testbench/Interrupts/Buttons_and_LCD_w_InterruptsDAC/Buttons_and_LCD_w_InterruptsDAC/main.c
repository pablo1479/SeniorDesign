/*
 * Buttons_with_Interrupts.c
 *
 * Created: 10/22/2024 4:29:07 PM
 * Author : david
 */ 

//THIS IS THE EXACT SAME CODE AS "Buttons_and_LCD_w_Interrupts", but the DAC is running in the background to test how fast of a frequency the DAC can do using interrupts for optimization
#define F_CPU 16000000UL  // Define CPU clock as 16 MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/io.h>        // Include standard I/O library for AVR
#include <util/delay.h>    // Include delay library
#include <stdio.h>
#include "liquid_crystal_i2c.h"
#include "i2c_master.h"

#define SCL_CLOCK 400000L

#define MCP4725_ADDR 0x62 // MCP4725 I2C address

volatile uint8_t buttonPressed = 0;			//volatile keywords allows variables to be changed using interrupts


ISR(PCINT2_vect) {
	// Check if button is pressed (PD2 is LOW)
	if (PIND & (1 << 2)) {
		while(PIND & (1 << 2)) {}				//busy while loop for "debouncing"
		buttonPressed = 1;
		
		
	}
	
}
void I2C_Init(void) {
	// Set the bit rate for 400 kHz I2C
	TWSR = 0x00;  // Prescaler set to 1
	TWBR = ((F_CPU / SCL_CLOCK) - 16) / 2;  // Set bit rate register for 400kHz
}
// I2C Start condition
void I2C_Start(void) {
	TWCR1 = (1<<TWSTA) | (1<<TWEN) | (1<<TWINT);  // Send start condition
	while (!(TWCR1 & (1<<TWINT)));  // Wait for transmission to complete
}

// I2C Stop condition
void I2C_Stop(void) {
	TWCR1 = (1<<TWSTO) | (1<<TWINT) | (1<<TWEN);  // Send stop condition
}

// I2C Write byte
void I2C_Write(uint8_t data) {
	TWDR1 = data;  // Load data to data register
	TWCR1 = (1<<TWINT) | (1<<TWEN);  // Start transmission
	while (!(TWCR1 & (1<<TWINT)));  // Wait for transmission to complete
}

void MCP4725_SetValue(uint16_t value) {
	I2C_Start();
	I2C_Write(MCP4725_ADDR << 1);  // Write address with write bit (0)
	I2C_Write((value >> 8) & 0x0F);  // Send upper data bits (D11-D8)
	I2C_Write(value & 0xFF);  // Send lower data bits (D7-D0)
	I2C_Stop();
}
		

int main(void)
{
    /* Replace with your application code */
	 I2C_Init();
	 DDRD &= (1<<2);		//sets pin D2 to input
	
	 PCICR |= (1 << PCIE2);  // Enable PCINT16-23 group interrupt
	 PCMSK2 |= (1 << PCINT18);  // Enable interrupt for PD2 (PCINT18)
	 
	 char x_str[4];		//allegedly i can make this 4
	 uint8_t x = 0;
	 
	 i2c_master_init(I2C_SCL_FREQUENCY_100);			//SET LCD TO I2C0 PINS
	 LiquidCrystalDevice_t device = lq_init(0x27, 20, 4, LCD_5x8DOTS);
	 lq_turnOnBacklight(&device);
	 
	 
	 uint16_t dac_value = 0;
	 sei(); //enables global interrupts
    while (1) 
    {	
		if(buttonPressed){
			
			x+=1;
			sprintf(x_str, "%d", x);
			lq_print(&device, x_str);
			lq_setCursor(&device, 1, 0);
			buttonPressed =0;
		}
		
		MCP4725_SetValue(dac_value);

		// Toggle between high and low values
		if (dac_value == 4095) {
			dac_value = 0;  // Switch to low value
			} else {
			dac_value = 4095;  // Switch to high value
		}
    }
}

