#include <avr/io.h>
#include <util/delay.h>



void setup() {
	DDRD &= ~(1 << PD0); // Set PD0 as input
	PORTD |= (1 << PD0); // Enable internal pull-up resistor
}

int main() {
	setup();
	DDRB |= (1 << PB5);
	
	 PCICR |= (1 << PCIE2);
	 PCMSK2 |= (1 << PCINT16);

	 // Enable global interrupts
	 sei();

	while (1) {
		
	}
}
ISR(PCINT2_vect) {
	// Check if the interrupt is caused by the SWITCH_PIN
	if (!(PIND & (1 << PD0))) { // If PD0 is LOW (switch pressed)
		PORTB |= (1 << PB5);
	}
	else { // If PD0 is HIGH (switch released)
		PORTB &= ~(1 << PB5); // Turn OFF LED
	}
}
