#include "motor.h"

// TODO - Is this necessary? At all?
static enum MOTOR_STATE_T{
	MOTOR_STOP = 0,
	MOTOR_FORWARD = 1,
	MOTOR_BACKWARD = 2
} motor_state[NUM_MOTORS];

void Initialize_Motor(){
	Initialize_PWM();
}

void Motor_Enable(){
	PWM_Enable();
}

void Motor_Disable(){
	PWM_Disable();
}

void set_motor(uint8_t MOTOR_ID, uint8_t power, bool forward){
	set_pwm(MOTOR_ID,power);

	if(power){
		if(forward){
			motor_state[MOTOR_ID] = MOTOR_FORWARD;
			// TODO - Add pin state changes
		}else{
			motor_state[MOTOR_ID] = MOTOR_BACKWARD;
		}
	}else{
		motor_state[MOTOR_ID] = MOTOR_STOP;
	}
}
