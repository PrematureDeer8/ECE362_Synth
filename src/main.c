#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "I2S/I2S.h"
#include "adc_ctrlr/adc_ctrlr.h"
#include "SPI/spi_LCD.h"
#include "midi_and_uart/uart_midi.h"

int main()
{
    stdio_init_all();
    // adc controller stuff
    initialize_pins();
    init_gpio_irq();
    init_pots();
    init_wavetables();
    init_chardisp_pins();
    cd_init();
    // make a I2S instance for ease of use
    init_wavegen(5, 7, pio0, true);
    uint32_t index = 0;
    bool toggle_key = 1;
    init_uart();
    attach_uart_irqs();
    while (true) {
        
        //dami block (adc controller)
        if(fx_button_flag){
            /*if(index >= NUM_NOTES){
                index = 0;
            }*/
            if(index == NUM_NOTES){
                toggle_key = !toggle_key;
            }
            index = index % NUM_NOTES;
            // printf("Fx button was triggered!\n");
            
            //index is not currently in the index status
            // if(toggle_key){
            //     key_press(index);
            // }else{
            //     key_release(index);
            // }
            // //keynote_status[index] = !keynote_status[index];
            // index++;
            // printf("Number of notes played: %d\n", index);
            /*if(index == 0 || index % 2){
                index += 4;
            }else{
                index += 3;
            }*/
            fx_button_flag = false;
        }
        if(wavegen_button_flag){
            printf("Wavegen button was pressed!\n");
            wavegen_button_flag = false;
        }
         // pots
        update_pots();
        update_lcd();
        sleep_ms(50);
    }
    free(inst);
}
