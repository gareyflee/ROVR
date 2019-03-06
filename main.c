//	ECE 153B W19 ROVR Project
#include "board.h"
#include "chip.h"

#include "pwm.h"

/**
 * @brief Runs board init functions included in every project
 */
void Initialize_Board(){
	SystemCoreClockUpdate();
	Board_Init();
}

int main(){
	Initialize_Board();
	Initialize_PWM();

	printf("Garey rulez");

	PWM_Enable();

	while(1)
		__WFI();

	return 0;
}
