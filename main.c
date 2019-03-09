//	ECE 153B W19 ROVR Project
#include "board.h"
#include "chip.h"

#include "motor.h"

/**
 * @brief Runs board init functions included in every project
 */
void Initialize_Board(){
	SystemCoreClockUpdate();
	Board_Init();
}

int main(){
	Initialize_Board();
	Initialize_Motor();

	printf("Garey rulez");

	set_motor(0,64,true);
	set_motor(1,64,true);
	Motor_Enable();

	while(1)
		__WFI();

	return 0;
}
