#ifndef ADC_CONTROLLER_H
#define ADC_CONTROLLER_H

#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define WAVEGEN_BUTTON_PIN 21
#define FX_BUTTON_PIN 26

//wavegen finite state machine
typedef enum {
    SIN_WAVE,
    SAW_WAVE,
    SQUARE_WAVE
} wavegen_fsm;

wavegen_fsm q_wavegen;

// fx finite state machine
typedef enum {
    WAVETABLE,
    ECHO,
    DELAY    
} fx_fsm;

fx_fsm q_fx;

bool wavegen_button_flag;
bool fx_button_flag;

void initialize_pins(void);
void gpio_input_isr(void);
void init_gpio_irq(void);

#endif