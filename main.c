//	ECE 153B W19 ROVR Project
#include "board.h"
#include "chip.h"
#include "i2cadc.h"
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
	Initialize_I2CADC();
	printf("Garey rocks my socks");

	set_motor(0,64,true);
	set_motor(1,64,true);
	Motor_Enable();
	
	
	uint8_t* Sample_Data;
	uint8_t mux_num = 0;
	uint8_t num_samples  = 512;
	float sample_avg;
	uint8_t isDone = 0;
	uint8_t bytes_processed = 0;
	
	while(True){
		Config_ADC(mux_num);
		bytes_processed = 0;
		sample_avg = 0.0;
		Sample_Data = Read_ADC(num_samples);
		while(bytes_processed < num_samples){
			if (ADC_Bytes_Read() > bytes_processed){
				sample_avg += float(Sample_Data[bytes_processed++]);			
			}
			else{
				__WFI();	
			}
		}
		sample_avg /= num_samples;
		printf("Avg for mic num %i is: %d\r\n", mux_num, sample_avg);
	}
		
	return 0;
}
