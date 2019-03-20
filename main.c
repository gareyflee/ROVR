#include "board.h"
#include "chip.h"
#include "i2cadc.h"
#include "motor.h"

#define FRONT 	0
#define LEFT 	1
#define BACK 	2
#define RIGHT 	3

#define NUM_MICS	4

#define MIC_0_SCALAR			1.1307879
#define MIC_1_SCALAR			1.0893626
#define MIC_2_SCALAR			1.4021659
#define MIC_3_SCALAR			0.6933965
const static float MIC_SCALARS[] =
	{MIC_0_SCALAR, MIC_1_SCALAR, MIC_2_SCALAR, MIC_3_SCALAR};

#define NUM_MIC_READ_ITERATIONS		4
#define NUM_SAMPLES 				10

#define FB_SCALAR				2.0
#define LR_SCALAR 				2.0

#define MIC_THRESH				200.0


void Initialize_Board(){
	SystemCoreClockUpdate();
	Board_Init();
}


float Average_Data(float *samples, int len){
	float sum = 0.0;
	int i;
	for (i = 0; i < len; i++)
		sum += samples[i];
	return sum/len;
}
void Median_Filt(float* samples, int len){
	if(len<3) return;
	float temp[3];
	temp[0] = samples[0];
	temp[1] = samples[1];

	unsigned i=0;
	for(i=1;i<len-1;++i){
		temp[2] = samples[i+1];
		if((temp[1]<temp[0] && temp[0]<temp[2]) || (temp[1]>temp[0] && temp[0] > temp[2])){
			samples[i]=temp[0];
		}else if((temp[1]<temp[2] && temp[2] < temp[0]) || (temp[1]>temp[2]&&temp[2]>temp[0])){
			samples[i]=temp[2];
		}else{
			samples[i]=temp[1];
		}
		temp[0]=temp[1];
		temp[1]=temp[2];
	}
	return;
}

float Read_Mic_Samples(uint8_t mux_num){
	static uint8_t last_mux_num = -1;
	int8_t* Sample_Data;
	float sample_avg;
	uint16_t bytes_processed = 0;
	if (last_mux_num != mux_num)
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
	sample_avg /= (NUM_SAMPLES*MIC_SCALARS[mux_num]);
	last_mux_num = mux_num;
	return sample_avg*MIC_SCALARS[mux_num];
}

void Update_Motors(){



}



float Read_Mic(uint8_t mux_num){
	int iter;
	float samples[NUM_MIC_READ_ITERATIONS] = {0};
	for (iter = 0; iter < NUM_MIC_READ_ITERATIONS; iter++){
		samples[iter] = Read_Mic_Samples(mux_num);
	}
	Median_Filt(samples, NUM_MIC_READ_ITERATIONS);
	return Average_Data(samples, NUM_MIC_READ_ITERATIONS);
}

void Ping_Mics(float* ret_mic_vals){
	bool is_found = false;
	int mux_num;
	int saved_sample_num = 0;
	float current_mic_val = 0;
	while (!is_found){
		current_mic_val = Read_Mic(mux_num);
		if (current_mic_val >= MIC_THRESH || saved_sample_num){
			ret_mic_vals[mux_num] = current_mic_val;
			saved_sample_num++;

		}


		if (saved_sample_num >3)
			is_found = true;
		if (mux_num++ >= NUM_MICS)
			mux_num = 0;
	}
}

int Get_Direction(float *mic_vals){
	if (mic_vals[FRONT] >= mic_vals[BACK] * FB_SCALAR)
		printf("Front\r\n");
	else if (mic_vals[BACK] >= mic_vals[FRONT] * FB_SCALAR)
		printf("Back\r\n");

	if (mic_vals[LEFT] >= mic_vals[RIGHT] * LR_SCALAR)
			printf("Left\r\n");
		else if (mic_vals[RIGHT] >= mic_vals[LEFT] * LR_SCALAR)
			printf("Right\r\n");
	return 0;
}


int main(){
	printf("Killing it....\r\n");

	Initialize_Board();
	Initialize_Motor();
	Initialize_I2CADC();

	bool is_turning = false;
	bool is_on = true;

	while (is_on){
		float mic_vals[NUM_MICS] = {0};
		Ping_Mics(mic_vals);
		Get_Direction(mic_vals);



	}

	return 0;
}
