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
		spi_config.frequency						= NRF_DRV_SPI_FREQ_1M;
		spi_config.irq_priority					= APP_IRQ_PRIORITY_LOW;
		spi_config.mode									= NRF_DRV_SPI_MODE_0; //CPOL = 0 (Active High); CPHA = 0
		spi_config.miso_pin 						= NRF_DRV_SPI_PIN_NOT_USED;
		spi_config.sck_pin 							= WHEELCHAIR_CONTROL_SPI_SCLK;
		spi_config.mosi_pin 						= WHEELCHAIR_CONTROL_SPI_MOSI;
		spi_config.ss_pin								= NRF_DRV_SPI_PIN_NOT_USED;
		spi_config.orc									= 0x55;
		errcode = nrf_drv_spi_init(&spi, &spi_config, spi_event_handler);
		NRF_LOG_PRINTF(" SPI Init Errcode = [%d] \r\n",errcode);
		APP_ERROR_CHECK(errcode);
}

uint8_t wheelchair_spi_write(uint8_t dac, uint8_t byte1, uint8_t byte2) {
	uint8_t errcode;
	uint8_t cs_dac_pin = 0;
	if (dac == 1) {
		cs_dac_pin = WHEELCHAIR_CONTROL_SPI_CS1;
	} else {
		cs_dac_pin = WHEELCHAIR_CONTROL_SPI_CS2;
	}
	//TODO: MODIFY HIGHBYTE (BYTE1)?
	uint8_t tx_transfer_data[2] = {byte1, byte2};
	nrf_gpio_pin_clear(cs_dac_pin); //PULL CS LOW
	errcode = nrf_drv_spi_transfer(&spi, tx_transfer_data, 2, NULL, 0);
	//delay?
	//nrf_delay_ms(1);
	nrf_delay_us(100);
	nrf_gpio_pin_set(cs_dac_pin);		//PULL CS BACK HIGH
	return errcode;
}

uint8_t encodeFirstByte(uint8_t byte1) {
	uint8_t retval = 0x0F & byte1;
	return (0x30 | retval);
}

uint8_t highbyte(uint16_t val) {
	return (val >> 8);
}

uint8_t lowbyte(uint16_t val) {
	return (val & 0xFF);
}
//using iterative method: ++
//uint8_t changespeed(uint16_t *currentHex, uint16_t desiredState, uint8_t dac)
	
uint8_t changespeed(uint16_t *currentHex, uint16_t desiredState, uint8_t dac) {
		NRF_LOG_PRINTF("Entered Changespeed \r\n");	
		uint8_t errcode = 0;
		uint16_t diff; //Voltage difference in int. 
		uint16_t RoC;
		uint16_t i = 0; 
		uint8_t	stp = 100;
		uint16_t outputValue = 0;
		uint8_t byte1 = 0;
		uint8_t byte2 = 0;
		if (*currentHex > desiredState) {
			//NRF_LOG_PRINTF("Vals b4 while loop: cH:[%d]>dS:[%d] \r\n",*currentHex, desiredState);
			diff = *currentHex-desiredState;
			RoC = diff/stp;
			//NRF_LOG_PRINTF("Vals b4 while loop: [%d] [%d] \r\n",diff, RoC);
			while (i<stp) {
				*currentHex -= RoC;
				outputValue = *currentHex;
				byte1 = highbyte(outputValue);
				byte2 = lowbyte(outputValue);
				byte1 = encodeFirstByte(byte1);
				errcode = wheelchair_spi_write(dac, byte1, byte2);
				//NRF_LOG_PRINTF("Vals IN while loop:i=%d [%d] \r\n",i,*currentHex);
				i++;
			}
		} 
		
		if (desiredState > *currentHex) {
			//NRF_LOG_PRINTF("Vals b4 while loop: cH:[%d]<dS:[%d] \r\n",*currentHex, desiredState);
			diff = desiredState - *currentHex;
			RoC = (diff/stp);
			//NRF_LOG_PRINTF("Vals b4 while loop: [%d] [%d] \r\n",diff, RoC);
			while(i<stp) {
				*currentHex += RoC;
				outputValue = *currentHex;
				byte1 = highbyte(outputValue);
				byte2 = lowbyte(outputValue);
				byte1 = encodeFirstByte(byte1);
				errcode = wheelchair_spi_write(dac,byte1,byte2);
				//NRF_LOG_PRINTF("Vals IN while loop:i=%d [%d] \r\n",i,*currentHex);
				i++;
			}
		}
		//because int values are only an estimation, jump the last value:
		*currentHex = desiredState;
		outputValue = *currentHex;
		byte1 = highbyte(outputValue);
			byte2 = lowbyte(outputValue);
			byte1 = encodeFirstByte(byte1);
			errcode = wheelchair_spi_write(dac,byte1,byte2);
			//NRF_LOG_PRINTF("Final Vals:i=%d [%d] \r\n",i,*currentHex);
		return errcode;
}
/** This is a temporary function for force-setting the wheelchair into it's forward state:
*/
void wheelchair_set_forward() {
		uint8_t byte1 = 0;
		uint8_t byte2 = 0;
		uint8_t byte1_1 = 0;
		uint8_t byte2_1 = 0;
		//uint8_t errcode;
		uint16_t desiredState = 2700; //2.70*833.33 = 2249.991 (DAC 1 CONTROLLING PINS 4 and 5)
		//uint16_t desiredState = 3175; //3.81V
		byte1 = highbyte(desiredState);
		byte2 = lowbyte(desiredState);
		byte1 = encodeFirstByte(byte1);
		uint16_t desiredState2 = 2000; 	// 2.36*833.33 DAC 2 Controlling pins 2 and 7 (formerly 1967)
		//uint16_t desiredState2 = 1967; //2.36V
		byte1_1 = highbyte(desiredState2);
		byte2_1 = lowbyte(desiredState2);
		byte1_1 = encodeFirstByte(byte1_1);
		wheelchair_spi_write(1, byte1, byte2);
		wheelchair_spi_write(2, byte1_1, byte2_1);
		//NRF_LOG_PRINTF("wheelchair_reset_init errcode = [%d] \r\n",errcode);
}

