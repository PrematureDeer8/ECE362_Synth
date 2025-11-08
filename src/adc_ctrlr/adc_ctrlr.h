#ifndef ADC_CONTROLLER_H
#define ADC_CONTROLLER_H

#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define WAVEGEN_BUTTON_PIN 23
#define FX_BUTTON_PIN 24

//wavegen finite state machine
typedef enum {
    SIN_WAVE,
    SAW_WAVE,
    SQUARE_WAVE
} wavegen_fsm;

wavegen_fsm q_wavegen;

bool wavegen_button_flag;
bool fx_button_flag;

void initialize_pins(void);
void gpio_input_isr(void);
void init_gpio_irq(void);

#endif