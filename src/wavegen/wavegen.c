#include "wavegen.h"
extern uint bitcrush_res;
extern float amplitude;


static float delta_theta = 2.0f * M_PI / (float)(WAVETABLE_SIZE);

//wavegen
float waveform_calc(int wave_sel, int corenum){
    if(key_status.length == 0){
        return 0;
    }
    //turn phase into a discrete sample point
    float waveform = 0;
    float* wavetable;
    for(int j = 0; j < key_status.length; j++){
        int i = key_status.index_status[j];
        //added phase update loop inside waveform calc for faster CPU time
        if (phase[corenum][i] >= (2 * M_PI)){
            phase[corenum][i] -= 2 * M_PI;
        }
        phase[corenum][i] += phase_increment[i];
        // total_sample_count++;
        uint index = ((uint)(phase[corenum][i] / delta_theta) % WAVETABLE_SIZE);
        if(wave_sel == 0){
            wavetable = sine_wavetable;
        }else if(wave_sel == 1){
            wavetable = saw_wavetable;
        }else{
            wavetable = square_wavetable;
        }
        waveform += wavetable[index];
        // waveform += sinf(phase[i]);
    }
    return amplitude * waveform / (float)(key_status.length);
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
//handle when a certain note key was pressed
void key_press(int index){
    if(key_status.index_tracker[index] == -1){
        key_status.index_status[key_status.length] = index;
        key_status.index_tracker[index] = key_status.length;
        key_status.length++;
    }else{
        printf("Index key: %d is already pressed!\n", index);
    }
}

void key_release(int index){
    if(key_status.index_tracker[index] == -1){
        printf("Index key: %d is already considered released!\n", index);
    }else{
        //put last valid index status element in the removed index
        key_status.index_status[key_status.index_tracker[index]] = key_status.index_status[key_status.length - 1];
        key_status.index_tracker[key_status.index_status[key_status.length - 1]] = key_status.index_tracker[index];
        key_status.index_tracker[index] = -1;
        key_status.length--;
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

// FXs

int16_t bitcrush(int16_t sample, int target_bits)
{
    if(target_bits >= 16)
    {
        return sample; //nothing to do here
    }
    if(target_bits < 1)
    {
        return 0; //no samples
    }
    int32_t step = 1 << (16 - target_bits); //ensures we keep the top target_bits
   // lets say our target bits is 4, then step will shift to bit position 12, and only hold the top 4 bits
    int32_t rounded;
    if(sample >= 0) //this gets the rounded number near the step value
    {
       rounded = sample + step/2;
    }
    else{
       rounded = sample - step/2;
    }
    int32_t crushed = (rounded / step) * step; //when rounded is dvided by step it should truncate to a whole number.
    //then when a whole number is multipled by step, it will ensure that the bit crushed value is in multiples of step

    if(crushed > INT16_MAX)
    {
        crushed = INT16_MAX;
    }
    if(crushed < -INT16_MAX)
    {
        crushed = -INT16_MAX;
    }

    return (int16_t)crushed;
}
