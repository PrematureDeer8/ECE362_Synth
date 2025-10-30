#include "I2S.h"

volatile bool toggle_a = 0;
volatile bool toggle_first_half = 1;
int increment = 0;

float get_clock_div_ratio(float sample_rate, float channels, float audio_bits, int instruction_count){
    return SYS_CLK_HZ / (sample_rate * audio_bits * channels * instruction_count);
}
//initalize audio buffer, phase accumulator, pio statemachine
I2S* init_wavegen(int BCLK, int TX_PIN, PIO chan, bool debug){
    phase_increment = 2 * M_PI * (float)(FUNC_FREQ) / SAMPLE_RATE;
    phase = 0;

    inst = malloc(sizeof(*inst));
    inst->BCLK = BCLK;
    inst->TX_PIN = TX_PIN;
    inst->pio = pio0;

    I2S_init(inst);

    total_sample_count = 0;

    //fill in audio buffer
    for(int i = 0; i < AUDIO_BUFFER_SIZE * 2; i++){
        float audio_val = waveform_calc(saw_wavetable, phase, total_sample_count, 0.5f, SAMPLE_RATE * 10); 
        int16_t sample = audio_val * INT16_MAX;
        audio_buffer[i] = ((uint32_t)(sample) << 16) | ((uint16_t)(sample));
        if(phase >= (2 * M_PI)){
            phase -= 2 * M_PI;
        }
        phase += phase_increment;
        total_sample_count++;
    }

    init_dma_for_I2S(inst, audio_buffer);
    //debug pins
    gpio_set_dir(18, 1);
    gpio_set_function(18, GPIO_FUNC_SIO);
    gpio_put(18, 0);
    gpio_set_dir(14, 1);
    gpio_set_function(14, GPIO_FUNC_SIO);
    gpio_put(14, 0);
    //return instance to be freed
    return inst;

}

void I2S_init(I2S* inst){
    // 1. Define your I2S pins
    const uint BCLK_PIN = inst->BCLK;
    // The LRCLK pin must be the next consecutive pin for this PIO program
    const uint LRCLK_PIN = BCLK_PIN + 1; // This will be GPIO 6
    const uint TX_PIN = inst->TX_PIN;

    // 2. Choose a PIO instance (pio0 or pio1)
    PIO pio = inst->pio;

    // 3. Claim a free state machine on our chosen PIO instance
    uint sm = pio_claim_unused_sm(pio, true);
    inst->sm = sm;

    // 4. Load the PIO program into the PIO's instruction memory
    uint offset = pio_add_program(pio, &i2s_clocks_program);

    // 5. Configure the state machine
    pio_sm_config c = i2s_clocks_program_get_default_config(offset);
    
    sm_config_set_out_pins(&c, TX_PIN, 1);
    // --- PIN CONFIGURATION ---
    // Map the .side_set pins to our chosen GPIOs, starting with BCLK_PIN.
    // The program uses 2 side-set pins, so this will configure GPIO 5 and 6.
    sm_config_set_sideset_pins(&c, BCLK_PIN);
    //shift right (false), autopull = true (autopull will stall the statemachine until FIFO is not empty)
    sm_config_set_out_shift(&c, false, true, 32); // 16 bit for each channel = 32 bits total
    //chain the TX and RX FIFO together in order to store more samples
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);


    //set direction of pins to output
    uint64_t pin_mask = (1ull << TX_PIN) | (3ull << BCLK_PIN);
    pio_sm_set_pindirs_with_mask(pio, sm, pin_mask, pin_mask);
    // Initialize the GPIO pins for PIO control
    pio_gpio_init(pio, TX_PIN);
    pio_gpio_init(pio, BCLK_PIN);
    pio_gpio_init(pio, LRCLK_PIN);

    // 6. Apply the configuration and start the state machine
    pio_sm_init(pio, sm, offset, &c);

    //configure clock divider
    //this goes after initializing the pio statemachine
    pio_sm_set_clkdiv(pio, sm, get_clock_div_ratio(SAMPLE_RATE, CHANNELS, AUDIO_BITS, 2));

    pio_sm_set_enabled(pio, sm, true);

}

