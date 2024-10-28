/*
 * TimerInterrupts_LED.c
 *
 * Created: 10/28/2024 1:40:08 AM
 * Author : david
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>


#define ON_TIME  500       // Time in milliseconds to keep the LED ON
#define OFF_TIME 500       // Time in milliseconds to keep the LED OFF

volatile uint8_t led_state = 0; // 0 = OFF, 1 = ON
volatile uint16_t timer_count = 0;

void init_timer1() {
	// Set up CTC mode (Clear Timer on Compare Match)
	
}

ISR(TIMER1_COMPA_vect) {
	timer_count +=1;
	
	if (timer_count == 500){
		if (led_state == 1) {
			PORTB |= (1 << PB5); // Turn off LED
			led_state = 0;
		
			} else if (led_state == 0) {
			PORTB &= ~(1 << PB5);
			led_state = 1;
		
		}
	}
	timer_count = 0;
}

int main(void) {
	// Configure LED pin as output
	DDRB |= (1 << PB5);

	// Initialize Timer1
	//init_timer1();
	
	TCCR1B |= (1 << WGM12);											//CTC Mode

	// Set the compare value for 1 ms at 16 MHz with a 64 prescaler
	OCR1A = 62;  // (16e6 / (64 * 1000)) - 1

	// Enable output compare A match interrupt
	TIMSK1 |= (1 << OCIE1A);

	// Set prescaler to 256 and start the timer
	TCCR1B |= (1 << CS12);

	// Enable global interrupts
	sei();

	// Main loop
	while (1) {
		// Main loop can remain empty as the ISR handles the LED timing
	}
}

