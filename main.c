//	ECE 153B W19 ROVR Project

#include "board.h"
#include "chip.h"
#include "i2cadc.h"
#include "motor.h"


#define NUM_SAMPLES 64

/**
 * @brief Runs board init functions included in every project
 */
void Initialize_Board(){
	SystemCoreClockUpdate();
	Board_Init();
}

/**
 * @brief performs a mic read for single mic mux_num over NUM_SAMPLES samples, returns avg power
 * @param mux_num the mic to read 0-3
 * @return average power of mic signal over NUM_SAMPLES samples
 */
float Read_Mic(uint8_t mux_num){
	int8_t* Sample_Data;
	float sample_avg;
	uint16_t bytes_processed = 0;
	
	Config_ADC(mux_num);
	bytes_processed = 0;
	sample_avg = 0.0;
	Sample_Data = (int8_t*)Read_ADC(NUM_SAMPLES);
	while(bytes_processed < NUM_SAMPLES){
		ADC_Read_Step();
		if (ADC_Bytes_Read() > bytes_processed){
			float converted = (float)(Sample_Data[bytes_processed++]);
			sample_avg += converted*converted;
		}
	}
	sample_avg /= NUM_SAMPLES;

	return sample_avg;
}

//Forward/backward, right/left
static float vector_arr[4];
/**
 * @brief takes a0,a1,a2,a3 arranged clockwise at 45 degree points, sets vector_arr
 * @param a float array a0,a1,a2,a3
 */
void vectorMath(const float* a){
	vector_arr[0] = a[0]+a[3]-(a[1]+a[2]);
	vector_arr[1] = a[0]+a[1]-(a[2]+a[3]);
	//return vector_differences;
}

int main(){
	Initialize_Board();
	//Initialize_Motor();
	Initialize_I2CADC();


	printf("Board initialized\r\n");


	//set_motor(0,64,true);
	//set_motor(1,64,true);
	//Motor_Enable();


	uint8_t fc = 0;
	float sample_avgs[4] = {0.0f,0.0f,0.0f,0.0f};

	while(1){
		uint8_t mux_num = 0;
		for(;mux_num<4;++mux_num){
			sample_avgs[mux_num] = Read_Mic(mux_num);
			if(fc){
				vectorMath(sample_avgs);

				//TODO : Use vector_differences to set PWM.
			}
		}

		fc = 1;
	}
		
	return 0;
}
