/*
 * Senior_Design_LCD.c
 *
 * Created: 10/23/2024 10:57:10 PM
 * Author : david
 */ 

#define F_CPU 16000000UL  // Define CPU clock as 16 MHz
#include <avr/io.h>        // Include standard I/O library for AVR
#include <util/delay.h>    // Include delay library
#include "liquid_crystal_i2c.h"			//update this library to use the DAC
#include "i2c_master.h"
#include <stdio.h>
#include <avr/interrupt.h>
#include "waveform.h"

volatile uint8_t selectPressed = 0;
volatile uint8_t upPressed = 0;
volatile uint8_t downPressed = 0;

volatile uint8_t waveform_id = 0;
volatile uint8_t dac_state = 0;
int main(void)
{
    /* Replace with your application code */
	DDRD &= (1<<2);		//select button
	DDRD &= (1<<4);		//up button
	DDRD &= (1<<7);		//down button
	
	uint8_t vol_num = 50;		//amplitude of the tone generator
	uint8_t fsm = 0;			//state machine that determines what setting the user is on
	char vol_str[4];
	char freq_str[5];
	char waveform[3][9] = {"Sine", "Square", "Triangle"};
	
	const uint16_t frequency[] = {20, 50, 100, 200, 300, 500, 700, 1000, 1200, 1500};
	uint8_t freq_id = 0;
	uint8_t screen = 0;
	
	i2c_master_init(I2C_SCL_FREQUENCY_100);					//SET LCD TO I2C0 PINS
	LiquidCrystalDevice_t device = lq_init(0x27, 20, 4, LCD_5x8DOTS);
	lq_turnOnBacklight(&device);
	
	 lq_setCursor(&device, 0, 0);
	 lq_print(&device, "Amplitude");
	 lq_setCursor(&device, 1, 0);
	 lq_print(&device, "Frequency");
	 lq_setCursor(&device, 2, 0);
	 lq_print(&device, "Waveform");
	 
	 PCICR |= (1 << PCIE2);  // Enable Pin Change Interrupt for PCINT16-23 group (Port D)
	 PCMSK2 |= (1 << PCINT18) | (1 << PCINT20) | (1 << PCINT23); //Enables interrupts for PD2, PD4, and PD7
	 sei();
	 
	 I2C_Init(); //initiates the DAC

	
     lq_setCursor(&device, 0, 17);
     sprintf(vol_str, "%d", vol_num);
     lq_print(&device, vol_str);
	 lq_print(&device, " ");
	 // Display Frequency
	 

	 lq_setCursor(&device, 1, 14);
	 lq_print(&device, "  ");
	 sprintf(freq_str, "%d", frequency[freq_id]);
	 lq_print(&device, freq_str);
	 
	 lq_setCursor(&device, 1, 18);
	 lq_print(&device, "Hz");
	 
	 //Display Waveform
	 lq_setCursor(&device, 2, 12);    
     lq_print(&device, "    ");  
     lq_print(&device, waveform[waveform_id]);
	 
	 lq_setCursor(&device, 3, 0);
	 lq_print(&device, "Up    Down    Select");
	 
	 int freq = 100;			//adjust this to adjust the frequency of the square wave
	 double period = (1/freq)/2;
	 TCCR1B |= (1 << WGM12);	
	 TIMSK1 |= (1 << OCIE1A);
	 sei();
	 
	 OCR1A = (int)period * 16000000 / 256;  // (16e6 / (64 * 1000)) - 1								this should be 5 ms
	 TCCR1B |= (1 << CS12);
	
	while (1) 
    {
		/*
				Switch Case for Waveform ID that generates waveforms using the DAC
		*/
		switch(waveform_id){
			case 0: //Sine waveform
				Generate_Sine_Wave(vol_num, frequency[freq_id]);
			break;
			
			case 1: //Square waveform
				//Generate_Square_Wave(vol_num, frequency[freq_id]);
				
				
				
			break;
			
			case 2: //Triangle waveform
				Generate_Triangle_Wave(vol_num, frequency[freq_id]);
			
			break;
		}
		
		if(selectPressed){
			switch(fsm){
				case 0:
					fsm = 3;
					lq_setCursor(&device, 0, 16);
					lq_print(&device, ">");
					lq_setCursor(&device, 0, 9);
					lq_print(&device, " ");
					lq_setCursor(&device, 3, 0);
					lq_print(&device, "Up    Down    Back");
				break;
				
				case 1:
					fsm = 4;
					lq_setCursor(&device, 1, 12);
					lq_print(&device, ">");
					lq_setCursor(&device, 1, 9);
					lq_print(&device, " ");
					lq_setCursor(&device, 3, 0);
					lq_print(&device, "Up    Down    Back");
					
					
					
				break;
				
				case 2:
					fsm = 5;
					lq_setCursor(&device, 2, 11);
					lq_print(&device, ">");
					lq_setCursor(&device, 2, 8);
					lq_print(&device, " ");
					lq_setCursor(&device, 3, 0);
					lq_print(&device, "Up    Down    Back");
				
				break;
				
				case 3:
					fsm = 0;
					lq_setCursor(&device, 0, 9);
					lq_print(&device, "<");
					
					lq_setCursor(&device, 0, 16);
					lq_print(&device, " ");
					lq_setCursor(&device, 3, 0);
					lq_print(&device, "Up    Down    Select");
					
				break;
				
				case 4:
					fsm = 1;
					lq_setCursor(&device, 1, 9);
					lq_print(&device, "<");
					
					lq_setCursor(&device, 1, 12);
					lq_print(&device, " ");
					lq_setCursor(&device, 3, 0);
					lq_print(&device, "Up    Down    Select");
				break;
				
				case 5:
					fsm = 2;
					
					lq_setCursor(&device, 2, 8);
					lq_print(&device, "<");
					lq_setCursor(&device, 2, 11);
					lq_print(&device, " ");
					lq_setCursor(&device, 3, 0);
					lq_print(&device, "Up    Down    Select");
				break;
			}
		
			selectPressed = 0;
		}
		
		
		if(upPressed){
			switch(fsm){
				case 0:
					fsm = 2;
					lq_setCursor(&device, 0, 9);
					lq_print(&device, " ");
					
					lq_setCursor(&device, 2, 8);
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
					fsm = 1;
					
					lq_setCursor(&device, 1, 9);
					lq_print(&device, "<");
					lq_setCursor(&device, 2, 8);
					lq_print(&device, " ");
					
				
				break;
				
				case 3:
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
					
					
				break;
				
				case 4:
					
					if (freq_id == 9) {
						freq_id = 0;
						} else {
							freq_id += 1;
						}
					lq_setCursor(&device, 1, 14);
					switch(freq_id) {
						case 0: case 1:
						lq_print(&device, "  ");
						break;
						case 2: case 3: case 4: case 5: case 6:
						lq_print(&device, " ");
						break;
						
					}
					sprintf(freq_str, "%d", frequency[freq_id]);
					lq_print(&device, freq_str);
						
					
				break;
				
				case 5:
					if (waveform_id == 2) {
						waveform_id = 0;
						} else {
						waveform_id += 1;
					}
					
					lq_setCursor(&device, 2, 12);
					
					switch(waveform_id) {
						case 0:  // Sine
							lq_print(&device, "    ");
							break;
						case 1:  // Square
							lq_print(&device, "  ");
							break;
					
					}
					
					lq_print(&device, waveform[waveform_id]);
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
					fsm = 2;
					
					lq_setCursor(&device, 1, 9);
					lq_print(&device, " ");
					lq_setCursor(&device, 2, 8);
					lq_print(&device, "<");
					
				break;
				
				case 2:
					fsm = 0;
					lq_setCursor(&device, 0, 9);
					lq_print(&device, "<");
					
					lq_setCursor(&device, 2, 8);
					lq_print(&device, " ");
					
				
				break;
				
				case 3:
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
					
					
					
				break;
				
				case 4:
					if (freq_id == 0) {
						freq_id = 9;
						} else {
						freq_id -= 1;
					}
					lq_setCursor(&device, 1, 14);
					switch(freq_id) {
						case 0: case 1:
						lq_print(&device, "  ");
						break;
						case 2: case 3: case 4: case 5: case 6:
						lq_print(&device, " ");
						break;
						
					}
					sprintf(freq_str, "%d", frequency[freq_id]);
					lq_print(&device, freq_str);
				break;
				
				case 5:
					if (waveform_id == 0) {
						waveform_id = 2;
						} else {
						waveform_id -= 1;
					}
					
					lq_setCursor(&device, 2, 12);
					
					switch(waveform_id) {
						case 0:  // Sine
						lq_print(&device, "    ");
						break;
						case 1:  // Square
						lq_print(&device, "  ");
						break;
						
					}
					
					lq_print(&device, waveform[waveform_id]);
				break;
				
			}
			
			downPressed = 0;
		}
    }
}




ISR(PCINT2_vect) {
	
	if  (PIND & (1 << 2)) {
		_delay_ms(50);
		while(PIND & (1 << 2)) {}
		selectPressed = 1;  // Set flag for PD2 button press
	}
	
	if  (PIND & (1 << 4)) {
		_delay_ms(50);
		while(PIND & (1 << 4)) {}
		upPressed = 1;  // Set flag for PD4 button press
	}
	
	if  (PIND & (1 << 7)) {
		_delay_ms(50);
		while(PIND & (1 << 7)) {}
		downPressed = 1;  // Set flag for PD7 button press
	}
}

ISR(TIMER1_COMPA_vect) {
	switch(waveform_id){
		case 1:
		if(dac_state ==0){
			MCP4725_SetValue(4095);
			dac_state = 1;
		}
		else if(dac_state ==1){
			MCP4725_SetValue(0);
			dac_state = 0;
			
		}
		break;
	}
}
