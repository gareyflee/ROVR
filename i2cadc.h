#ifndef I2CADC_H
#define I2CADC_H

#include "board.h"
#include "chip.h"

#define I2C_SPEED			400000
#define I2C_ADC_ADDR		0b1001001

#define ADC_CONFIG_LEN		3			// Length of ADC config packet in bytes, including config register set

#define ADC_READ_TIMER 		LPC_TIMER1
#define ADC_PRESCALE_VALUE	12			// Clock cycle / 10000 (set to 1/10 ms increments)
#define ADC_READ_PERIOD		3033 		// In microseconds/10
#define ADC_IRQ_HANDLER		TIMER1_IRQHandler
#define ADC_IRQ_NVIC_NAME	TIMER1_IRQn

#define MAX_ADC_SAMP_BUFF 256

/**
 * @brief Sets up I2C0 and ADC_READ_TIMER for adc operation
 */
void Initialize_I2CADC();

/**
 * @brief Configures the ADC for our project and sets mux value
 * @param mux the multiplexer input to use
 */
void Config_ADC(uint8_t mux);


/**
 * @brief Reads samples single upper bytes from ADC conversion register
 * @param samples the number of samples to read, at most MAX_ADC_SAMP_BUFF (Not yet implemented)
 * @return pointer to array of returned data, length samples
 *
 * Returned buffer will be overwritten by next started read
 */
int8_t* Read_ADC(uint16_t samples);

/**
 * @brief Checks flag indicating next ADC read is due and performs it if so.
 */
void ADC_Read_Step();

/**
 * @brief checks whether the read op is done yet
 */
uint8_t ADC_Read_Complete();

/**
 * @brief returns how many bytes have been read
 */
uint16_t ADC_Bytes_Read();

#endif //I2C_H
