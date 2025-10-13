#include "I2S.h"

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

void init_dma_for_I2S(I2S* inst, uint32_t* audio_buffer){

    dma_channel_config c = dma_channel_get_default_config(I2S_DMA_CHANNEL);
    pio_hw_t* pio_hw;

    if(inst->pio == pio0){
        pio_hw = pio0_hw; 
    }else if(inst->pio == pio1){
        pio_hw = pio1_hw;
    }else if(inst->pio == pio2){
        pio_hw = pio2_hw;
    }else{
        //something went wrong (there are only 3 pio instances)
        return;
    }

    dma_hw->ch[I2S_DMA_CHANNEL].read_addr = audio_buffer;
    dma_hw->ch[I2S_DMA_CHANNEL + 1].read_addr = audio_buffer + AUDIO_BUFFER_SIZE; // go to next half

    //dma (only one bus transfer)
    dma_hw->ch[I2S_DMA_CHANNEL].transfer_count = (0u << 28) + 1u;
    dma_hw->ch[I2S_DMA_CHANNEL + 1].transfer_count = (0u << 28) + 1u;

    uint32_t* write_addr = &pio_hw->txf[inst->sm];
    dma_hw->ch[I2S_DMA_CHANNEL].write_addr = write_addr;
    dma_hw->ch[I2S_DMA_CHANNEL + 1].write_addr = write_addr;

    dma_hw->ch[I2S_DMA_CHANNEL].ctrl_trig = 0;
    dma_hw->ch[I2S_DMA_CHANNEL + 1].ctrl_trig = 0;
    uint32_t temp = 0;
    temp |= 2u << 2; //set packet size to (one word)
    temp |= DREQ_PIO0_TX0 << 17; //set data request to PIO TX0
    temp |= 1 << 4; //increment read address after every transfer
    temp |= 3 << 4; //set the ring buffer size to 8 bytes (2**3)
    temp |= 1u << 0; // enable dma
    dma_hw->ch[I2S_DMA_CHANNEL + 1].ctrl_trig = temp;
    dma_hw->ch[I2S_DMA_CHANNEL].ctrl_trig = temp | (1 << 13); //chain_to channel I2S_DMA_CHANNEL + 1

    dma_irqn_set_channel_enabled(DMA_IRQ_0, I2S_DMA_CHANNEL, 1);
    dma_irqn_set_channel_enabled(DMA_IRQ_0, I2S_DMA_CHANNEL + 1, 1);

    irq_set_exclusive_handler(DMA_IRQ_0, dma_isr);
    irq_set_enabled(DMA_IRQ_0, 1);


}
void dma_isr(){
    if(dma_irqn_get_channel_status(DMA_IRQ_0, I2S_DMA_CHANNEL)){
        dma_irqn_acknowledge_channel(DMA_IRQ_0, I2S_DMA_CHANNEL);
    }else{
        dma_irqn_acknowledge_channel(DMA_IRQ_0, I2S_DMA_CHANNEL + 1);
    }
}

void write_audio_buffer(I2S* inst, uint32_t* audio_buffer, uint audio_buffer_len){
    for(int i = 0; i < audio_buffer_len; i++){
        pio_sm_put_blocking(inst->pio, inst->sm, audio_buffer[i]);
    }
}

