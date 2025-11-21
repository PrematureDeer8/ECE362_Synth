#include "spi_LCD.h"
#include <stdio.h>
#include "adc_ctrlr/adc_ctrlr.h"

extern wavegen_fsm q_wavegen;
extern uint bitcrush_res;
/***************************************************************** */

// "chardisp" stands for character display, which can be an LCD or OLED
void init_chardisp_pins() {
    // fill in
    gpio_set_function(SPI_DISP_CSn, GPIO_FUNC_SPI);
    gpio_set_function(SPI_DISP_SCK, GPIO_FUNC_SPI);
    gpio_set_function(SPI_DISP_TX, GPIO_FUNC_SPI);

    spi_init(spi1, 10000);
    spi_set_format(spi1, 9, 0, 0, SPI_MSB_FIRST);
}

void send_spi_cmd(spi_inst_t* spi, uint16_t value) {
    // fill in
    while(spi_is_busy(spi) == true) 
    {
    }
    spi_write16_blocking(spi1, &value, 1);

}
void send_spi_data(spi_inst_t* spi, uint16_t value)
{
    send_spi_cmd(spi, (value | 0x100));
}

void cd_init() {
    // fill in
    sleep_ms(1);
    send_spi_cmd(spi1, 0b00111100); //8 bit interface, 2 lin display, 11 dots per char
    sleep_us(40);
    send_spi_cmd(spi1, 0b0000001100); //display on/off comand
    sleep_us(40);
    send_spi_cmd(spi1, 0b0000000001);
    sleep_ms(2);
    send_spi_cmd(spi1, 0b0000000110);
    sleep_us(40);
}

void cd_display1(const char *str) {
    // fill in
    send_spi_cmd(spi1, 0b0000010);   //return home command which sets DDRAM address to 00H into address counter (The DDRAM is what holds the displayed characters)
    for(int i = 0; i < 16; i++)
    {
        send_spi_data(spi1, str[i]);
    }
}
void cd_display2(const char *str) {
    // fill in
    send_spi_cmd(spi1, 0b0011000000);  //ddram is holding the address of the second line
     for(int i = 0; i < 16; i++)
    {
        send_spi_data(spi1, str[i]);
    }
    

}
static void make_lcd_line(char *dst16, const char *src) //this pads when the new string is shorter than previous
{
    int i = 0;
    while(i < 16 && src[i] != '\0')
    {
        dst16[i] = src[i];
        i++;
    }
    while(i < 16)
    {
        dst16[i] = ' ';
        i++;
    }
    dst16[16] = '\0';
}
void update_lcd(void)
{
    static int last_wave = -1;
    static uint last_bits = (uint)-1;
    if((q_wavegen == last_wave) && bitcrush_res == last_bits)
    {
        return; //no change
    }

    last_wave = q_wavegen;
    last_bits = bitcrush_res;
    const char *wave_name;
    if(q_wavegen == SIN_WAVE)
    {
        wave_name = "SINE";
    }
    if(q_wavegen == SQUARE_WAVE)
    {
        wave_name = "SQUARE";
    }
    if(q_wavegen == SAW_WAVE)
    {
        wave_name = "SAW";
    }
    char line1[17];
    char line2[17];
    char temp[32];
    snprintf(temp, sizeof(temp), "Wave: %s", wave_name);
    make_lcd_line(line1, temp);

    snprintf(temp, sizeof(temp), "Bits: %2u", bitcrush_res);
    make_lcd_line(line2, temp);

    cd_display1(line1);
    cd_display2(line2);

}

/***************************************************************** */