#include "waveform.h"
#include "i2c_master.h"  // Include I2C functions for MCP4725
#include <util/delay.h>
#include <avr/pgmspace.h>

#define MCP4725_ADDR 0x62  // MCP4725 DAC I2C address
//#define F_CPU 16000000UL

const uint16_t sine_lut[] PROGMEM = {
	2048, 2061, 2074, 2087, 2099, 2112, 2125, 2138,
	2151, 2164, 2177, 2189, 2202, 2215, 2228, 2241,
	2254, 2266, 2279, 2292, 2305, 2317, 2330, 2343,
	2356, 2368, 2381, 2394, 2406, 2419, 2432, 2444,
	2457, 2470, 2482, 2495, 2507, 2520, 2532, 2545,
	2557, 2570, 2582, 2595, 2607, 2619, 2632, 2644,
	2656, 2669, 2681, 2693, 2705, 2717, 2730, 2742,
	2754, 2766, 2778, 2790, 2802, 2814, 2826, 2838,
	2850, 2861, 2873, 2885, 2897, 2908, 2920, 2932,
	2943, 2955, 2966, 2978, 2989, 3001, 3012, 3023,
	3035, 3046, 3057, 3068, 3079, 3091, 3102, 3113,
	3124, 3134, 3145, 3156, 3167, 3178, 3188, 3199,
	3210, 3220, 3231, 3241, 3252, 3262, 3273, 3283,
	3293, 3303, 3313, 3323, 3334, 3344, 3353, 3363,
	3373, 3383, 3393, 3402, 3412, 3422, 3431, 3441,
	3450, 3459, 3469, 3478, 3487, 3496, 3505, 3514,
	3523, 3532, 3541, 3550, 3558, 3567, 3576, 3584,
	3593, 3601, 3609, 3618, 3626, 3634, 3642, 3650,
	3658, 3666, 3674, 3682, 3690, 3697, 3705, 3712,
	3720, 3727, 3735, 3742, 3749, 3756, 3763, 3770,
	3777, 3784, 3791, 3798, 3804, 3811, 3817, 3824,
	3830, 3836, 3843, 3849, 3855, 3861, 3867, 3873,
	3879, 3884, 3890, 3896, 3901, 3907, 3912, 3917,
	3922, 3928, 3933, 3938, 3943, 3947, 3952, 3957,
	3962, 3966, 3971, 3975, 3979, 3983, 3988, 3992,
	3996, 4000, 4004, 4007, 4011, 4015, 4018, 4022,
	4025, 4028, 4032, 4035, 4038, 4041, 4044, 4047,
	4049, 4052, 4055, 4057, 4060, 4062, 4064, 4067,
	4069, 4071, 4073, 4075, 4076, 4078, 4080, 4081,
	4083, 4084, 4086, 4087, 4088, 4089, 4090, 4091,
	4092, 4093, 4093, 4094, 4095, 4095, 4095, 4096,
	4096, 4096, 4096, 4096, 4096, 4096, 4095, 4095,
	4095, 4094, 4093, 4093, 4092, 4091, 4090, 4089,
	4088, 4087, 4086, 4084, 4083, 4081, 4080, 4078,
	4076, 4075, 4073, 4071, 4069, 4067, 4064, 4062,
	4060, 4057, 4055, 4052, 4049, 4047, 4044, 4041,
	4038, 4035, 4032, 4028, 4025, 4022, 4018, 4015,
	4011, 4007, 4004, 4000, 3996, 3992, 3988, 3983,
	3979, 3975, 3971, 3966, 3962, 3957, 3952, 3947,
	3943, 3938, 3933, 3928, 3922, 3917, 3912, 3907,
	3901, 3896, 3890, 3884, 3879, 3873, 3867, 3861,
	3855, 3849, 3843, 3836, 3830, 3824, 3817, 3811,
	3804, 3798, 3791, 3784, 3777, 3770, 3763, 3756,
	3749, 3742, 3735, 3727, 3720, 3712, 3705, 3697,
	3690, 3682, 3674, 3666, 3658, 3650, 3642, 3634,
	3626, 3618, 3609, 3601, 3593, 3584, 3576, 3567,
	3558, 3550, 3541, 3532, 3523, 3514, 3505, 3496,
	3487, 3478, 3469, 3459, 3450, 3441, 3431, 3422,
	3412, 3402, 3393, 3383, 3373, 3363, 3353, 3344,
	3334, 3323, 3313, 3303, 3293, 3283, 3273, 3262,
	3252, 3241, 3231, 3220, 3210, 3199, 3188, 3178,
	3167, 3156, 3145, 3134, 3124, 3113, 3102, 3091,
	3079, 3068, 3057, 3046, 3035, 3023, 3012, 3001,
	2989, 2978, 2966, 2955, 2943, 2932, 2920, 2908,
	2897, 2885, 2873, 2861, 2850, 2838, 2826, 2814,
	2802, 2790, 2778, 2766, 2754, 2742, 2730, 2717,
	2705, 2693, 2681, 2669, 2656, 2644, 2632, 2619,
	2607, 2595, 2582, 2570, 2557, 2545, 2532, 2520,
	2507, 2495, 2482, 2470, 2457, 2444, 2432, 2419,
	2406, 2394, 2381, 2368, 2356, 2343, 2330, 2317,
	2305, 2292, 2279, 2266, 2254, 2241, 2228, 2215,
	2202, 2189, 2177, 2164, 2151, 2138, 2125, 2112,
	2099, 2087, 2074, 2061, 2048, 2035, 2022, 2009,
	1997, 1984, 1971, 1958, 1945, 1932, 1919, 1907,
	1894, 1881, 1868, 1855, 1842, 1830, 1817, 1804,
	1791, 1779, 1766, 1753, 1740, 1728, 1715, 1702,
	1690, 1677, 1664, 1652, 1639, 1626, 1614, 1601,
	1589, 1576, 1564, 1551, 1539, 1526, 1514, 1501,
	1489, 1477, 1464, 1452, 1440, 1427, 1415, 1403,
	1391, 1379, 1366, 1354, 1342, 1330, 1318, 1306,
	1294, 1282, 1270, 1258, 1246, 1235, 1223, 1211,
	1199, 1188, 1176, 1164, 1153, 1141, 1130, 1118,
	1107, 1095, 1084, 1073, 1061, 1050, 1039, 1028,
	1017, 1005, 994, 983, 972, 962, 951, 940,
	929, 918, 908, 897, 886, 876, 865, 855,
	844, 834, 823, 813, 803, 793, 783, 773,
	762, 752, 743, 733, 723, 713, 703, 694,
	684, 674, 665, 655, 646, 637, 627, 618,
	609, 600, 591, 582, 573, 564, 555, 546,
	538, 529, 520, 512, 503, 495, 487, 478,
	470, 462, 454, 446, 438, 430, 422, 414,
	406, 399, 391, 384, 376, 369, 361, 354,
	347, 340, 333, 326, 319, 312, 305, 298,
	292, 285, 279, 272, 266, 260, 253, 247,
	241, 235, 229, 223, 217, 212, 206, 200,
	195, 189, 184, 179, 174, 168, 163, 158,
	153, 149, 144, 139, 134, 130, 125, 121,
	117, 113, 108, 104, 100, 96, 92, 89,
	85, 81, 78, 74, 71, 68, 64, 61,
	58, 55, 52, 49, 47, 44, 41, 39,
	36, 34, 32, 29, 27, 25, 23, 21,
	20, 18, 16, 15, 13, 12, 10, 9,
	8, 7, 6, 5, 4, 3, 3, 2,
	1, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 1, 1, 2, 3, 3,
	4, 5, 6, 7, 8, 9, 10, 12,
	13, 15, 16, 18, 20, 21, 23, 25,
	27, 29, 32, 34, 36, 39, 41, 44,
	47, 49, 52, 55, 58, 61, 64, 68,
	71, 74, 78, 81, 85, 89, 92, 96,
	100, 104, 108, 113, 117, 121, 125, 130,
	134, 139, 144, 149, 153, 158, 163, 168,
	174, 179, 184, 189, 195, 200, 206, 212,
	217, 223, 229, 235, 241, 247, 253, 260,
	266, 272, 279, 285, 292, 298, 305, 312,
	319, 326, 333, 340, 347, 354, 361, 369,
	376, 384, 391, 399, 406, 414, 422, 430,
	438, 446, 454, 462, 470, 478, 487, 495,
	503, 512, 520, 529, 538, 546, 555, 564,
	573, 582, 591, 600, 609, 618, 627, 637,
	646, 655, 665, 674, 684, 694, 703, 713,
	723, 733, 743, 752, 762, 773, 783, 793,
	803, 813, 823, 834, 844, 855, 865, 876,
	886, 897, 908, 918, 929, 940, 951, 962,
	972, 983, 994, 1005, 1017, 1028, 1039, 1050,
	1061, 1073, 1084, 1095, 1107, 1118, 1130, 1141,
	1153, 1164, 1176, 1188, 1199, 1211, 1223, 1235,
	1246, 1258, 1270, 1282, 1294, 1306, 1318, 1330,
	1342, 1354, 1366, 1379, 1391, 1403, 1415, 1427,
	1440, 1452, 1464, 1477, 1489, 1501, 1514, 1526,
	1539, 1551, 1564, 1576, 1589, 1601, 1614, 1626,
	1639, 1652, 1664, 1677, 1690, 1702, 1715, 1728,
	1740, 1753, 1766, 1779, 1791, 1804, 1817, 1830,
	1842, 1855, 1868, 1881, 1894, 1907, 1919, 1932,
	1945, 1958, 1971, 1984, 1997, 2009, 2022, 2035
	
};


