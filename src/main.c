#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "I2S/I2S.h"

int main() {
    stdio_init_all();
    //make a I2S instance for ease of use
    init_wavegen(5, 7, pio0, true);  //func in I2S, creates wave
    uint32_t start_address = (uint32_t)(&audio_buffer[0]);
    uint32_t end_address = (uint32_t)(&audio_buffer[(2 * AUDIO_BUFFER_SIZE)- 1]);
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
            // printf("Transfer time: %lu\n", (s1 - s2));
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

