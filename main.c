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

#define NUM_MICS 4

//Forward/backward, right/left
static float vector_arr[2] = {0.0f,0.0f};
//Forward positive, right positive

/**
 * @brief takes a0,a1,a2,a3 arranged clockwise at 45 degree points, sets vector_arr
 * @param a float array a0,a1,a2,a3
 */
void vectorMath(const float* a){
	vector_arr[0] = a[0]+a[3]-(a[1]+a[2]);
	vector_arr[1] = a[0]+a[1]-(a[2]+a[3]);
	//return vector_differences;
}

#define MOTOR_SPEED 64
#define SOUND_THRESH 125.0

/**
 * @brief update the motor state with the current vector_arr data
 */
void updateMotors(){
	if(vector_arr[0] > SOUND_THRESH){
		set_motor(0,MOTOR_SPEED,true);
		set_motor(1,MOTOR_SPEED,true);
	}else if(vector_arr[0] < -SOUND_THRESH){
		set_motor(0,MOTOR_SPEED,false);
		set_motor(1,MOTOR_SPEED,false);
	}else{
		set_motor(0,0,true);
		set_motor(1,0,true);
	}
}

//#define JUST_FORWARD_DEBUG

int main(){
	Initialize_Board();
	Initialize_Motor();
#ifndef JUST_FORWARD_DEBUG
	Initialize_I2CADC();


	printf("ROVR initialized\r\n");

	updateMotors();
	Motor_Enable();

	bool fc = false;
	float sample_avgs[NUM_MICS] = {0.0f,0.0f,0.0f,0.0f};

	while(true){
		uint8_t mux_num = 0;
		for(;mux_num<NUM_MICS;++mux_num){
			sample_avgs[mux_num] = Read_Mic(mux_num);
			if(fc){
				vectorMath(sample_avgs);
				updateMotors();
			}
		}

		fc = true;
	}
#else // JUST_FORWARD_DEBUG
	set_motor(0,64,true);
	set_motor(1,64,true);
	Motor_Enable();

	while(true) __WFI();
#endif // JUST_FORWARD_DEBUG
	return 0;
}

