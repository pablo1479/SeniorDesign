/*
 * Buttons.c
 *	connect the button to pin 2
	connector 1 to Vcc
	connector 2 to digital pin and ground w/ 10k resistor
	built-in LED should flash on microcontroller
	
	
 * Created: 10/21/2024 3:16:40 AM
 * Author : david
 */ 

#include <avr/io.h>


int main(void)
{
    /* Replace with your application code */
	DDRD &= (1<<2);
	DDRB |= (1 << PB5);
    while (1) 
    {
		if (PIND & (1 << 2)) {
			PORTB |= (1 << PB5);
		
		}
		else{
			PORTB &= ~(1 << PB5);
		}
		
    }
}

