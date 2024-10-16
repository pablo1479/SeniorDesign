#ifndef WAVEFORM_H
#define WAVEFORM_H


#include <avr/io.h> 

void Generate_Sine_Wave(uint16_t amplitude, int frequency);
void Generate_Square_Wave(uint16_t amplitude, int frequency);
void Generate_Triangle_Wave(uint16_t amplitude, int frequency);
void Generate_Ramp_Wave(uint16_t amplitude, int frequency);

#endif
