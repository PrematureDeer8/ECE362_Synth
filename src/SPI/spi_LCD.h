#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"


#define SPI_DISP_SCK 10 // spi1
#define SPI_DISP_CSn 9  // spi1
#define SPI_DISP_TX 11  // spi1

void init_chardisp_pins();
void send_spi_cmd(spi_inst_t* spi, uint16_t value);
void send_spi_data(spi_inst_t* spi, uint16_t value);
void cd_init();
void cd_display1(const char *str);
void cd_display2(const char *str);
void update_lcd(void);
