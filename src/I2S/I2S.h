/*
    Configuration File for the I2S module
    Helper Functions for I2S peripheral
*/

#ifndef I2S_H
#define I2S_H

#include "hardware/platform_defs.h"

#define SAMPLE_RATE 44100 //standard sample rate in Hz for HIGH res sound
#define CHANNELS 2 // 2 channels for left/right
#define AUDIO_BITS 16 //length of DAC resoultion

// function that will get the clock divider ratio need for a specific sample rate
double get_clock_div_ratio(double sample_rate, double channels, double audio_bits, int instruction_count);


#endif 