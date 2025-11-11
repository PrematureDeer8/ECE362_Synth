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

#define PIN_MIDI_IN 36 // TODO: Define pin correctly

/* 3 bytes in a standard midi frame:

    Byte 1:
    
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

#define MIDI_BYTE_MAX 3 

// typedef struct mfif{
//     char midi_rx_buffer[MIDI_BYTE_MAX];
//     short buffer_size;
// } midi_fifo_t;

// midi_fifo_t midi_fifo;
char midi_rx_buffer[MIDI_BYTE_MAX];
short buffer_size = 0;

void push_to_fifo(char byte) 
{
    if (buffer_size < 3) {
        midi_rx_buffer[buffer_size++] = byte;
    } else {
        // something
    }
}

// Sets MIDI RX pin to UART function initializes UART0
void init_uart() 
{
    gpio_set_function(PIN_MIDI_IN, UART_FUNCSEL_NUM(uart0, PIN_MIDI_IN));
    uart_init(uart0, BAUD_RATE);
}

void attach_uart_irqs() 
{
    uart_get_hw(uart0)->imsc |= (1u << 4); // 0b1000 masks the "recieve interrupt" bit
    uart_set_fifo_enabled(uart0, false); // TODO: check if we need the fifo enabled
    irq_set_exclusive_handler(UART0_IRQ, uart_rx_isr);
    irq_set_enabled(UART0_IRQ, true);
}

// TODO:
void uart_rx_isr() 
{
    uart_get_hw(uart0)->icr |= (1u << UART_UARTICR_RXIC_LSB); // acknowledge interrupt

}

// Sets up DMA to transfer array of MIDI notes to wave gen function
void init_dma();