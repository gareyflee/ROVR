#ifndef MOTOR_H
#define MOTOR_H

#include "pwm.h"

#define NUM_MOTORS 2

void Initialize_Motor();

void Motor_Enable();
void Motor_Disable();

void set_motor(uint8_t MOTOR_ID, uint8_t power, bool forward);

#endif //MOTOR_H
