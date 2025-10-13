#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "I2S/I2S.h"


int main() {
    
    uint sm = I2S_init();
    uint32_t* audio_buffer = malloc(sizeof(*audio_buffer) * AUDIO_BUFFER_SIZE);
    //lets put in an alternating pattern of 11110000 for each channel
    for(int i = 0; i < AUDIO_BUFFER_SIZE; i++){
        audio_buffer[i] = (0xF0F0ul << 16) | (0xFF00u << 0);
    }
    while (true) {
        write_audio_buffer(pio0, sm, audio_buffer, AUDIO_BUFFER_SIZE);
    }
    free(audio_buffer);
}

