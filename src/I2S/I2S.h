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
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>
#include "wavegen/wavegen.h"

#define SAMPLE_RATE 44100 // had to lower it to give cpu more breathing room );
#define CHANNELS 2 // 2 channels for left/right
#define AUDIO_BITS 16 //length of DAC resoultion
#define AUDIO_BUFFER_SIZE 512 
#define NUM_NOTES 5

int dma_chan, dma_chan2;

typedef struct _I2S {
    PIO pio;
    uint sm;
    uint BCLK;
    uint TX_PIN;
} I2S;

I2S* inst;

volatile uint32_t audio_buffer[AUDIO_BUFFER_SIZE * 2] __attribute__((aligned(AUDIO_BUFFER_SIZE * 2 * sizeof(uint32_t)))); // volatile is so that the compiler doesn't "optimize out"
uint32_t total_sample_count;
float phase_increment[NUM_NOTES];
float phase[NUM_NOTES];


I2S* init_wavegen(int BCLK, int TX_PIN, PIO chan, bool debug);

// function that will get the clock divider ratio need for a specific sample rate
float get_clock_div_ratio(float sample_rate, float channels, float audio_bits, int instruction_count);
void I2S_init(I2S* inst); // this function will set the statemachine in I2S inst
// void write_audio_buffer(I2S* inst, volatile int32_t* audio_buffer, uint audio_buffer_len);
void init_dma_for_I2S(I2S* inst, volatile uint32_t* audio_buffer);
void dma_isr_1();
void dma_isr_0();
void fill_audio_buffer(int start, int length);
//sample rate in HZ, the transfer FIFO length in bytes, number of bytes transferred by a dma channel
double get_dma_interrupt_interval(int sample_rate, int pio_tx_fifo_length, int dma_transfer_bytes);
void core1_entry();



#endif 