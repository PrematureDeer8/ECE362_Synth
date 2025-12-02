#include "uart_midi.h"
#include "I2S/I2S.h"

#define BAUD_RATE 31250

/* MIDI pinout:
         , - ~ ~ ~ - ,
     , '               ' ,
   ,                       ,
  ,                         ,
 ,                           ,
 ,       3             1     ,
 ,       /  5        4 \     ,
  ,     /   /   2    \  \   ,
   ,       /    |     \    ,
     ,          |       , '
       ' - , _ _ _ ,  '
*/

#define RP2350_PIN_MIDI_IN 37 // TODO: Define pin correctly

/* 3 bytes in a standard midi frame:

    Byte 0:
    
    [7]  : always 1, indicates status bit
    [6:4]: type bits;
        0x0: note on
        0x1: note off
        0x2: aftertouch
        0x3: continuous controller
        0x4: patch change
        0x5: channel pressure
        0x6: pitch bend
        0x7: non-music commands
    [3:0]: which of the 15 midi channels to broadcast on

    (From ccrma.stanford.edu/~craig/articles/linuxmidi/misc/essenmidi.html)

    Byte 0  Meaning         # Parameters 	Byte 1          byte 2
    0x80 	Note-off                2 	    key 	        velocity
    0x90 	Note-on 	            2 	    key 	        veolcity
    0xA0 	Aftertouch 	            2 	    key 	        touch
    0xB0 	Continuous controller 	2 	    controller # 	controller value
    0xC0 	Patch change 	        2 	    instrument # 	
    0xD0 	Channel Pressure 	    1 	    pressure
    0xE0 	Pitch bend 	            2 	    lsb (7 bits) 	msb (7 bits)
    0xF0 	(non-musical commands) 	X       X               X
*/ 

#define MIDI_NOTE_ON  (1 << 4)
#define MIDI_BYTE_MAX 3
#define MIDI_MAX_INDEX 2
#define MIDI_TO_ENUM_OFFSET 60 // TODO: figure out default keyboard C enum (C4 = 0d60)

/*
       0     1     2
    |-----|-----|-----|
    f              b
*/
int midi_rx_buffer[MIDI_BYTE_MAX];
// int midi_buffer_front = -1;
int midi_buffer_len = 0;
  

// Safely stores incoming MIDI data for processing. It discards messages if
// the queue is already full (this should not be noticeable to the user unless
// there's a timing hiccup.)

// Sets UART GPIO PIN to UART function, initializes UART0
void init_uart() 
{
    gpio_set_function(RP2350_PIN_MIDI_IN, UART_FUNCSEL_NUM(uart1, RP2350_PIN_MIDI_IN));
    uart_init(uart1, BAUD_RATE);
    
}

void attach_uart_irqs() 
{
    uart_set_irq_enables(uart1, 1, 0);
    // uart_set_fifo_enabled(uart0, false); // TODO: check if we need the fifo enabled
    irq_set_exclusive_handler(UART1_IRQ, uart_rx_isr);
    irq_set_enabled(UART1_IRQ, true);
}

// TODO: make data available to wavegen function somehow, 
//       pipe directly to wave gen tables?
//       talk to Gabe
void uart_rx_isr() 
{

    uart_get_hw(uart1)->icr |= (1u << UART_UARTICR_RXIC_LSB); // acknowledge interrupt (writing 1 in this register clears the corresponding bit)
    int data = uart_get_hw(uart1)->dr & 0xFF;

    if(data != 248){
        midi_rx_buffer[midi_buffer_len++] = data;
    }
    if(midi_buffer_len == 2){
        int index = midi_rx_buffer[midi_buffer_len - 1] - 48;
        key_press(index);
    }
    midi_buffer_len = midi_buffer_len % MIDI_BYTE_MAX;
    
}

// Pops and interprets MIDI data in the receive FIFO.
// NOTE: active-low; 0 means key pressed, 1 means key depressed

// Sets up DMA to transfer array of MIDI notes to wave gen function
void init_dma() {

}