//+BOARD_CUSTOM
#ifndef MUSA_NRF51_BREAKOUT_H
#define MUSA_NRF51_BREAKOUT_H
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP
#define NRF_CLOCK_LFCLKSRC {                                      \
            .source= NRF_CLOCK_LF_SRC_RC,                         \
            .rc_ctiv= 2,                                          \
            .rc_temp_ctiv= 1,                                     \
            .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM    \
}
#endif
//EOF - - 
															
															
