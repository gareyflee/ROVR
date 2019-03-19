//	ECE 153B W19 ROVR Project
#include "board.h"
#include "chip.h"
#include "i2cadc.h"
#include "motor.h"


#define num_samples 64

/**
 * @brief Runs board init functions included in every project
 */
void Initialize_Board(){
	SystemCoreClockUpdate();
	Board_Init();
}

float Read_Mic(uint8_t mux_num){
	int8_t* Sample_Data;
	float sample_avg;
	uint16_t bytes_processed = 0;
	
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

	return sample_avg;
}

int main(){
	Initialize_Board();
	//Initialize_Motor();
	Initialize_I2CADC();
	printf("Garey suxx\r\n");
	//set_motor(0,64,true);
	//set_motor(1,64,true);
	//Motor_Enable();

	float differences[33];
	uint8_t i=0;
	for(;i<33;++i){
		uint8_t mux_num = 0;
		float sample_avgs[4] = {0,0,0,0};
		for(;mux_num<2;++mux_num){
			sample_avgs[mux_num] = Read_Mic(mux_num);
			//printf("Avg for mic num %i is: %f\r\n", mux_num, sample_avgs[mux_num]);
		}
		differences[i] = sample_avgs[0]-sample_avgs[1];
	}
	for(i=0;i<33;++i){
		printf("%f,",differences[i]);
	}

		
	return 0;
}
