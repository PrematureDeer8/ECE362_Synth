#include "adc_ctrlr.h"


// initialize gpio pins 24 and 23
void initialize_pins(void){
    uint gpio_mask = (1 << FX_BUTTON_PIN) | (1 << WAVEGEN_BUTTON_PIN);
    
    gpio_set_function(FX_BUTTON_PIN, GPIO_FUNC_SIO);
    gpio_set_function(WAVEGEN_BUTTON_PIN, GPIO_FUNC_SIO);
    
    gpio_set_dir(FX_BUTTON_PIN, false);
    gpio_set_dir(WAVEGEN_BUTTON_PIN, false);

}


void init_gpio_irq() {
    // fill in
    uint gpio_mask = (1 << FX_BUTTON_PIN) | (1 << WAVEGEN_BUTTON_PIN);

    wavegen_button_flag = false;
    fx_button_flag = false;

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
    printf("hello world\n");
    if (gpio_get_irq_event_mask(FX_BUTTON_PIN) == GPIO_IRQ_EDGE_RISE){
        gpio_acknowledge_irq(FX_BUTTON_PIN,GPIO_IRQ_EDGE_RISE); // acknowledges the interrupt request
        printf("fx button asserted! \n");
    } 
    if (gpio_get_irq_event_mask(WAVEGEN_BUTTON_PIN) == GPIO_IRQ_EDGE_RISE){
        gpio_acknowledge_irq(WAVEGEN_BUTTON_PIN, GPIO_IRQ_EDGE_RISE); // acknowledges the interrupt request
        printf("wavegen button asserted! \n");
    }
}