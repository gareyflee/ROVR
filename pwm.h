#include <stdint.h>

#ifndef PWM_C
#define PWM_C

#include "board.h"

#define TIMER0_IRQ_HANDLER				TIMER0_IRQHandler  // TIMER0 interrupt IRQ function name
#define TIMER0_INTERRUPT_NVIC_NAME		TIMER0_IRQn        // TIMER0 interrupt NVIC interrupt name

#define TIMER0_PRESCALE_VALUE 12000	// Clock cycle / 10000 (set to 1/10 ms increments)

#define MY_TIME_EFFECT 100

#define PWM_0_PIN 10
#define PWM_0_PORT 2

#define PWM_1_PIN 26
#define PWM_1_PORT 2

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
void TIMER0_IRQHandler(void);

/*****************************************************************************
 * Public functions
 ****************************************************************************/

void Initialize_PWM();

void PWM_Enable();


void set_pwm(uint16_t pwm_val);

#endif //PWM_C
