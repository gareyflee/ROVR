//	ECE 153B W18 Lab 1E
//	Configuring ADCs

#if false

#include "board.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define TIMER0_IRQ_HANDLER				TIMER0_IRQHandler  // TIMER0 interrupt IRQ function name
#define TIMER0_INTERRUPT_NVIC_NAME		TIMER0_IRQn        // TIMER0 interrupt NVIC interrupt name

#define TIMER0_PRESCALE_VALUE 12000	// Clock cycle / 10000 (set to 1/10 ms increments)

#define ADC_IRQ_HANDLER					LPC_IRQHandler	// ADC interrupt IRQ function name
#define ADC_INTERRUPT_NVIC_NAME			ADC_IRQn		// ADC interrupt NVIC interrupt name

#ifdef BOARD_NXP_LPCXPRESSO_1769
#define ADC_CHANNEL ADC_CH0
#else
#define ADC_CHANNEL ADC_CH2
#endif

#define ADC_MAX_VAL 0b111111111111

#define ADC_BURST_BIT_RATE 1000

static ADC_CLOCK_SETUP_T ADCSetup;

// Set MY_TIME_EFFECT to
//   10000 for visible effects
//      10 for dimming
#define MY_TIME_EFFECT 10

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/**
 * @brief 	Handle interrupt from TIMER0
 * @return 	Nothing
 */
void TIMER0_IRQHandler(void)
{
	int8_t matchNum = 3;
	if(Chip_TIMER_MatchPending(LPC_TIMER0,1)){
			matchNum = 1;
			Board_LED_Set(0, false);
	}else if(Chip_TIMER_MatchPending(LPC_TIMER0,0)){
		matchNum = 0;
		Board_LED_Set(0, true);
		NVIC_ClearPendingIRQ(TIMER0_INTERRUPT_NVIC_NAME);
		//Timer reset handled by TIMER_RESET_ON_MATCH flag.
	}else{
		//assert(false);
		//If you get here, you fucked up.
	}

	Chip_TIMER_ClearMatch(LPC_TIMER0,matchNum);
}

/**
 * @brief	Handle interrupt from ADC
 * @return	Nothing
 */
void ADC_IRQHandler(void){
	NVIC_DisableIRQ(ADC_INTERRUPT_NVIC_NAME);
	Chip_ADC_Int_SetChannelCmd(LPC_ADC, ADC_CHANNEL, DISABLE);

	uint16_t dataADC;
	Chip_ADC_ReadValue(LPC_ADC, ADC_CHANNEL, &dataADC);

	//Debug: light up LED1 when ADC is over 50%.
	/*
	if(dataADC > ADC_MAX_VAL/2){
		Board_LED_Set(1,true);
	}else{
		Board_LED_Set(1,false);
	}*/

	uint16_t newMatch = (dataADC*MY_TIME_EFFECT)/ADC_MAX_VAL;
	Chip_TIMER_SetMatch(LPC_TIMER0,1,newMatch);	// Set match value


	NVIC_EnableIRQ(ADC_INTERRUPT_NVIC_NAME);
	Chip_ADC_Int_SetChannelCmd(LPC_ADC, ADC_CHANNEL, ENABLE);
};

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Main program body
 * @return	Does not return
 */
int main(void)
{
	// Generic Initialization
	SystemCoreClockUpdate();
	Board_Init();

	// Turn off LED0
	Board_LED_Set(0, false);

	//// Setup interrupts

	// TIMER0:
	Chip_TIMER_Init(LPC_TIMER0);						// Initialize TIMER0
	Chip_TIMER_PrescaleSet(LPC_TIMER0,TIMER0_PRESCALE_VALUE);	// Set prescale value
	Chip_TIMER_SetMatch(LPC_TIMER0,0,MY_TIME_EFFECT);	// Set match value
	Chip_TIMER_SetMatch(LPC_TIMER0,1,MY_TIME_EFFECT/2);	// Set match value
	Chip_TIMER_MatchEnableInt(LPC_TIMER0, 0);		// Configure to trigger interrupt on match
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0, 0);	// Configure to reset timer on match
	Chip_TIMER_MatchEnableInt(LPC_TIMER0, 1);		// Configure to trigger interrupt on match

	// ADC:
	Chip_ADC_Init(LPC_ADC,&ADCSetup);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CHANNEL, ENABLE);

	//// Start interrupts
	// TIMER0:
	NVIC_ClearPendingIRQ(TIMER0_INTERRUPT_NVIC_NAME);
	NVIC_EnableIRQ(TIMER0_INTERRUPT_NVIC_NAME);

	// ADC:
	NVIC_ClearPendingIRQ(ADC_INTERRUPT_NVIC_NAME);
	NVIC_EnableIRQ(ADC_INTERRUPT_NVIC_NAME);
	Chip_ADC_Int_SetChannelCmd(LPC_ADC, ADC_CHANNEL, ENABLE);
	ADCSetup.burstMode = true;
	Chip_ADC_SetSampleRate(LPC_ADC, &ADCSetup, ADC_BURST_BIT_RATE);
	Chip_ADC_SetBurstCmd(LPC_ADC, ENABLE);

	// Start timer0 here
	Chip_TIMER_Enable(LPC_TIMER0);  // Start TIMER0

	// Wait for interrupts - LED will toggle on each wakeup event
	while (1) {
		__WFI();
	}

	return 0;
}

#endif /* Disable unused code */
