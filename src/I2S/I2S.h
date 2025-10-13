/*
    Configuration File for the I2S module
    Helper Functions for I2S peripheral
*/

#ifndef I2S_H
#define I2S_H

#include "hardware/platform_defs.h"
#include "I2S/i2s_clocks.pio.h"
#include "hardware/pio.h"

#define SAMPLE_RATE 44100 //standard sample rate in Hz for HIGH res sound
#define CHANNELS 2 // 2 channels for left/right
#define AUDIO_BITS 16 //length of DAC resoultion
#define AUDIO_BUFFER_SIZE 256

typedef struct _I2S {
    PIO pio;
    uint sm;
} I2S;

// function that will get the clock divider ratio need for a specific sample rate
float get_clock_div_ratio(float sample_rate, float channels, float audio_bits, int instruction_count);
uint I2S_init();
void write_audio_buffer(PIO pio, uint sm, uint32_t* audio_buffer, uint audio_buffer_len);


#endif 