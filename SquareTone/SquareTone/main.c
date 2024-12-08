/*
 * Senior_Design_LCD.c
 *
 * Created: 10/23/2024 10:57:10 PM
 * Author : david
 */ 

#define F_CPU 8000000UL  // Define CPU clock as 8 MHz
#include <avr/io.h>        // Include standard I/O library for AVR
#include <util/delay.h>    // Include delay library
#include "liquid_crystal_i2c.h"			//update this library to use the DAC
#include "i2c_master.h"
#include <stdio.h>
#include <avr/interrupt.h>
//#include "waveform.h"

#define down PD7
#define up PD4
#define select PD2

volatile uint8_t selectPressed = 0;
volatile uint8_t upPressed = 0;
volatile uint8_t downPressed = 0;

volatile uint8_t dac_state = 0;

uint8_t vol_num = 50;
volatile uint16_t vol_sq;
int main(void)
{
    /* Replace with your application code */
	DDRD &= (1<<select);		//select button
	DDRD &= (1<<up);		//up button
	DDRD &= (1<<down);		//down button
	
	
	
	//amplitude of the tone generator
	uint8_t fsm = 0;			//state machine that determines what setting the user is on
	char vol_str[4];
	vol_sq = (vol_num * 40.95);
	
	char freq_str[5];
	
	
	const uint16_t frequency[] = {20, 120, 220, 320, 420, 520, 620, 720, 820, 1000};
	uint8_t freq_id = 0;
	
													
	
	i2c_master_init(I2C_SCL_FREQUENCY_100);					//SET LCD TO I2C1 PINS
	LiquidCrystalDevice_t device = lq_init(0x27, 20, 4, LCD_5x8DOTS);
	lq_turnOnBacklight(&device);
	  
	 
	
	 
	PCICR |= (1 << PCIE2);  // Enable Pin Change Interrupt for PCINT16-23 group (Port D)
	
	PCMSK2 |=  (1 << PCINT18) | (1 << PCINT20) | (1 << PCINT23); //Enables interrupts for PD2, PD4, and PD7		UPDATE: PD1
	sei();
	
	I2C_Init(); //initiates the DAC 
	TCCR1B |= (1 << WGM12);	
	TIMSK1 |= (1 << OCIE1A);
	TCCR1B |= (1 << CS12); // Prescaler 256
	sei();
	 
	 //CALCULATES CLOCK FREQUENCY FOR INITIAL VALUES
	 uint16_t freq_sq = 46;						//Initial value
	 double period = (1.0/freq_sq) / 2;
	 uint16_t timer = period *(8000000.0 / 256.0) - 1;
	
	

	
	freq_sq = frequency[freq_id];
	period = (1.0/freq_sq)/2;
	timer = period *(8000000.0 / 256.0) - 1;
	OCR1A = timer;
	
	//SETTINGS
	lq_setCursor(&device, 0, 0);
	lq_print(&device, "Amplitude");
	lq_setCursor(&device, 1, 0);
	lq_print(&device, "Frequency");
	
	
	//AMPLITUDE
	vol_num = 50;
	lq_setCursor(&device, 0, 17);
	sprintf(vol_str, "%d", vol_num);
	lq_print(&device, vol_str);
	lq_print(&device, " ");
	
	//CURSOR
	lq_setCursor(&device, 0, 9);
	lq_print(&device, "<");
	
	//FREQUENCY
	freq_id = 0;
	lq_setCursor(&device, 1, 14);
	lq_print(&device, "  ");
	sprintf(freq_str, "%d", frequency[freq_id]);
	lq_print(&device, freq_str);
	
	lq_setCursor(&device, 1, 18);
	lq_print(&device, "Hz");
	
	
	lq_print(&device, "Up    Down    Select");
	
	
	
	
	while (1) 
    {
				
		if(selectPressed){
			switch(fsm){
				case 0:
					fsm = 2;
					lq_setCursor(&device, 0, 16);
					lq_print(&device, ">");
					lq_setCursor(&device, 0, 9);
					lq_print(&device, " ");
					lq_setCursor(&device, 3, 16);
					lq_print(&device, "Back");
				break;
				
				case 1:
					fsm = 3;
					lq_setCursor(&device, 1, 12);
					lq_print(&device, ">");
					lq_setCursor(&device, 1, 9);
					lq_print(&device, " ");
					lq_setCursor(&device, 3, 16);
					lq_print(&device, "Back");
					
					
					
				break;
				
				
				
				case 2:
					fsm = 0;
					lq_setCursor(&device, 0, 9);
					lq_print(&device, "<");
					
					lq_setCursor(&device, 0, 16);
					lq_print(&device, " ");
					lq_setCursor(&device, 3, 14);
					lq_print(&device, "Select");
					
				break;
				
				case 3:
					fsm = 1;
					lq_setCursor(&device, 1, 9);
					lq_print(&device, "<");
					
					lq_setCursor(&device, 1, 12);
					lq_print(&device, " ");
					lq_setCursor(&device, 3, 14);
					lq_print(&device, "Select");
				break;
				

			}
		
			selectPressed = 0;
		}
		
		
		if(upPressed){
			switch(fsm){
				case 0:
					fsm = 1;
					lq_setCursor(&device, 0, 9);
					lq_print(&device, " ");
					
					lq_setCursor(&device, 1, 9);
					lq_print(&device, "<");
					
				break;
				
				case 1:
					fsm = 0;
					lq_setCursor(&device, 0, 9);
					lq_print(&device, "<");
					lq_setCursor(&device, 1, 9);
					lq_print(&device, " ");
					
				break;
				
				
				case 2:
					if(vol_num<100){
						vol_num += 1;
						lq_setCursor(&device, 0, 17);
						sprintf(vol_str, "%d", vol_num);
						lq_print(&device, vol_str);
						if (vol_num < 10) {
							lq_print(&device, "  ");
							} else if (vol_num < 100) {
									lq_print(&device, " ");
						}
					}
					vol_sq = (uint16_t)(vol_num * 40.95);
					
					
					
					
				break;
				
				case 3:
					
					if (freq_id == 9) {
						freq_id = 0;
						} else {
							freq_id += 1;
						}
					lq_setCursor(&device, 1, 14);
					
					switch(freq_id) {
						case 0:
						lq_print(&device, "  ");
						break;
						case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: 
						lq_print(&device, " ");
						break;
						
					}
					sprintf(freq_str, "%d", frequency[freq_id]);
					lq_print(&device, freq_str);
					
							
					freq_sq = frequency[freq_id];
					period = (1.0/freq_sq)/2;
					timer = period *(16000000.0 / 256.0) - 1;
					OCR1A = timer;	
					
				break;
				
				
			
			
		}
		upPressed = 0;
		}
		
		
		if(downPressed){
			switch(fsm){
				case 0:
					fsm = 1;
					lq_setCursor(&device, 0, 9);
					lq_print(&device, " ");
					
					lq_setCursor(&device, 1, 9);
					lq_print(&device, "<");
				
				break;
				
				case 1:
					fsm = 0;
					lq_setCursor(&device, 0, 9);
					lq_print(&device, "<");
					lq_setCursor(&device, 1, 9);
					lq_print(&device, " ");
				
				break;
				
				
				
				case 2:
					if(vol_num > 0){
						vol_num -= 1;
						lq_setCursor(&device, 0, 17);
						sprintf(vol_str, "%d", vol_num);
						lq_print(&device, vol_str);
						if (vol_num < 10) {
							lq_print(&device, "  ");
							} else if (vol_num < 100) {
							lq_print(&device, " ");
						}
					}
					vol_sq = (uint16_t)(vol_num * 40.95);
					
					
				break;
				
				case 3:
					if (freq_id == 0) {
						freq_id = 9;
						} else {
						freq_id -= 1;
					}
					lq_setCursor(&device, 1, 14);
					switch(freq_id) {
						case 0:
						lq_print(&device, "  ");
						break;
						case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
						lq_print(&device, " ");
						break;
						
					}
					sprintf(freq_str, "%d", frequency[freq_id]);
					lq_print(&device, freq_str);
					
					freq_sq = frequency[freq_id];
					period = (1.0/freq_sq)/2;
					timer = period *(16000000.0 / 256.0) - 1;
					OCR1A = timer;
					
				break;
				
				
			
			
		}
		downPressed = 0;
    }
}
	}




ISR(PCINT2_vect) {
	
	if  (PIND & (1 << select)) {
		_delay_ms(50);
		while(PIND & (1 << select)) {}
		selectPressed = 1;  // Set flag for PD2 button press
	}
	
	if  (PIND & (1 << up)) {
		_delay_ms(50);
		while(PIND & (1 << up)) {}
		upPressed = 1;  // Set flag for PD4 button press
	}
	
	if  (PIND & (1 << down)) {
		_delay_ms(50);
		while(PIND & (1 << down)) {}
		downPressed = 1;  // Set flag for PD7 button press
	}
	
	
}


ISR(TIMER1_COMPA_vect) {
		if(dac_state ==0){
			MCP4725_SetValue(vol_sq);
			dac_state = 1;
		}
		else if(dac_state ==1){
			MCP4725_SetValue(0);
			dac_state = 0;
			
		}
	
		
		
	}
