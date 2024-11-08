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
//#include "waveform.h"

volatile uint8_t selectPressed = 0;
volatile uint8_t upPressed = 0;
volatile uint8_t downPressed = 0;

volatile uint8_t waveform_id = 0;
volatile uint8_t dac_state = 0;


const uint16_t sine_lut[256] = {	2047, 2097, 2147, 2198, 2248, 2298, 2347, 2397, 2446, 2496, 2544, 2593, 2641, 2689,
	2737, 2784, 2830, 2877, 2922, 2967, 3012, 3056, 3099, 3142, 3184, 3226, 3266, 3306,
	3346, 3384, 3422, 3458, 3494, 3530, 3564, 3597, 3629, 3661, 3691, 3721, 3749, 3776,
	3803, 3828, 3852, 3875, 3897, 3918, 3938, 3957, 3974, 3991, 4006, 4020, 4033, 4044,
	4055, 4064, 4072, 4079, 4084, 4088, 4092, 4093, 4094, 4093, 4092, 4088, 4084, 4079,
	4072, 4064, 4055, 4044, 4033, 4020, 4006, 3991, 3974, 3957, 3938, 3918, 3897, 3875,
	3852, 3828, 3803, 3776, 3749, 3721, 3691, 3661, 3629, 3597, 3564, 3530, 3494, 3458,
	3422, 3384, 3346, 3306, 3266, 3226, 3184, 3142, 3099, 3056, 3012, 2967, 2922, 2877,
	2830, 2784, 2737, 2689, 2641, 2593, 2544, 2496, 2446, 2397, 2347, 2298, 2248, 2198,
	2147, 2097, 2047, 1997, 1947, 1896, 1846, 1796, 1747, 1697, 1648, 1598, 1550, 1501,
	1453, 1405, 1357, 1310, 1264, 1217, 1172, 1127, 1082, 1038, 995, 952, 910, 868, 828,
	788, 748, 710, 672, 636, 600, 564, 530, 497, 465, 433, 403, 373, 345, 318, 291, 266,
	242, 219, 197, 176, 156, 137, 120, 103, 88, 74, 61, 50, 39, 30, 22, 15, 10, 6, 2, 1, 0,
	1, 2, 6, 10, 15, 22, 30, 39, 50, 61, 74, 88, 103, 120, 137, 156, 176, 197, 219, 242, 266,
	291, 318, 345, 373, 403, 433, 465, 497, 530, 564, 600, 636, 672, 710, 748, 788, 828, 868,
	910, 952, 995, 1038, 1082, 1127, 1172, 1217, 1264, 1310, 1357, 1405, 1453, 1501, 1550, 1598,
1648, 1697, 1747, 1796, 1846, 1896, 1947, 1997};
const uint16_t triangle_lut [256] = {	0, 32, 64, 97, 129, 161, 193, 226, 258, 290, 322, 355, 387, 419, 451, 484, 516, 548, 580,
	613, 645, 677, 709, 742, 774, 806, 838, 871, 903, 935, 967, 1000, 1032, 1064, 1096, 1129,
	1161, 1193, 1225, 1258, 1290, 1322, 1354, 1386, 1419, 1451, 1483, 1515, 1548, 1580, 1612,
	1644, 1677, 1709, 1741, 1773, 1806, 1838, 1870, 1902, 1935, 1967, 1999, 2031, 2064, 2096,
	2128, 2160, 2193, 2225, 2257, 2289, 2322, 2354, 2386, 2418, 2451, 2483, 2515, 2547, 2580,
	2612, 2644, 2676, 2709, 2741, 2773, 2805, 2837, 2870, 2902, 2934, 2966, 2999, 3031, 3063,
	3095, 3128, 3160, 3192, 3224, 3257, 3289, 3321, 3353, 3386, 3418, 3450, 3482, 3515, 3547,
	3579, 3611, 3644, 3676, 3708, 3740, 3773, 3805, 3837, 3869, 3902, 3934, 3966, 3998, 4031,
	4063, 4095, 4063, 4031, 3999, 3967, 3935, 3903, 3871, 3839, 3807, 3775, 3743, 3711, 3679,
	3647, 3615, 3583, 3551, 3519, 3487, 3455, 3423, 3391, 3359, 3327, 3295, 3263, 3231, 3199,
	3167, 3135, 3103, 3071, 3039, 3007, 2975, 2943, 2911, 2879, 2847, 2815, 2783, 2751, 2719,
	2687, 2655, 2623, 2591, 2559, 2527, 2495, 2463, 2431, 2399, 2367, 2335, 2303, 2271, 2239,
	2207, 2175, 2143, 2111, 2079, 2048, 2016, 1984, 1952, 1920, 1888, 1856, 1824, 1792, 1760,
	1728, 1696, 1664, 1632, 1600, 1568, 1536, 1504, 1472, 1440, 1408, 1376, 1344, 1312, 1280,
	1248, 1216, 1184, 1152, 1120, 1088, 1056, 1024, 992, 960, 928, 896, 864, 832, 800, 768, 736,
	704, 672, 640, 608, 576, 544, 512, 480, 448, 416, 384, 352, 320, 288, 256, 224, 192, 160, 128,
96, 64, 32, 0};
uint8_t INCR = 1;
volatile uint16_t vol_sq = 4095;
volatile uint8_t i = 0;

