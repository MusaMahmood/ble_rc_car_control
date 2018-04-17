//wheelchair_control_spi.c
#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_log.h"
#include "nrf_delay.h"
#include "wheelchair_control_spi.h"
/**@headers for µs delay:*/
#include <stdio.h> 
#include "compiler_abstraction.h"
#include "nrf.h"
#include "nrf_gpio.h"

#define RESET_VOLTAGE_INT 2053 //Formally 2083

void spi_event_handler(nrf_drv_spi_evt_t const * p_event)
{
		/*switch (p_event->type) {
				case NRF_DRV_SPI_EVENT_DONE:
					break;
				default:
					break;
		}*/
    //NRF_LOG_PRINTF(" >>> Transfer completed.\r\n");
}

/**@INITIALIZE SPI INSTANCE */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(0); //SPI INSTANCE


void spi_init(void) {
		uint8_t errcode;
		nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG(0);
		spi_config.bit_order						= NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;
		spi_config.frequency						= NRF_DRV_SPI_FREQ_125K;
		spi_config.irq_priority					= APP_IRQ_PRIORITY_LOW;
		spi_config.mode									= NRF_DRV_SPI_MODE_0; //CPOL = 0 (Active High); CPHA = 0
		spi_config.miso_pin 						= WHEELCHAIR_CONTROL_SPI_MISO;
		spi_config.sck_pin 							= WHEELCHAIR_CONTROL_SPI_SCLK;
		spi_config.mosi_pin 						= WHEELCHAIR_CONTROL_SPI_MOSI;
		spi_config.ss_pin								= NRF_DRV_SPI_PIN_NOT_USED;
		spi_config.orc									= 0x55;
		errcode = nrf_drv_spi_init(&spi, &spi_config, spi_event_handler);
		NRF_LOG_PRINTF(" SPI Init Errcode = [%d] \r\n",errcode);
		APP_ERROR_CHECK(errcode);
}

uint8_t spi_write(uint8_t data) {
	uint8_t errcode;
	uint8_t data_array[1] = { data };
	errcode = nrf_drv_spi_transfer(&spi, data_array, 1, NULL, 0);
	nrf_delay_ms(50);
	//if (errcode != 0) {
		NRF_LOG_PRINTF(" SPI TX Errcode = [%d] \r\n",errcode);
	//}
	return errcode;
}

