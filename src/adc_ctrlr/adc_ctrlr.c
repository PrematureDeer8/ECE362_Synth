#include "adc_ctrlr.h"

uint bitcrush_res = 16;
float amplitude = 1.0f;

// initialize gpio pins 24 and 23
void initialize_pins(void){
    gpio_set_dir(FX_BUTTON_PIN, false);
    gpio_set_dir(WAVEGEN_BUTTON_PIN, false);
    
    gpio_put(FX_BUTTON_PIN, 0);
    gpio_put(FX_BUTTON_PIN, 0);

    gpio_set_function(FX_BUTTON_PIN, GPIO_FUNC_SIO);
    gpio_set_function(WAVEGEN_BUTTON_PIN, GPIO_FUNC_SIO);

}



void init_gpio_irq() {
    // fill in
    uint gpio_mask = (1 << FX_BUTTON_PIN) | (1 << WAVEGEN_BUTTON_PIN);

    wavegen_button_flag = false;
    fx_button_flag = false;

    // initialize the wavegen fsm to sin_wave
    q_wavegen = SIN_WAVE;

    // assigns a handler for pins 23 and 24
    gpio_add_raw_irq_handler_masked((gpio_mask), gpio_input_isr);

    // allows GPIOs from bank 0 to send IRQs
    irq_set_enabled(IO_IRQ_BANK0, true);  

    // enable the GPIO IRQ for both pins such that handler is called on rising edge
    gpio_set_irq_enabled(FX_BUTTON_PIN, GPIO_IRQ_EDGE_RISE, 1);
    gpio_set_irq_enabled(WAVEGEN_BUTTON_PIN, GPIO_IRQ_EDGE_RISE, 1);
}

void gpio_input_isr(void){

    // triggered the interrupt,
    if (gpio_get_irq_event_mask(FX_BUTTON_PIN) == GPIO_IRQ_EDGE_RISE){
        gpio_acknowledge_irq(FX_BUTTON_PIN,GPIO_IRQ_EDGE_RISE); // acknowledges the interrupt request
        fx_button_flag = true;
    } 
    if (gpio_get_irq_event_mask(WAVEGEN_BUTTON_PIN) == GPIO_IRQ_EDGE_RISE){
        
        gpio_acknowledge_irq(WAVEGEN_BUTTON_PIN, GPIO_IRQ_EDGE_RISE); // acknowledges the interrupt request
        wavegen_button_flag = true; // sets the wavegen_flag

        // changes the wavegen state when pressed and resets the state to SIN_WAVE when pressed and q_wavegen is currently SQUARE_WAVE
        q_wavegen = (q_wavegen + 1) % 3;
    }
}
//POTS
void init_pots(void)
{
    adc_init();
    adc_gpio_init(POT1_PIN);
    adc_gpio_init(POT2_PIN);
}
void update_pots(void)
{
    //amplitude
    adc_select_input(1); //channel 1 = GPIO27
    uint16_t raw_amp = adc_read();
    
    float x = (float)raw_amp / 4095.0f;
    amplitude = x * x; //helps smooth curve for better low-volume control

    //bitcrush
    adc_select_input(2); //channel 2 = GPIO28
    uint16_t raw_bit = adc_read(); //0-4096

    bitcrush_res = 1 + (raw_bit * 16) / 4095; //maps 1-16 bits
    //printf("raw1=%u amp=%.4f  raw2=%u bits=%u\n", raw_amp, amplitude, raw_bit, bitcrush_res);

}