/*
    Configuration File for the I2S module
    Helper Functions for I2S peripheral
*/

#ifndef I2S_H
#define I2S_H

#include "hardware/platform_defs.h"
#include "I2S/i2s_clocks.pio.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include <stdio.h>

#define SAMPLE_RATE 44100 //standard sample rate in Hz for HIGH res sound
#define CHANNELS 2 // 2 channels for left/right
#define AUDIO_BITS 16 //length of DAC resoultion
#define AUDIO_BUFFER_SIZE 256 // length OSR FIFO when RX join with TX
#define I2S_DMA_CHANNEL 0 //this is the DMA channel for I2S (DO NOT USE IT FOR ANOTHER SUBMODULE)

typedef struct _I2S {
    PIO pio;
    uint sm;
    uint BCLK;
    uint TX_PIN;
} I2S;

volatile bool dma_interrupt_fired;

// function that will get the clock divider ratio need for a specific sample rate
float get_clock_div_ratio(float sample_rate, float channels, float audio_bits, int instruction_count);
void I2S_init(I2S* inst); // this function will set the statemachine in I2S inst
void write_audio_buffer(I2S* inst, volatile uint32_t* audio_buffer, uint audio_buffer_len);
void init_dma_for_I2S(I2S* inst, volatile uint32_t* audio_buffer);
void dma_isr();


#endif 