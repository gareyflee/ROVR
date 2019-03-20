#include <stdint.h>

#ifndef PWM_C
#define PWM_C

#include "board.h"

#define TIMER0_IRQ_HANDLER				TIMER0_IRQHandler  // TIMER0 interrupt IRQ function name
#define TIMER0_INTERRUPT_NVIC_NAME		TIMER0_IRQn        // TIMER0 interrupt NVIC interrupt name

#define TIMER0_PRESCALE_VALUE 12000	// Clock cycle / 10000 (set to 1/10 ms increments)

#define MY_TIME_EFFECT 256

#define PWM_0_PORT 2
#define PWM_0_PIN 11

#define PWM_1_PORT 2
#define PWM_1_PIN 13

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief Initialize PWM GPIO pins and timer block
 */
void Initialize_PWM();

/**
 * @brief Start PWM timer block
 */
void PWM_Enable();

/**
 * @brief Halt PWM timer block
 *
 * Note: Does not disable PWM output signal!
 */
void PWM_Disable();

/**
 * @brief Set PWM power
 * @param PWM_ID the PWM unit to change
 * @param pwm_val 0-255 value representing 0-100% duty cycle
 */
void set_pwm(uint8_t PWM_ID,uint8_t pwm_val);

#endif //PWM_C
