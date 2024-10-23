/*
 * Buttons_with_Interrupts.c
 *
 * Created: 10/22/2024 4:29:07 PM
 * Author : david
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

ISR(PCINT2_vect) {
	// Check if button is pressed (PD2 is LOW)
	if (PIND & (1 << 2)) {
		PORTB |= (1 << PB5);
		
	}
	else{
		PORTB &= ~(1 << PB5);
	}
}

		

int main(void)
{
    /* Replace with your application code */
	DDRD &= (1<<2);		//sets pin D2 to input
	DDRB |= (1 << PB5);//sets built in LED to output
	
	 PCICR |= (1 << PCIE2);  // Enable PCINT16-23 group interrupt
	 PCMSK2 |= (1 << PCINT18);  // Enable interrupt for PD2 (PCINT18)
	 
	 sei(); //enables global interrupts
    while (1) 
    {
    }
}