void wheelchair_set_left() {
		uint8_t byte1 = 0;
		uint8_t byte2 = 0;
		uint8_t byte1_1 = 0;
		uint8_t byte2_1 = 0;
		uint16_t desiredState = RESET_VOLTAGE_INT;
		byte1 = highbyte(desiredState);
		byte2 = lowbyte(desiredState);
		byte1 = encodeFirstByte(byte1);
	
		uint16_t desiredState2 = 1708; //2.70*833.33 = 2249.991
		byte1_1 = highbyte(desiredState2);
		byte2_1 = lowbyte(desiredState2);
		byte1_1 = encodeFirstByte(byte1_1);
		wheelchair_spi_write(1, byte1, byte2);
		wheelchair_spi_write(2, byte1_1, byte2_1);
		//NRF_LOG_PRINTF("wheelchair_reset_init errcode = [%d] \r\n",errcode);
}

void wheelchair_set_right() {
		uint8_t byte1 = 0;
		uint8_t byte2 = 0;
		uint8_t byte1_1 = 0;
		uint8_t byte2_1 = 0;
		uint16_t desiredState = RESET_VOLTAGE_INT;
		byte1 = highbyte(desiredState);
		byte2 = lowbyte(desiredState);
		byte1 = encodeFirstByte(byte1);
		uint16_t desiredState2 = 2250; //2.70*833.33 = 2249.991
		byte1_1 = highbyte(desiredState2);
		byte2_1 = lowbyte(desiredState2);
		byte1_1 = encodeFirstByte(byte1_1);
		wheelchair_spi_write(1, byte1, byte2);
		wheelchair_spi_write(2, byte1_1, byte2_1);
		//NRF_LOG_PRINTF("wheelchair_reset_init errcode = [%d] \r\n",errcode);
}

void wheelchair_set_reverse() {
		uint8_t byte1 = 0;
		uint8_t byte2 = 0;
		uint8_t byte1_1 = 0;
		uint8_t byte2_1 = 0;
		uint16_t desiredState = 937;
		byte1 = highbyte(desiredState);
		byte2 = lowbyte(desiredState);
		byte1 = encodeFirstByte(byte1);
		uint16_t desiredState2 = 2083; //2.70*833.33 = 2249.991
		byte1_1 = highbyte(desiredState2);
		byte2_1 = lowbyte(desiredState2);
		byte1_1 = encodeFirstByte(byte1_1);
		wheelchair_spi_write(1, byte1, byte2);
		wheelchair_spi_write(2, byte1_1, byte2_1);
		//NRF_LOG_PRINTF("wheelchair_reset_init errcode = [%d] \r\n",errcode);
}

void wheelchair_reset_init() {
		uint8_t byte1 = 0;
		uint8_t byte2 = 0;
		uint16_t desiredState = RESET_VOLTAGE_INT;
		byte1 = highbyte(desiredState);
		byte2 = lowbyte(desiredState);
		byte1 = encodeFirstByte(byte1);
		wheelchair_spi_write(1, byte1, byte2);
		wheelchair_spi_write(2, byte1, byte2);
		//NRF_LOG_PRINTF("wheelchair_reset_init errcode = [%d] \r\n",errcode);
}

