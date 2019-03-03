//	ECE 153B W19 ROVR Project
#include "board.h"
#include "chip.h"

/**
 * @brief Runs board init functions included in every project
 */
void Initialize_Board(){
	SystemCoreClockUpdate();
	Board_Init();
}

int main(){
	Initialize_Board();
	printf("Garey Rules\n\n");

	return 0;
}
