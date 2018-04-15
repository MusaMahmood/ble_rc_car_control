//wheelchair_control_spi.h

#include "app_util_platform.h"
//SPI PINS

//LED Pins
#if defined(BOARD_WHC_CTRL_V1)
	#define LED0 14
	#define LED1 13
	#define LED2 15
	#define LED3 16
	#define WHEELCHAIR_CONTROL_SPI_CS1 9
	#define WHEELCHAIR_CONTROL_SPI_CS2 10
	#define WHEELCHAIR_CONTROL_SPI_MOSI 8
	#define WHEELCHAIR_CONTROL_SPI_SCLK 11
#endif

#if defined(BOARD_WHC_CTRL_V2)
	#define LED0 4 //POWER
	#define LED1 3
	#define LED2 15
	#define LED3 16
	#define WHEELCHAIR_CONTROL_SPI_CS1 9
	#define WHEELCHAIR_CONTROL_SPI_CS2 12 //SWAPPED 7:28 4/26/17 
	#define WHEELCHAIR_CONTROL_SPI_MOSI 10
	#define WHEELCHAIR_CONTROL_SPI_SCLK 11
#endif

void spi_init(void);

uint8_t wheelchair_spi_write(uint8_t dac_pin, uint8_t byte1, uint8_t byte2);

uint8_t encodeFirstByte(uint8_t byte1);

uint8_t highbyte(uint16_t val);

uint8_t lowbyte(uint16_t val);

uint8_t changespeed(uint16_t *currentHex, uint16_t desiredState, uint8_t dac);

void wheelchair_set_forward(void);

void wheelchair_set_left(void);

void wheelchair_set_right(void);

void wheelchair_set_reverse(void);

void wheelchair_reset_init(void);

