#include "i2cadc.h"

static uint8_t i2c_tx_buffer[ADC_CONFIG_LEN];
static int8_t i2c_rx_buffer[MAX_ADC_SAMP_BUFF];


static uint8_t ADC_read_flag = 0;
static uint16_t ADC_bytes_read = 0;
static uint16_t ADC_bytes_to_read = 0;

/**
 * @brief	I2C0 Interrupt handler
 * @return	None
 */
void I2C0_IRQHandler(void)
{
	Chip_I2C_MasterStateHandler(I2C0);
}

void ADC_IRQ_HANDLER(){
	ADC_read_flag = 1;

	Chip_TIMER_ClearMatch(ADC_READ_TIMER,0);
	NVIC_ClearPendingIRQ(ADC_IRQ_NVIC_NAME);
}

void Initialize_I2CADC(){
	Board_LED_Set(0,0);

    // Initialize I2C
    Board_I2C_Init(I2C0);
    Chip_I2C_Init(I2C0);
    Chip_I2C_SetClockRate(I2C0, I2C_SPEED);
    Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandler);
    NVIC_EnableIRQ(I2C0_IRQn);

    // Initialize read timer
    Chip_TIMER_Init(ADC_READ_TIMER); 							// Initialize ADC_READ_TIMER
	Chip_TIMER_PrescaleSet(ADC_READ_TIMER,ADC_PRESCALE_VALUE);	// Set prescale value
	Chip_TIMER_SetMatch(ADC_READ_TIMER,0,ADC_READ_PERIOD);		// Set ADC timer match
	Chip_TIMER_MatchEnableInt(ADC_READ_TIMER, 0);
	Chip_TIMER_ResetOnMatchEnable(ADC_READ_TIMER,0);			// Enable Reset on match hit

	NVIC_ClearPendingIRQ(ADC_IRQ_NVIC_NAME);
	NVIC_EnableIRQ(ADC_IRQ_NVIC_NAME);
}


// C bitfields have no bit-order guarantees
// This can't be used for data transmit!
// Helpful reference, though.
#if false
#define C_MAKES_SENSE_TRUE
struct ADC_CONFIG_T{
	uint16_t os: 1;
	uint16_t mux_single: 1;
	uint16_t mux_select: 2;
	uint16_t pga: 3;
	uint16_t mode: 1;
	uint16_t dr: 3;
	uint16_t compconf: 3;
	uint16_t compqueu: 2;
};
#endif //false

void Config_ADC(uint8_t mux){
	const uint8_t config_register_pointer = 0b00000001;


	// Bit[15]		OS bit (Just leave 0)
	// Bit[14]		MUX single-ended select (We want this 1)
	// Bit[13:12]	MUX select (Fill in the two LO bits of mux)
	// Bit[11:9]	PGA programming (Default is 010; we might need to change)
	// Bit[8]		MODE select (We need mode 0 -- continuous conversion)
	// Bit[7:5]		Data Rate (We need 111 -- 3.3ksps)
	// Bit[4:2]		Comparator configuration (Don't-care)
	// Bit[1:0]		Comparator Queue (Set to 11 to disable comparator)
	// const uint8_t our_default_config[] = {0b01110100,0b11100011};
	const uint8_t our_default_config[] = {0b01000100, 0b11100011};

	// Set config register as our register pointer byte
	i2c_tx_buffer[0] = config_register_pointer;

	// Write config data to our tx buffer
	i2c_tx_buffer[1] = our_default_config[0];
	i2c_tx_buffer[2] = our_default_config[1];

	// AND the mux select bits into their correct spot
	//  in the tx buffer
	i2c_tx_buffer[1] |= mux<<4;

	// Transmit.
	Chip_I2C_MasterSend(I2C0, I2C_ADC_ADDR,i2c_tx_buffer, ADC_CONFIG_LEN);
}

int8_t* Read_ADC(uint16_t samples){
	const uint8_t conversion_register_pointer = 0b00000000;

	ADC_bytes_to_read = samples;

	Chip_I2C_MasterCmdRead(I2C0, I2C_ADC_ADDR, conversion_register_pointer, (uint8_t*)(i2c_rx_buffer), 1);
	*i2c_rx_buffer -= 78; // Remove offset
	ADC_bytes_read = 1;


	Chip_TIMER_Reset(ADC_READ_TIMER);
	Chip_TIMER_Enable(ADC_READ_TIMER);

	return i2c_rx_buffer;
}

void ADC_Read_Step(){
	if(ADC_read_flag){
		ADC_read_flag = 0;

		int read = Chip_I2C_MasterRead(I2C0, I2C_ADC_ADDR, (uint8_t*)(i2c_rx_buffer+ADC_bytes_read), 1);
		if(read){
			i2c_rx_buffer[ADC_bytes_read] -= 78; // Change if we change PGA
			++ADC_bytes_read;
		}else
			Board_LED_Set(0,1);

		if(ADC_bytes_read>=ADC_bytes_to_read)
			Chip_TIMER_Disable(ADC_READ_TIMER);
	}
}


uint8_t ADC_Read_Complete(){
	return ADC_bytes_read==ADC_bytes_to_read;
}


uint16_t ADC_Bytes_Read(){
	return ADC_bytes_read;
}
