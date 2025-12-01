#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "I2S/I2S.h"
#include "adc_ctrlr/adc_ctrlr.h"
#include "midi_and_uart/uart_midi.h"

int main() {
    stdio_init_all();
    //adc controller stuff
    initialize_pins();
    init_gpio_irq();
    init_wavetables();
    //make a I2S instance for ease of use
    init_wavegen(5, 7, pio0, true);
    init_uart();
    attach_uart_irqs();
    // key_press(0 + 12);
    // key_press(4 + 12);
    // key_press(7 + 12);
    while (true) {
        // char c = uart_getc(uart1);
        // printf("%c\n", c);
        // if(uart_trig){
        //     int data = uart_get_hw(uart1)->dr & 0xFF;
        //     printf("%d\n", data);
        //     uart_trig = false;
        // }
    }
    free(inst);
}

