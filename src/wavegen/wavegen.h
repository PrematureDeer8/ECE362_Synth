#ifndef WAVEGEN_H
#define WAVEGEN_H

#include "pico/stdlib.h"
#include <math.h>

#define ATTACK_CUTOFF 10

float waveform_calc(float (*wavegen_func)(float), float phase, uint32_t samples, float alpha, float beta);

float attack_env(float x, float alpha, float beta);
float sin_wave(float x);
float square_wave(float x);
float saw_wave(float x);


#endif