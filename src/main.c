#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "I2S/I2S.h"

int main() {
    //make a I2S instance for ease of use
    I2S* inst = malloc(sizeof(*inst));
    inst->BCLK = 5;
    inst->TX_PIN = 7;
    inst->pio = pio0;
    I2S_init(inst);
    audio_buffer = malloc(sizeof(*audio_buffer) * AUDIO_BUFFER_SIZE * 2); // for 2 dma channels
    buffer_a_flag = false;
    buffer_b_flag = false;

    // double func_step_size = get_dma_interrupt_interval(SAMPLE_RATE, 4, AUDIO_BUFFER_SIZE);
    uint64_t total_samples_generated = 0;
    double start_time = 0;
    //lets put in an alternating pattern for each channel
    // for(int i = 0; i < AUDIO_BUFFER_SIZE; i++){
    //     audio_buffer[i] = (0x00FFul << 16) | (0x00FFu << 0);
    // }
    // for(int i = AUDIO_BUFFER_SIZE; i < AUDIO_BUFFER_SIZE * 2; i++){
    //     audio_buffer[i] = (0xFF00ul << 16) | (0xFF00u << 0);
    // }
    gpio_set_dir(18, 1);
    gpio_set_function(18, GPIO_FUNC_SIO);
    gpio_put(18, 0);
    gpio_set_dir(14, 1);
    gpio_set_function(14, GPIO_FUNC_SIO);
    gpio_put(14, 0);
    init_dma_for_I2S(inst, audio_buffer);
    while (true) {
        //load in the new values into the audio buffer
        if(buffer_a_flag){
            // gpio_put(15, 1);
            buffer_a_flag = false;
            for(int i = 0; i < AUDIO_BUFFER_SIZE; i++){
                // double audio_val = waveform_calc((double)(total_samples_generated) / SAMPLE_RATE); 
                // int16_t sample = audio_val * INT16_MAX;
                // audio_buffer[i] = (((uint32_t)(sample)) << 16) | ((uint16_t)(sample));
                audio_buffer[i] = (0xFF00ul << 16) | (0xFF00u << 0);
                total_samples_generated++;
            }
            // gpio_put(15, 0);
        }
        if(buffer_b_flag){
            buffer_b_flag = false;
            for(int i = AUDIO_BUFFER_SIZE; i < (AUDIO_BUFFER_SIZE * 2); i++){
                // double audio_val = waveform_calc((double)(total_samples_generated) / SAMPLE_RATE);
                // int16_t sample = audio_val * INT16_MAX; 
                // audio_buffer[i] = (((uint32_t)(sample)) << 16) | ((uint16_t)(sample));
                audio_buffer[i] = (0xFF00ul << 16) | (0xFF00u << 0);
                total_samples_generated++;
            }
        }
    }
    free(inst);
    free((void*)(audio_buffer));
}

