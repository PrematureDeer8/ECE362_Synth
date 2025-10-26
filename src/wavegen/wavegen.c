#include "wavegen.h"

//wavegen
//this some nasty C lol
float waveform_calc(float (*wavegen_func)(float), float phase, uint32_t samples, float alpha, float beta){
    return attack_env(samples, alpha, beta) * (*wavegen_func)(phase);
}

//sine wave
float sin_wave(float x){
    return sinf(x);
}
//square wave
float square_wave(float x){
    float k_prime = (int)(x / (2 * M_PI)); //take the floor of this value
    if((x > (k_prime) * 2 * M_PI) && (x < ((k_prime + 0.5) * 2 * M_PI))){
        return 1;
    }else{
        return -1;
    }
}
float saw_wave(float x){
    float k_prime = (x / M_PI);
    int f_k_prime = (2 * k_prime + 2) / 4.0f;
    return (x/M_PI - (f_k_prime * 2));
}

//envelopes

float attack_env(float x, float alpha, float beta){
    //limit the envelope between [0, 1]
    if(x > beta){
        return 1.0f;
    }
    return powf(x / beta, alpha);
}