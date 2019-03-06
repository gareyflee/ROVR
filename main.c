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

	while(1)
		__WFI();

	return 0;
}
