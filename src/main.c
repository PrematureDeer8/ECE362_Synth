#include <pico/stdlib.h>
#include <stdio.h>
#include "hardware/pio.h"
#include "I2S/I2S.pio.h"
#include "I2S/I2S.h"


int main(void){

    //set BCLK to pin 4
    const uint BCLK = 4;

    PIO I2S_pio = pio0;

    //load I2S program instructions into a PIO instance
    for(uint i = 0; i < count_of(I2S_program_instructions); i++){
        I2S_pio->instr_mem[i] = I2S_program_instructions[i];
    }
    printf("Instruction count: %d\n", count_of(I2S_program_instructions));

    //calculate clock division
    double cdr = get_clock_div_ratio(SAMPLE_RATE, CHANNELS, AUDIO_BITS, count_of(I2S_program_instructions));

    //set state machine 1's clock divider to the clock division ratio (cdr)
    I2S_pio->sm[0].clkdiv = (uint32_t) (cdr * (1 << 16));
    
    //we can choose to set 5 pins in the state machine
    I2S_pio->sm[0].pinctrl = 
        (1 << PIO_SM0_PINCTRL_SET_COUNT_LSB) |
        (BCLK << PIO_SM0_PINCTRL_SET_BASE_LSB) ;

    return 0;
}

