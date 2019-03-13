#ifndef I2CADC_H
#define I2CADC_H

#include "board.h"
#include "chip.h"

#define I2C_SPEED			400000
#define I2C_ADC_ADDR		0x48

#define ADC_CONFIG_LEN		3 // Length of ADC config packet in bytes, including config register set

#define ADC_READ_TIMER 		LPC_TIMER1
#define ADC_TIMER_PRESCALE	1200 		// Clock cycle / 1000 (set to 1 microsecond increments)
#define ADC_READ_PERIOD		303 		// In microseconds

volatile extern uint8_t ADC_isRead = False;

/**
 * @brief Sets up I2C0 and ADC_READ_TIMER for adc operation
 */
void Initialize_I2CADC();

/**
 * @brief Configures the ADC for our project and sets mux value
 * @param mux the multiplexer input to use
 */
void Config_ADC(uint8_t mux);

#define MAX_ADC_SAMP_BUFF 1024

/**
 * @brief Reads samples single upper bytes from ADC conversion register
 * @param samples the number of samples to read, at most MAX_ADC_SAMP_BUFF (Not yet implemented)
 * @return pointer to array of returned data, length samples
 *
 * Returned buffer will be overwritten
 */
uint8_t* Read_ADC(uint16_t samples);

#endif //I2C_H
