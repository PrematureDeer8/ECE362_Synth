#ifndef UART_MIDI_H
#define UART_MIDI_H
/*  Functions for receiving MIDI data over UART on RP2350.

    Description: Prepares RP2350 to raise UART interrupts when a user presses
    a key on a 25-key octave MIDI keyboard. A DMA pipes the MIDI buffer
    to the wave generator functions to produce the right notes

    Author: Alex Resendiz
    Project: ECE 362 Final Design Project
*/

// Sets MIDI RX pin to UART function initializes UART0
void init_uart();

// Attaches exclusive UART interrupts. Runs once at startup.
// MIDI byte data is sent over UART.
void attach_uart_irqs();

// On receiving a UART interrrupt, collects up to 3 MIDI bytes
// No return values b/c DMA handles memory transfer on interrupt
void uart_rx_isr();

// Sets up DMA to transfer array of MIDI notes to wave gen function
void init_dma();

// Stuffs midi bytes to midi buffer in the order recieved.
// Handles overflow
void push_to_midi_fifo();

// ??? Maybe a queue would be better?
void pop_from_midi_fifo();
#endif