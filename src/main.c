#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "I2S/I2S.h"


int main() {
    //make a I2S instance for ease of use
    I2S* inst = malloc(sizeof(*inst));
    inst->BCLK = 5;
    inst->TX_PIN = 10;
    inst->pio = pio0;
    I2S_init(inst);
    uint32_t* audio_buffer = malloc(sizeof(*audio_buffer) * AUDIO_BUFFER_SIZE * 2); // for 2 dma channels
    //lets put in an alternating pattern for each channel
    for(int i = 0; i < AUDIO_BUFFER_SIZE * 2; i++){
        audio_buffer[i] = (0xF0F0ul << 16) | (0xFF00u << 0);
    }
    init_dma_for_I2S(inst, audio_buffer);
    // while (true) {
    //     write_audio_buffer(inst, audio_buffer, AUDIO_BUFFER_SIZE);
    // }
    free(inst);
    free(audio_buffer);
}

