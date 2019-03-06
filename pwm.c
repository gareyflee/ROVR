#include <stdint.h>

#include "pwm.h"

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
	int8_t matchNum=3;
	if(Chip_TIMER_MatchPending(LPC_TIMER0,1)){
		matchNum = 1;
		Chip_GPIO_SetPinState(LPC_GPIO,PWM_0_PORT,PWM_0_PIN,false);
	}else if(Chip_TIMER_MatchPending(LPC_TIMER0,2)){
		matchNum = 2;
		Chip_GPIO_SetPinState(LPC_GPIO,PWM_1_PORT,PWM_1_PIN,false);
	}else if(Chip_TIMER_MatchPending(LPC_TIMER0,0)){
		matchNum = 0;
		Chip_GPIO_SetPinState(LPC_GPIO,PWM_0_PORT,PWM_0_PIN,true);
		Chip_GPIO_SetPinState(LPC_GPIO,PWM_1_PORT,PWM_1_PIN,true);
		NVIC_ClearPendingIRQ(TIMER0_INTERRUPT_NVIC_NAME);
	}else{
		//assert(false);
		//If you get here, you fucked up.
	}
	Chip_TIMER_ClearMatch(LPC_TIMER0,matchNum);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

void Initialize_PWM(){
	Chip_GPIO_WriteDirBit(LPC_GPIO, PWM_0_PORT, PWM_0_PIN, true);
	Chip_GPIO_WritePortBit(LPC_GPIO, PWM_0_PORT, PWM_0_PIN, true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, PWM_1_PORT, PWM_1_PIN, true);
	Chip_GPIO_WritePortBit(LPC_GPIO, PWM_1_PORT, PWM_1_PIN, true);

	Chip_IOCON_PinMux(LPC_GPIO, PWM_0_PORT, PWM_0_PIN, IOCON_FUNC0, IOCON_MODE_PULLDOWN | IOCON_DIGMODE_EN);	// Configures pin as GPIO w/ no additional function

	Chip_GPIO_SetPinDIROutput(LPC_GPIO, PWM_0_PORT, PWM_0_PIN); // Configure pin as output
	Chip_GPIO_SetPinState(LPC_GPIO,PWM_0_PORT,PWM_0_PIN,false);
	Chip_IOCON_PinMux(LPC_GPIO, PWM_1_PORT, PWM_1_PIN, IOCON_FUNC0, IOCON_MODE_PULLDOWN | IOCON_DIGMODE_EN);	// Configures pin as GPIO w/ no additional function

	Chip_GPIO_SetPinDIROutput(LPC_GPIO, PWM_1_PORT, PWM_1_PIN); // Configure pin as output
	Chip_GPIO_SetPinState(LPC_GPIO,PWM_1_PORT,PWM_1_PIN,false);



	Chip_TIMER_Init(LPC_TIMER0);								// Initialize TIMER0
	Chip_TIMER_PrescaleSet(LPC_TIMER0,TIMER0_PRESCALE_VALUE);	// Set prescale value
	Chip_TIMER_SetMatch(LPC_TIMER0,0,MY_TIME_EFFECT);			// Set match value
	Chip_TIMER_SetMatch(LPC_TIMER0,1,0);						// Set match value to 0
	Chip_TIMER_SetMatch(LPC_TIMER0,2,0);						// Set match value to 0
	Chip_TIMER_MatchEnableInt(LPC_TIMER0, 0);					// Configure to trigger interrupt on match
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0, 0);				// Configure to reset timer on match
	Chip_TIMER_MatchEnableInt(LPC_TIMER0, 1);					// Configure to trigger interrupt on match
	Chip_TIMER_MatchEnableInt(LPC_TIMER0, 2);					// Configure to trigger interrupt on match

	NVIC_ClearPendingIRQ(TIMER0_INTERRUPT_NVIC_NAME);
	NVIC_EnableIRQ(TIMER0_INTERRUPT_NVIC_NAME);
}

void PWM_Enable(){
	Chip_TIMER_Enable(LPC_TIMER0);  // Start TIMER0
}

void PWM_Disable(){
	Chip_TIMER_Disable(LPC_TIMER0);
}


void set_pwm(uint8_t PWM_ID,uint8_t pwm_val){
	const uint8_t match = PWM_ID + 1;
	if(match == 1 || match == 2){
		Chip_TIMER_SetMatch(LPC_TIMER0,match,pwm_val);
	}else{
		//If you get here, your're fucked.
	}
}
