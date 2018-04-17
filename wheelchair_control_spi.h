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
	#define WHEELCHAIR_CONTROL_SPI_CS1 2
	#define WHEELCHAIR_CONTROL_SPI_CS2 1 //SWAPPED 7:28 4/26/17 
	#define WHEELCHAIR_CONTROL_SPI_MOSI 14
	#define WHEELCHAIR_CONTROL_SPI_SCLK 12
#endif

void spi_init(void);

uint8_t spi_write(uint8_t data);

