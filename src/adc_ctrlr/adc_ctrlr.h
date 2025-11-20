#ifndef ADC_CONTROLLER_H
#define ADC_CONTROLLER_H

#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define WAVEGEN_BUTTON_PIN 21
#define FX_BUTTON_PIN 26
#define POT1_PIN 41 //amplitude
//make sure right leg goes to V and left goes to GND
#define POT2_PIN 42 //bitcrush

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
void init_pots(void);
void update_pots(void);

#endif