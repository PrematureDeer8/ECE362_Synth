#include "wavegen.h"

static float delta_theta = 2.0f * M_PI / WAVETABLE_SIZE;

//wavegen
float waveform_calc(float* wavetable, float phase, uint32_t samples, float alpha, float beta){
    //turn phase into a discrete sample point
    uint i = (uint)(phase / delta_theta);
    return wavetable[i];
    // return attack_env(samples, alpha, beta) * (*wavegen_func)(phase);
}

void init_wavetables(){
    float theta;
    for(int i = 0; i < WAVETABLE_SIZE; i++){
        theta = (float)(i) * 2.0f * M_PI / (WAVETABLE_SIZE);
        sine_wavetable[i] = sinf(theta);
        square_wavetable[i] = square_wave(theta);
        saw_wavetable[i] = saw_wave(theta);
    }
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