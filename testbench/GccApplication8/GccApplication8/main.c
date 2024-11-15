/*
 * Buttons_with_Interrupts.c
 *
 * Created: 10/22/2024 4:29:07 PM
 * Author : david
 */ 
#define F_CPU 16000000UL  // Define CPU clock as 16 MHz

#include <avr/io.h>
#include <avr/interrupt.h>
#include "liquid_crystal_i2c.h"			//update this library to use the DAC
#include "i2c_master.h"
#include <stdio.h>
#include <util/delay.h> 


volatile uint8_t dac_screen = 0;
volatile uint8_t aux_screen = 0;
uint8_t screenFlag = 0;
ISR(PCINT2_vect) {
    
    if (PIND & (1 << PD1) ) {
        dac_screen = 1;
		aux_screen=0;
    }
	 else {
		 aux_screen = 1; // Turn off LED when PD1 is LOW
		 dac_screen = 0;
	 }
	 screenFlag = 1;
    
}

int main(void) {
    // Set PD1 as input
    DDRD &= ~(1 << DDD1);    // Clear bit 1 of DDRD to set PD1 as input

    // Enable pull-up resistor on PD1
    

    // Set PB5 (built-in LED) as output
    //DDRB |= (1 << PB5);

    // Enable pin change interrupt for PCINT17 (PD1)
    PCICR |= (1 << PCIE2);   // Enable PCINT2 group (PCINT16-23)
    PCMSK2 |= (1 << PCINT17); // Enable interrupt specifically for PD1 (PCINT17)

    // Enable global interrupts
    sei();
	
	i2c_master_init(I2C_SCL_FREQUENCY_100);					//SET LCD TO I2C0 PINS
	LiquidCrystalDevice_t device = lq_init(0x27, 20, 4, LCD_5x8DOTS);
	lq_turnOnBacklight(&device);
	
	//uint8_t fsm_screen = 0;

    while (1) {
        // Main loop can perform other tasks
		if (screenFlag){
			if(dac_screen){
					lq_clear(&device);
					lq_setCursor(&device, 1,0);
					lq_print(&device, "TONE GENERATOR MODE");
					_delay_ms(2000);
					lq_clear(&device);
					lq_setCursor(&device, 0, 0); // moving cursor to the next line
					lq_print(&device, "Amplitude");
					lq_setCursor(&device, 1, 0); // moving cursor to the next line
					lq_print(&device, "Frequency");
					
					lq_setCursor(&device, 2, 0); // moving cursor to the next line
					lq_print(&device, "Waveform");
			}
				
				if(aux_screen){
					lq_clear(&device);
					
					
					lq_setCursor(&device, 1, 0); // moving cursor to the next line
					lq_print(&device, "AUX MODE");
				}
				screenFlag = 0;
				
			}
			
			
			
			
			
			
		}
		
		
    }










//I STILL DONT KNOW WHATS THE POINT OF THIS, SHOULDNT THE RELAY SWITCH THE CIRCUITS AUTOMATICALLY, WHY AM I BEING TOLD TO DO THIS