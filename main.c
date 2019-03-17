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
	//Initialize_Motor();
	Initialize_I2CADC();
	printf("Garey suxx");
	Board_LED_Set(1,0);

	//set_motor(0,64,true);
	//set_motor(1,64,true);
	//Motor_Enable();
	
	int8_t* Sample_Data;
	uint8_t mux_num = 0;
	uint16_t num_samples  = 512;
	float sample_avg;
	uint16_t bytes_processed = 0;
	
	while(1){
		Config_ADC(mux_num);
		bytes_processed = 0;
		sample_avg = 0.0;
		Sample_Data = (int8_t*)Read_ADC(num_samples);
		while(bytes_processed < num_samples){
			ADC_Read_Step();
			if (ADC_Bytes_Read() > bytes_processed){
				float converted = (float)(Sample_Data[bytes_processed++]);
				sample_avg += converted*converted;
			}
		}
		sample_avg /= num_samples;
		printf("Avg for mic num %i is: %f\r\n", mux_num, sample_avg);
		unsigned* dat = ADC_debug_dat();
		printf("i2c count: %u\r\ntimer count: %u\r\n",dat[0],dat[1]);
		Board_LED_Set(1,1);

		unsigned i=0;
		for(i=0;i<num_samples;++i){
			printf("%i,",Sample_Data[i]);
		}
	}
		
	return 0;
}
