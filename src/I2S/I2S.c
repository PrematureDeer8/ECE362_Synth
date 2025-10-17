#include "I2S.h"

volatile bool toggle_a = 0;
volatile bool toggle_b = 0;
int increment = 0;

float get_clock_div_ratio(float sample_rate, float channels, float audio_bits, int instruction_count){
    return SYS_CLK_HZ / (sample_rate * audio_bits * channels * instruction_count);
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

/*void init_dma_for_I2S(I2S* inst, volatile uint32_t* audio_buffer){

    dma_channel_config cfg_a = dma_channel_get_default_config(I2S_DMA_CHANNEL_A);
    channel_config_set_transfer_data_size(&cfg_a, DMA_SIZE_32);
    channel_config_set_read_increment(&cfg_a, true);
    channel_config_set_write_increment(&cfg_a, false);
    channel_config_set_dreq(&cfg_a, pio_get_dreq(inst->pio, inst->sm, true));
    // When Channel A finishes, it will trigger Channel B
    // channel_config_set_chain_to(&cfg_a, I2S_DMA_CHANNEL_B);

    dma_channel_configure(
        I2S_DMA_CHANNEL_A,
        &cfg_a,
        &inst->pio->txf[inst->sm], // Write address (PIO TX FIFO)
        audio_buffer,                      // Read address (start of buffer)
        AUDIO_BUFFER_SIZE,              // Transfer the first half
        false                              // Don't start yet
    );

    // --- Configure Channel B (for the second half of the buffer) ---
    dma_channel_config cfg_b = dma_channel_get_default_config(I2S_DMA_CHANNEL_B);
    channel_config_set_transfer_data_size(&cfg_b, DMA_SIZE_32);
    channel_config_set_read_increment(&cfg_b, true);
    channel_config_set_write_increment(&cfg_b, false);
    channel_config_set_dreq(&cfg_b, pio_get_dreq(inst->pio, inst->sm, true));
    // When Channel B finishes, it will trigger Channel A to start over
    // channel_config_set_chain_to(&cfg_b, I2S_DMA_CHANNEL_A);

    dma_channel_configure(
        I2S_DMA_CHANNEL_B,
        &cfg_b,
        &inst->pio->txf[inst->sm], // Write address (PIO TX FIFO)
        &audio_buffer[AUDIO_BUFFER_SIZE], // Read address (second half)
        AUDIO_BUFFER_SIZE,                // Transfer the second half
        false                                 // Don't start yet
    );

    // --- Configure Interrupts ---
    // We want an interrupt when each channel completes.
    // Both channels will trigger the same IRQ line (DMA_IRQ_0).
    dma_channel_set_irq0_enabled(I2S_DMA_CHANNEL_A, true);
    dma_channel_set_irq1_enabled(I2S_DMA_CHANNEL_B, true);

    // Set our ISR as the exclusive handler for DMA_IRQ_0
    irq_set_exclusive_handler(DMA_IRQ_0, &dma_isr_0);
    irq_set_enabled(DMA_IRQ_0, true);

    dma_channel_set_irq1_enabled(I2S_DMA_CHANNEL_B, true);
    irq_set_exclusive_handler(DMA_IRQ_1, &dma_isr_1);
    irq_set_enabled(DMA_IRQ_1, true);

    // --- Start the DMA loop ---
    // Manually trigger the first channel to start the chain reaction.
    // dma_channel_start(I2S_DMA_CHANNEL_A);
    dma_channel_start(I2S_DMA_CHANNEL_B);


}*/

void init_dma_for_I2S(I2S* inst, volatile uint32_t* audio_buffer){
    // dma_chan = dma_claim_unused_channel(true);
    dma_chan = 0;
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    // We need to move SRC address but not DST
    channel_config_set_write_increment(&c, false);
    channel_config_set_read_increment(&c, true);
    uint bytes_size = (uint)(log2f(AUDIO_BUFFER_SIZE * sizeof(*audio_buffer)));
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
    irq_set_exclusive_handler(DMA_IRQ_0, &dma_isr_0);
    irq_set_enabled(DMA_IRQ_0, true);
    

}

void dma_isr_0(){

    //signal to the cpu to calculate the next audio data 0 - 255
    // dma_irqn_acknowledge_channel(DMA_IRQ_0, dma_chan);
    dma_hw->intr = 1u << dma_chan;
    buffer_a_flag = true;
    gpio_put(14, toggle_a);
    toggle_a = !toggle_a;
    uint32_t* next_trans_count = (DMA_BASE + 0x804);
    printf("Trans count reload value: %lu!\n", (unsigned long)(*next_trans_count));
    // dma_irqn_set_channel_enabled(0, dma_chan, 1);

}

double get_dma_interrupt_interval(int sample_rate, int pio_tx_fifo_length, int dma_transfer_bytes){
    return (double)(dma_transfer_bytes) / ((double)(pio_tx_fifo_length) * (double)(sample_rate));
}

double waveform_calc(double x){
    return sin(2 * M_PI * FUNC_FREQ * x);
}
/*
void write_audio_buffer(I2S* inst, volatile uint32_t* audio_buffer, uint audio_buffer_len){
    for(int i = 0; i < audio_buffer_len; i++){
        pio_sm_put_blocking(inst->pio, inst->sm, audio_buffer[i]);
    }
}
    */

