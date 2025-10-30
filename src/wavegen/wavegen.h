#ifndef WAVEGEN_H
#define WAVEGEN_H

#include "pico/stdlib.h"
#include <math.h>

#define ATTACK_CUTOFF 10
#define WAVETABLE_SIZE 1024

float sine_wavetable[WAVETABLE_SIZE];
float square_wavetable[WAVETABLE_SIZE];
float saw_wavetable[WAVETABLE_SIZE];

void init_wavetables();
float waveform_calc(float* wavegen_func, float phase, uint32_t samples, float alpha, float beta);

float attack_env(float x, float alpha, float beta);
float sin_wave(float x);
float square_wave(float x);
float saw_wave(float x);



#endif