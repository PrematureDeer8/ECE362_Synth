#include "I2S.h"

double get_clock_div_ratio(double sample_rate, double channels, double audio_bits, int instruction_count){
    return SYS_CLK_HZ / (sample_rate * audio_bits * channels * instruction_count);
}