// Generate a sine wave
void Generate_Sine_Wave(uint16_t amplitude, uint16_t frequency) {
	// Implement sine wave generation logic using the MCP4725_SetValue function
	uint16_t dacvalue;
	uint16_t accumulator = 0;
	uint16_t stepsize = 40;					//TODO: adjust frequency based on step-size value
	uint16_t lut_size = 1000;
	//sizeof(sine_lut) / sizeof(sine_lut[0]);
	
	
	
	
		while(accumulator < 1000){
		dacvalue = (uint16_t)  ( (pgm_read_word(&sine_lut[accumulator])) * amplitude )/ 100 ;

		MCP4725_SetValue(dacvalue);

		accumulator += stepsize;

		
		}
		//_delay_ms(100);
	
}

// Generate a square wave
void Generate_Square_Wave(uint16_t amplitude, uint16_t frequency) {
	// Implement square wave generation logic using the MCP4725_SetValue function
	
	uint16_t dac_value_high = (uint16_t)(40.95 * amplitude);  // High value for the square wave (maximum for a 12-bit DAC)
	
	
	MCP4725_SetValue(0);
	_delay_ms(1);
	MCP4725_SetValue(dac_value_high);
	_delay_ms(1);
	
	//TODO: FIX FREQUENCY CODE LATER
	
	/*switch(frequency){
		case 20:
		break;
		
		case 50:
		break;
		
		case 100:
		break;
		
		case 200:
		break;
		
		case 300:
		break;
		
		case 500:
		break;
		
		case 700:
		break;
		
		case 1000:
		break;
		
		case 1200:
		break;
		
		case 1500:
		break;
		
	}*/
}

