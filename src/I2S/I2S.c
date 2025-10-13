#include "I2S.h"

float get_clock_div_ratio(float sample_rate, float channels, float audio_bits, int instruction_count){
    return SYS_CLK_HZ / (sample_rate * audio_bits * channels * instruction_count);
}

uint I2S_init(){
    // 1. Define your I2S pins
    const uint BCLK_PIN = 5;
    // The LRCLK pin must be the next consecutive pin for this PIO program
    const uint LRCLK_PIN = BCLK_PIN + 1; // This will be GPIO 6
    const uint TX_PIN = 10;

    // 2. Choose a PIO instance (pio0 or pio1)
    PIO pio = pio0;

    // 3. Claim a free state machine on our chosen PIO instance
    uint sm = pio_claim_unused_sm(pio, true);

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

    return sm;

}

void write_audio_buffer(PIO i2s, uint sm, uint32_t* audio_buffer, uint audio_buffer_len){
    for(int i = 0; i < audio_buffer_len; i++){
        pio_sm_put_blocking(i2s, sm, audio_buffer[i]);
    }
}

