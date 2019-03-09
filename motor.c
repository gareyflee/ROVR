#include "motor.h"

static const uint8_t M_PORTS[] = {2,2,2,2};
static const uint8_t M_PINS[] = {12,14,27,26};

void Initialize_Motor(){
	Initialize_PWM();

	uint8_t MOTOR_ID;
	for(MOTOR_ID = 0;MOTOR_ID < NUM_MOTORS;++MOTOR_ID){
		Chip_GPIO_WriteDirBit(LPC_GPIO, M_PORTS[MOTOR_ID*2], M_PINS[MOTOR_ID*2], true);
		Chip_GPIO_WritePortBit(LPC_GPIO, M_PORTS[MOTOR_ID*2], M_PINS[MOTOR_ID*2], true);
		Chip_GPIO_WriteDirBit(LPC_GPIO, M_PORTS[MOTOR_ID*2+1], M_PINS[MOTOR_ID*2+1], true);
		Chip_GPIO_WritePortBit(LPC_GPIO, M_PORTS[MOTOR_ID*2+1], M_PINS[MOTOR_ID*2+1], true);

		Chip_IOCON_PinMux(LPC_GPIO, M_PORTS[MOTOR_ID*2], M_PINS[MOTOR_ID*2],
				IOCON_FUNC0, IOCON_MODE_PULLDOWN | IOCON_DIGMODE_EN);	// Configures pin as GPIO w/ no additional function
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, M_PORTS[MOTOR_ID*2], M_PINS[MOTOR_ID*2]); // Configure pin as output
		Chip_GPIO_SetPinState(LPC_GPIO,M_PORTS[MOTOR_ID*2],M_PINS[MOTOR_ID*2],false);

		Chip_IOCON_PinMux(LPC_GPIO, M_PORTS[MOTOR_ID*2+1], M_PINS[MOTOR_ID*2+1],
				IOCON_FUNC0, IOCON_MODE_PULLDOWN | IOCON_DIGMODE_EN);	// Configures pin as GPIO w/ no additional function
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, M_PORTS[MOTOR_ID*2+1], M_PINS[MOTOR_ID*2+1]); // Configure pin as output
		Chip_GPIO_SetPinState(LPC_GPIO,M_PORTS[MOTOR_ID*2+1],M_PINS[MOTOR_ID*2+1],false);
	}
}

void Motor_Enable(){
	PWM_Enable();
}

void Motor_Disable(){
	PWM_Disable();
}

void set_motor(uint8_t MOTOR_ID, uint8_t power, bool forward){
	set_pwm(MOTOR_ID,power);
	if(MOTOR_ID == 0 || MOTOR_ID == 1){
		if(power){
			if(forward){
				Chip_GPIO_SetPinState(LPC_GPIO,M_PORTS[MOTOR_ID*2],M_PINS[MOTOR_ID*2],true);
				Chip_GPIO_SetPinState(LPC_GPIO,M_PORTS[MOTOR_ID*2+1],M_PINS[MOTOR_ID*2+1],false);
			}else{
				Chip_GPIO_SetPinState(LPC_GPIO,M_PORTS[MOTOR_ID*2],M_PINS[MOTOR_ID*2],false);
				Chip_GPIO_SetPinState(LPC_GPIO,M_PORTS[MOTOR_ID*2+1],M_PINS[MOTOR_ID*2+1],true);
			}
		}else{
			Chip_GPIO_SetPinState(LPC_GPIO,M_PORTS[MOTOR_ID*2],M_PINS[MOTOR_ID*2],false);
			Chip_GPIO_SetPinState(LPC_GPIO,M_PORTS[MOTOR_ID*2+1],M_PINS[MOTOR_ID*2+1],false);
		}
	}
}
