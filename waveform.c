#include "waveform.h"
#include "i2c_master.h"  // Include I2C functions for MCP4725
#include <util/delay.h>

#define MCP4725_ADDR 0x62  // MCP4725 DAC I2C address

// Function to set the MCP4725 DAC value using I2C
void MCP4725_SetValue(uint16_t value) {
    i2c_master_start(MCP4725_ADDR, 0x00);  // Start I2C communication with write flag
    i2c_master_write((value >> 8) & 0x0F);  // Send high nibble (upper 4 bits)
    i2c_master_write(value & 0xFF);  // Send low byte
    i2c_master_stop();  // Stop I2C communication
}

// Generate a sine wave
void Generate_Sine_Wave(uint16_t amplitude, int frequency) {
    // Implement sine wave generation logic using the MCP4725_SetValue function
}

// Generate a square wave
void Generate_Square_Wave(uint16_t amplitude, int frequency) {
    // Implement square wave generation logic using the MCP4725_SetValue function
}

// Generate a triangle wave
void Generate_Triangle_Wave(uint16_t amplitude, int frequency) {
    // Implement triangle wave generation logic using the MCP4725_SetValue function
}