// Generate a triangle wave
void Generate_Triangle_Wave(uint16_t amplitude, uint16_t frequency) {
	// Implement triangle wave generation logic using the MCP4725_SetValue function
	
	int16_t  dac_value = 0;  // Start at the minimum value
	uint16_t step_size = (uint16_t)50 * (amplitude/100);  // Step size for each increment/decrement 1500 steps 2.16kHZ
	uint8_t direction = 1;  // 1 means rising, 0 means falling

	while (direction !=2) {
		// Send the current DAC value to the MCP4725
		
		MCP4725_SetValue(dac_value);

		// Adjust DAC value based on the current direction (rising or falling)
		if (direction) {
			
			dac_value += step_size;  // Increment value while rising
			
			if (dac_value >=  (uint16_t)amplitude * 40.95 ) {  // If the peak is reached, start falling
				direction = 0;  // Switch to falling
				dac_value = (uint16_t)amplitude * 40.95;
			}
		}
		else {
			
			dac_value -= step_size;  // Decrement value while falling
			
			if (dac_value <= 0) {  // If the bottom is reached, start rising
				direction = 2;  //end function
				dac_value = 0;
				MCP4725_SetValue(dac_value);
			}
		}

		//_delay_ms(3);  // Adjust delay to control the frequency of the triangle wave
	}
}
