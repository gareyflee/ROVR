#include "Chip.h"

#define TIMER0_IRQ_HANDLER				TIMER0_IRQHandler  
#define TIMER0_INTERRUPT_NVIC_NAME			TIMER0_IRQn      
#define TIMER_0_MATCH					100
#define TIMER0_PRESCALE_VALUE 12000	// Clock cycle / 10000 (set to 1/10 ms increments)

#define TRUE						1
#define FALSE						0


void TIMER0_IRQHandler(void){
	Chip_TIMER_Disable(LPC_TIMER0);		  	// Stop TIMER0
	Chip_TIMER_Reset(LPC_TIMER0);		 	// Reset TIMER0
	Chip_TIMER_ClearMatch(LPC_TIMER0,0);  		// Clear TIMER0 match
	Chip_TIMER_Enable(LPC_TIMER0);			// Enable Timer0
}

void Initialize_Timers(){
	// Initialize Timer0
	Chip_TIMER_Init(LPC_TIMER0);
	Chip_TIMER_PrescaleSet(LPC_TIMER0,TIMER0_PRESCALE_VALUE);
	Chip_TIMER_SetMatch(LPC_TIMER0,0,TIMER_0_MATCH);
	Chip_TIMER_MatchEnableInt(LPC_TIMER0, 0);
  

	// Enable interrupts in the NVIC for the timers
	NVIC_ClearPendingIRQ(TIMER0_INTERRUPT_NVIC_NAME);
	NVIC_EnableIRQ(TIMER0_INTERRUPT_NVIC_NAME);
}
  
