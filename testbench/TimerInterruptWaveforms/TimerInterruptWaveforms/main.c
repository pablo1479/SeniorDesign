/*
 * TimerInterruptWaveforms.c
 *
 * Created: 10/28/2024 2:09:54 AM
 * Author : david
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#define SCL_CLOCK 400000L
#define MCP4725_ADDR 0x62  // MCP4725 DAC I2C address

volatile uint16_t dac_state = 0;

void I2C_Init(void) {
	// Set the bit rate for 400 kHz I2C
	TWSR1 = 0x00;  // Prescaler set to 1
	TWBR1 = ((F_CPU / SCL_CLOCK) - 16) / 2;  // Set bit rate register for 400kHz
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

ISR(TIMER1_COMPA_vect) {
	if(dac_state ==0){
		MCP4725_SetValue(4095);
		dac_state = 1;
	}
	else if(dac_state ==1){
		MCP4725_SetValue(0);
		dac_state = 0;
	
	}
}
int main(void)
{
    /* Replace with your application code */
	
	I2C_Init();
	TCCR1B |= (1 << WGM12);											//CTC Mode

	
	OCR1A = 311;  // (16e6 / (64 * 1000)) - 1								this should be 5 ms

	// Enable output compare A match interrupt
	TIMSK1 |= (1 << OCIE1A);

	// Set prescaler to 256 and start the timer
	TCCR1B |= (1 << CS12);

	// Enable global interrupts
	sei();
	
	
    while (1) 
    {		
    }
}

