#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "I2S/I2S.h"

int main() {
    stdio_init_all();
    //make a I2S instance for ease of use
    I2S* inst = malloc(sizeof(*inst));
    total_sample_count = 0;
    phase_increment = 2 * M_PI * (float)(FUNC_FREQ) / SAMPLE_RATE;
    phase = 0;
    inst->BCLK = 5;
    inst->TX_PIN = 7;
    inst->pio = pio0;
    I2S_init(inst);
    // audio_buffer = malloc(sizeof(*audio_buffer) * AUDIO_BUFFER_SIZE * 2); // for 2 dma channels
    buffer_a_flag = false;
    buffer_b_flag = false;

    // double func_step_size = get_dma_interrupt_interval(SAMPLE_RATE, 4, AUDIO_BUFFER_SIZE);
    //lets put in an alternating pattern for each channel
    for(int i = 0; i < AUDIO_BUFFER_SIZE * 2; i++){
        double audio_val = waveform_calc(phase_increment); 
        int16_t sample = audio_val * INT16_MAX;
        audio_buffer[i] = ((uint32_t)(sample) << 16) | ((uint16_t)(sample));
        if(phase >= (2 * M_PI)){
            phase -= 2 * M_PI;
        }
        phase += phase_increment;
    }
    gpio_set_dir(18, 1);
    gpio_set_function(18, GPIO_FUNC_SIO);
    gpio_put(18, 0);
    gpio_set_dir(14, 1);
    gpio_set_function(14, GPIO_FUNC_SIO);
    gpio_put(14, 0);
    sleep_ms(3000);
    uint32_t start_address = (uint32_t)(&audio_buffer[0]);
    uint32_t end_address = (uint32_t)(&audio_buffer[(2 * AUDIO_BUFFER_SIZE)- 1]);
    printf("Read start address: %p\n", (void *)(audio_buffer));
    printf("Read end address: %p\n", (void *)(&audio_buffer[AUDIO_BUFFER_SIZE - 1]));
    init_dma_for_I2S(inst, audio_buffer);
    absolute_time_t start_time, end_time;
    uint32_t transfer_count, last_transfer_count;
    last_transfer_count = 0;
    end_time = 0;
    uint32_t read_addr, last_read_addr;
    while (true) {
        //load in the new values into the audio buffer
        transfer_count = (uint32_t)(dma_hw->ch[dma_chan].transfer_count);
        uint32_t mask = transfer_count & ~(1ul << 28);
        // printf("Read address: %p\n", (void*)(read_addr));
        // printf("transfer count: %lu\n", (unsigned long)(mask));
        if(transfer_count > last_transfer_count){
            start_time = get_absolute_time();
            uint32_t s1 = to_ms_since_boot(start_time);
            uint32_t s2 = to_ms_since_boot(end_time);
            printf("Transfer time: %lu\n", (s1 - s2));
            end_time = start_time;
            // printf("current read address: %p\n", (void*)(read_addr));
            // printf("last Read address: %p\n", (void*)(last_read_addr));
            // printf("transfer count: %lu\n", (last_transfer_count & ~(1ul << 28)));
            // printf("\n");
        }
        // last_read_addr = read_addr;
        last_transfer_count = transfer_count;

        read_addr = (uint32_t)(dma_hw->ch[dma_chan].read_addr);
        if((read_addr < start_address) || (read_addr > end_address)){
            // printf("Incorrect read address encountered: %lu\n", (unsigned long)(read_addr));
            printf("%lu, ", (unsigned long)(read_addr));
        }
    }
    free(inst);
}