uint8_t vol_num = 50;
int main(void)
{
    /* Replace with your application code */
	DDRD &= (1<<2);		//select button
	DDRD &= (1<<4);		//up button
	DDRD &= (1<<7);		//down button
	
			//amplitude of the tone generator
	uint8_t fsm = 0;			//state machine that determines what setting the user is on
	char vol_str[4];
	
	char freq_str[5];
	char waveform[3][9] = {"Sine", "Square", "Triangle"};
	
	const uint16_t frequency[] = {48,96,192,384, 768, 1536};
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
	 
	 lq_setCursor(&device, 0, 9);
	 lq_print(&device, "<");
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
	 
	 //int freq = 100;			//adjust this to adjust the frequency of the square wave
	 
	 TCCR1B |= (1 << WGM12);	
	 TIMSK1 |= (1 << OCIE1A);
	 sei();
	 
	 
	 uint16_t freq_sq = 20;
	 double period = (1.0/freq_sq) / 2;
	 uint16_t timer = period *(16000000.0 / 256.0) - 1;
	TCCR1B |= (1 << CS10);// | (1<<CS12);													// prescaler 1024
	OCR1A = 1; //prescaler 256
	
	while (1) 
    {
		/*
				Switch Case for Waveform ID that generates waveforms using the DAC
		*/
		switch(waveform_id){
			case 0: //Sine waveform
				//Generate_Sine_Wave(vol_num, frequency[freq_id]);
			break;
			
			case 1: //Square waveform
				//Generate_Square_Wave(vol_num, frequency[freq_id]);
				
				
				
			break;
			
			case 2: //Triangle waveform
				//Generate_Triangle_Wave(vol_num, frequency[freq_id]);
			
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
					lq_print(&device, "Up    Down      Back");
				break;
				
				case 1:
					fsm = 4;
					lq_setCursor(&device, 1, 12);
					lq_print(&device, ">");
					lq_setCursor(&device, 1, 9);
					lq_print(&device, " ");
					lq_setCursor(&device, 3, 0);
					lq_print(&device, "Up    Down      Back");
					
					
					
				break;
				
				case 2:
					fsm = 5;
					lq_setCursor(&device, 2, 11);
					lq_print(&device, ">");
					lq_setCursor(&device, 2, 8);
					lq_print(&device, " ");
					lq_setCursor(&device, 3, 0);
					lq_print(&device, "Up    Down      Back");
				
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
					vol_sq = (uint16_t)(vol_num * 40.95);
					
					
					
					
				break;
				
				case 4:
					
					if (freq_id == 5) {
						freq_id = 0;
						} else {
							freq_id += 1;
						}
					lq_setCursor(&device, 1, 14);
					
					switch(freq_id) {
						case 0: case 1:
						lq_print(&device, "  ");
						break;
						case 2: case 3: case 4:
						lq_print(&device, " ");
						break;
						
					}
					sprintf(freq_str, "%d", frequency[freq_id]);
					lq_print(&device, freq_str);
					i = 0;
					
					if(waveform_id ==1){
						TCCR1B &= ~(1 << CS12) & ~(1 << CS11) & ~(1 << CS10);
						TCCR1B |= (1 << CS12); // Prescaler 256
						freq_sq = frequency[freq_id];
						period = (1.0/freq_sq)/2;
						timer = period *(16000000.0 / 256.0) - 1;
						OCR1A = timer;
						
						}
					else{
						TCCR1B &= ~(1 << CS12) & ~(1 << CS11) & ~(1 << CS10);
						TCCR1B |= (1 << CS10);// | (1<<CS12);													// prescaler 1
						OCR1A = 1;  // (16e6 / (64 * 1000)) - 1
						switch(freq_id){
							case 0:
								INCR = 1;
							break;
							case 1:
								INCR = 2;
							break;
							case 2:
								INCR = 4;
							break;
							case 3:
								INCR = 8;
							break;
							case 4:
								INCR = 16;
							break;
							case 5:
								INCR = 32;
							break;
							
						}
					}
						
					
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
					
					i = 0;
					if(waveform_id ==1){
						TCCR1B &= ~(1 << CS12) & ~(1 << CS11) & ~(1 << CS10);
						TCCR1B |= (1 << CS12); // Prescaler 256
						freq_sq = frequency[freq_id];
						period = (1.0/freq_sq)/2;
						timer = period *(16000000.0 / 256.0) - 1;
						OCR1A = timer;
						
					}
					else{
						TCCR1B &= ~(1 << CS12) & ~(1 << CS11) & ~(1 << CS10);
						TCCR1B |= (1 << CS10);// | (1<<CS12);													// prescaler 1024
						OCR1A = 1;  // (16e6 / (64 * 1000)) - 1
						switch(freq_id){
							case 0:
							INCR = 1;
							break;
							
							case 1:
							INCR = 2;
							break;
							
							case 2:
							INCR = 4;
							break;
							
							case 3:
							INCR = 8;
							break;
							
							case 4:
							INCR = 16;
							break;
							
							case 5:
							INCR = 32;
							break;
							
						}
					}
					break;
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
					vol_sq = (uint16_t)(vol_num * 40.95);
					
					
				break;
				
				case 4:
					if (freq_id == 0) {
						freq_id = 5;
						} else {
						freq_id -= 1;
					}
					lq_setCursor(&device, 1, 14);
					switch(freq_id) {
						case 0: case 1:
						lq_print(&device, "  ");
						break;
						case 2: case 3: case 4:
						lq_print(&device, " ");
						break;
						
					}
					sprintf(freq_str, "%d", frequency[freq_id]);
					lq_print(&device, freq_str);
					i = 0;
					if(waveform_id ==1){
						TCCR1B &= ~(1 << CS12) & ~(1 << CS11) & ~(1 << CS10);
						TCCR1B |= (1 << CS12); // Prescaler 256
						freq_sq = frequency[freq_id];
						period = (1.0/freq_sq)/2;
						timer = period *(16000000.0 / 256.0) - 1;
						OCR1A = timer;
						
					}
					else{
						TCCR1B &= ~(1 << CS12) & ~(1 << CS11) & ~(1 << CS10);
						TCCR1B |= (1 << CS10);// | (1<<CS12);													// prescaler 1024
						OCR1A = 1;  // (16e6 / (64 * 1000)) - 1
						switch(freq_id){
							case 0:
								INCR = 1;
							break;
							
							case 1:
								INCR = 2;
							break;
							
							case 2:
								INCR = 4;
							break;
							
							case 3:
								INCR = 8;
							break;
							
							case 4:
								INCR = 16;
							break;
							
							case 5:
								INCR = 32;
							break;
							
						}
					}
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
					
					i = 0;
					if(waveform_id ==1){
						TCCR1B &= ~(1 << CS12) & ~(1 << CS11) & ~(1 << CS10);
						TCCR1B |= (1 << CS12); // Prescaler 256
						freq_sq = frequency[freq_id];
						period = (1.0/freq_sq)/2;
						timer = period *(16000000.0 / 256.0) - 1;
						OCR1A = timer;
						
					}
					else{
						TCCR1B &= ~(1 << CS12) & ~(1 << CS11) & ~(1 << CS10);
						TCCR1B |= (1 << CS10);// | (1<<CS12);													// prescaler 1024
						OCR1A = 1;  // (16e6 / (64 * 1000)) - 1
						switch(freq_id){
							case 0:
							INCR = 1;
							break;
							
							case 1:
							INCR = 2;
							break;
							
							case 2:
							INCR = 4;
							break;
							
							case 3:
							INCR = 8;
							break;
							
							case 4:
							INCR = 16;
							break;
							
							case 5:
							INCR = 32;
							break;
							
						}
					}
					break;
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
			MCP4725_SetValue(vol_sq);
			dac_state = 1;
		}
		else if(dac_state ==1){
			MCP4725_SetValue(0);
			dac_state = 0;
			
		}
		break;
		case 2:
			//MCP4725_SetValue(triangle_lut[i]);
			MCP4725_SetValue(triangle_lut[i] / 100 * vol_num);
			i+=INCR;
		break;
		case 0:
			MCP4725_SetValue(sine_lut[i] / 100 * vol_num);
			i+=INCR;
		break;
		
		
	}
}
