#include <avr/io.h>
#include <util/delay.h>

#define ADC_THRESHOLD 500 // Set your threshold here based on desired voltage detection

void adc_init() {
	// Set the reference voltage to AVcc (5V) and select ADC0 (PC0) as input
	ADMUX = (1 << REFS0); // AVcc with external capacitor at AREF pin
	ADCSRA = (1 << ADEN)  // Enable ADC
	ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler of 128 for 16 MHz
}

uint16_t adc_read() {
	 //ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1 << ADSC); // Start conversion
	while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
	return ADC; // ADC result is a 10-bit value (0 to 1023)
}

uint8_t isAuxConnected() {
	uint16_t adc_value = adc_read();
	return (adc_value > ADC_THRESHOLD); // Return true if above threshold
}

int main() {
	adc_init();
	DDRB |= (1 << PB2); // Set PB2 as output for the relay control

	while (1) {
		if (isAuxConnected()) {
			PORTB |= (1 << PB2); // Set PB2 high to activate AUX
			} else {
			PORTB &= ~(1 << PB2); // Set PB2 low to activate DAC
		}
		_delay_ms(100); // Small delay to debounce
	}
}