void init_dma_for_I2S(I2S* inst, volatile uint32_t* audio_buffer){
    dma_chan = dma_claim_unused_channel(true);
    // dma_chan = 0;
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    // We need to move SRC address but not DST
    channel_config_set_write_increment(&c, false);
    channel_config_set_read_increment(&c, true);
    uint bytes_size = (uint)(log2f(AUDIO_BUFFER_SIZE * sizeof(*audio_buffer) * 2));
    channel_config_set_ring(&c, false, bytes_size);
    
    // Set DMA timer 0 as DMA trigger
    channel_config_set_dreq(&c, pio_get_dreq(inst->pio, inst->sm, true));
    // Set DMA timer 0 as DMA trigger

    dma_channel_configure
	(
        dma_chan,
        &c,
        &inst->pio->txf[inst->sm],      // Destination is PIO FIFO
        audio_buffer,                 // Source is a memory array
        (0x1ul << 28) | AUDIO_BUFFER_SIZE,                     // Length of the transaction
        true                    // start immediately
    );

    dma_irqn_set_channel_enabled(0, dma_chan, 1);

    // Set our ISR as the exclusive handler for DMA_IRQ_0
    irq_set_priority(DMA_IRQ_0, 0);//highest priority(we can't wait around)
    irq_set_exclusive_handler(DMA_IRQ_0, &dma_isr_0);
    irq_set_enabled(DMA_IRQ_0, true);
    

}
//as of now this is to slow
void dma_isr_0(){

    //signal to the cpu to calculate the next audio data 0 - 255
    // dma_irqn_acknowledge_channel(DMA_IRQ_0, dma_chan);
    
    
    //this line below will acknowledge the interrupt!
    dma_hw->intr = 1u << dma_chan; // this is the line we have been searching for
    
    gpio_put(14, 1);
    // calculate next samples for first half of audio buffer
    if(toggle_first_half){
        for(int i = 0; i < AUDIO_BUFFER_SIZE; i++){
            float audio_val = waveform_calc(saw_wavetable, phase, total_sample_count, 0.5f, SAMPLE_RATE * 10); 
            int16_t sample = audio_val * INT16_MAX;
            audio_buffer[i] = ((uint32_t)(sample) << 16) | ((uint16_t)(sample));
            //make sure phase stays at a reasonable level
            if(phase >= (2 * M_PI)){
                phase -= 2 * M_PI;
            }
            phase += phase_increment;
            total_sample_count++;
        }
        toggle_first_half = 0;
    //calculate next samples for second half of audio buffer
    }else{
        for(int i = AUDIO_BUFFER_SIZE; i < (AUDIO_BUFFER_SIZE * 2); i++){
            float audio_val = waveform_calc(saw_wavetable, phase, total_sample_count, 0.5f, SAMPLE_RATE * 10);  
            int16_t sample = audio_val * INT16_MAX;
            audio_buffer[i] = ((uint32_t)(sample) << 16) | ((uint16_t)(sample));
            //make sure phase stays at a reasonable level (does not increment forever)
            if(phase >= (2 * M_PI)){
                phase -= 2 * M_PI;
            }
            phase += phase_increment;
            total_sample_count++;
        }
        toggle_first_half = 1;
    }
    gpio_put(14, 0);

}

double get_dma_interrupt_interval(int sample_rate, int pio_tx_fifo_length, int dma_transfer_bytes){
    return (double)(dma_transfer_bytes) / ((double)(pio_tx_fifo_length) * (double)(sample_rate));
}
//sine wave

/*float waveform_calc(float x){
    return sinf(x);
}*/
//square wave
/*float waveform_calc(float x){
    float k_prime = (int)(x / (2 * M_PI)); //take the floor of this value
    if((x > (k_prime) * 2 * M_PI) && (x < ((k_prime + 0.5) * 2 * M_PI))){
        return 1;
    }else{
        return -1;
    }
}*/
//saw wave
/*float waveform_calc(float x){
    float k_prime = (x / M_PI);
    int f_k_prime = (2 * k_prime + 2) / 4.0f;
    return (x/M_PI - (f_k_prime * 2));
}*/

/*
void write_audio_buffer(I2S* inst, volatile uint32_t* audio_buffer, uint audio_buffer_len){
    for(int i = 0; i < audio_buffer_len; i++){
        pio_sm_put_blocking(inst->pio, inst->sm, audio_buffer[i]);
    }
}
    */

