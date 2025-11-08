#ifndef ADC_CONTROLLER_H
#define ADC_CONTROLLER_H

#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define WAVEGEN_BUTTON_PIN 15U
#define FX_BUTTON_PIN 16U

bool wavegen_button_flag;
bool fx_button_flag;

void initialize_pins(void);
void gpio_input_isr(void);
void gpio_init_isr(void);

#endif