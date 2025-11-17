#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "uart_midi.h"

#define BAUD_RATE 115200

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

#define RP2350_PIN_MIDI_IN 36 // TODO: Define pin correctly

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
#define MIDI_TO_ENUM_OFFSET 40 // Todo, figure out default keyboard C enum

/*
       0     1     2
    |-----|-----|-----|
    f              b
*/
char midi_rx_buffer[MIDI_BYTE_MAX];
int midi_buffer_front = -1;
int midi_buffer_back = 0;
  

// Safely stores incoming MIDI data for processing. It discards messages if
// the queue is already full (this should not be noticeable to the user unless
// there's a timing hiccup.)
void push_to_queue(char byte) 
{
    if (midi_buffer_front >= MIDI_MAX_INDEX) return; // queue full
    midi_rx_buffer[midi_buffer_front++] = byte;
}

char pop_from_queue() 
{
    if (midi_buffer_front == midi_buffer_back - 1) return; // queue empty
    return midi_rx_buffer[midi_buffer_back--];
}

// Sets UART GPIO PIN to UART function, initializes UART0
void init_uart() 
{
    gpio_set_function(RP2350_PIN_MIDI_IN, UART_FUNCSEL_NUM(uart0, RP2350_PIN_MIDI_IN));
    uart_init(uart0, BAUD_RATE);
}

void attach_uart_irqs() 
{
    uart_get_hw(uart0)->imsc |= (1u << UART_UARTIMSC_RXIM_LSB); // 0b1000 masks the "recieve interrupt" bit
    uart_set_fifo_enabled(uart0, false); // TODO: check if we need the fifo enabled
    irq_set_exclusive_handler(UART0_IRQ, uart_rx_isr);
    irq_set_enabled(UART0_IRQ, true);
}

// TODO: make data available to wavegen function somehow, 
//       pipe directly to wave gen tables?
//       talk to Gabe
void uart_rx_isr() 
{
    uart_get_hw(uart0)->icr |= (1u << UART_UARTICR_RXIC_LSB); // acknowledge interrupt (writing 1 in this register clears the corresponding bit)
    if (midi_buffer_front >= MIDI_BYTE_MAX) {
        return;
    }
    push_to_queue(uart_get_hw(uart0)->dr); // read the UART rx buffer, push it to note queue
}

// Pops and interprets MIDI data in the receive FIFO.
// NOTE: active-low; 0 means key pressed, 1 means key depressed
void pipe_midi_to_wavegen() 
{
    bool note_status = pop_from_queue() & MIDI_NOTE_ON;
    int note_enum = pop_from_queue();
    int velocity = pop_from_queue();
    // keynote_status[note_enum - MIDI_TO_ENUM_OFFSET] = note_status;
}

// Sets up DMA to transfer array of MIDI notes to wave gen function
void init_dma() {

}