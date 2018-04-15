# Bluetooth Low Energy wheelchair control firmware for nRF51 (SDK11, S130)

This firmware is for an nRF51x22 microcontroller using SDK 11.0.0 and Softdevice S130. 

In order to edit and compile, please download the SDK 11.0.0 distribution from https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v11.x.x/
Extract the repository into ...\nRF5_SDK_11.0.0\examples\\<my_projects>\\, so that the main.c file is located in a folder like:

> nRF5_SDK_11.0.0\examples\\<my_projects>\\\<repository name>\main.c


There is a single service and characteristic for wheelchair control with write characteristic enabled. 
You may send commands from a central device such as an Android or iOS device using Nordic's 'nRF Connect' application.

The following commands trigger writes to two DACs whose output can control an VR2 wheelchair controller through it's 8-pin interface.

The following table shows the command and the corresponding action:
- 0x00: Reset
- 0x01: Forward
- 0x0F: Left
- 0xF0: Right
- 0xFF: Reverse

All the DAC voltages are nerfed for safety reasons, and may require some experimentation to fix. 

The code assumes no external low-frequency clock, and therefore the internal clock source is selected: 
_From custom_board.h_
```
#ifndef NRF51_BREAKOUT_H
#define NRF51_BREAKOUT_H
//...
#define NRF_CLOCK_LFCLKSRC {                                      \
            .source= NRF_CLOCK_LF_SRC_RC,                         \
            .rc_ctiv= 2,                                          \
            .rc_temp_ctiv= 1,                                     \
            .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM    \
}
#endif
```

Also, note that for different versions of the board, the LED indicators, and the SPI pins for the DACs are different and need to be defined
```
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
```

And finally, the min and max connection intervals for Bluetooth Low Energy packet transfer are 10x1.25ms, and 50x1.25 ms respectively. You may reduce either to get a lower latency connection, at the cost of higher power consumption. 
