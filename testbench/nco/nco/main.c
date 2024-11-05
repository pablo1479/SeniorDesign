/*
 * nco.c
 *
 * Created: 11/5/2024 2:07:49 AM
 * Author : david
 */ 

#include <avr/io.h>
#define SCL_CLOCK 400000L
#define MCP4725_ADDR 0x62
void I2C_Init(void) {
	// Set the bit rate for 400 kHz I2C
	TWSR1 = 0x00;  // Prescaler set to 1
	TWBR1 = ((F_CPU / SCL_CLOCK) - 16) / 2;  // Set bit rate register for 400kHz
}

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
const uint16_t sine_lut[256] = [	2047, 2097, 2147, 2198, 2248, 2298, 2347, 2397, 2446, 2496, 2544, 2593, 2641, 2689,
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
									1648, 1697, 1747, 1796, 1846, 1896, 1947, 1997];
int main(void)
{
    /* Replace with your application code */
	I2C_Init();
	TCCR1B |= (1 << WGM12);													//CTC Mode
	TIMSK1 |= (1 << OCIE1A);
	
	TCCR1B |= (1 << CS12);				// prescaler 256
    while (1) 
    {
    }
}
volatile uint8_t i;
ISR(TIMER1_COMPA_vect) {
	MCP4725_SetValue(i);
	MCP4725_SetValue(sine_lut[i]);
	i +=1;
}