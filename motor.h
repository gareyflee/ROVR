#ifndef MOTOR_H
#define MOTOR_H

#include "pwm.h"

#define NUM_MOTORS 2

/**
 * @brief Prepares motor GPIO and PWM states
 */
void Initialize_Motor();

/**
 * @brief Begins motor PWM timer
 */
void Motor_Enable();

/**
 * @brief Halts motor PWM timer
 *
 * Note: May leave motors in an on or off state. Don't rely on this to stop the motors dead stop.
 *   Use set_motor(MOTOR_ID,0,true) first, then use Motor_Disable();
 */
void Motor_Disable();

/**
 * @brief Sets the motor PWM power and direction
 * @param MOTOR_ID the ID of the motor to change power and direction of
 * @param power 0-255 value indicating 0-100% duty cycle
 * @param forward if true then forward, else backward
 */
void set_motor(uint8_t MOTOR_ID, uint8_t power, bool forward);

#endif //MOTOR_H
