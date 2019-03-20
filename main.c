#include "board.h"
#include "chip.h"
#include "i2cadc.h"
#include "motor.h"

#define FRONT 	0
#define LEFT 	1
#define BACK 	2
#define RIGHT 	3
#define NONE	5

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
#define LR_SCALAR 				2.5

#define MIC_THRESH				250.0


#define SLEEP_TIMER_PRESCALE	120000
#define SLEEP_TIMER_MATCH		100
#define SLEEP_TIMER				LPC_TIMER2
#define SLEEP_TIMER_IRQ_HANDLER				TIMER2_IRQHandler
#define SLEEP_TIMER_INTERRUPT_NVIC_NAME		TIMER2_IRQn

#define MOTOR_SPEED_MAX 255
#define MOTOR_SPEED_TURN 128

#define MOTOR_LEFT		1
#define MOTOR_RIGHT 	0
#define MOTOR_FORWARD 	true
#define MOTOR_BACKWARD	false
static bool FB_Sleep = false;
static bool LR_Sleep = false;

void SLEEP_TIMER_IRQ_HANDLER(void){
	Chip_TIMER_Disable(SLEEP_TIMER);
	FB_Sleep = LR_Sleep = false;
	Chip_TIMER_ClearMatch(SLEEP_TIMER,0);
	NVIC_ClearPendingIRQ(SLEEP_TIMER_INTERRUPT_NVIC_NAME);
}

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

void Initialize_Sleep_Timer(){
	Chip_TIMER_Init(SLEEP_TIMER);								// Initialize TIMER0
	Chip_TIMER_PrescaleSet(SLEEP_TIMER,SLEEP_TIMER_PRESCALE);	// Set prescale value
	Chip_TIMER_SetMatch(SLEEP_TIMER,0,SLEEP_TIMER_MATCH);
	Chip_TIMER_MatchEnableInt(SLEEP_TIMER, 0);					// Configure to trigger interrupt on match
	Chip_TIMER_ResetOnMatchEnable(SLEEP_TIMER, 0);

	NVIC_ClearPendingIRQ(SLEEP_TIMER_INTERRUPT_NVIC_NAME);
	NVIC_EnableIRQ(SLEEP_TIMER_INTERRUPT_NVIC_NAME);
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

void Update_Motor_Direction(int dir){
	static int recent_direction = NONE;
	if (dir == FRONT){
		set_motor(MOTOR_LEFT, MOTOR_SPEED_MAX, MOTOR_FORWARD);
		set_motor(MOTOR_RIGHT, MOTOR_SPEED_MAX, MOTOR_FORWARD);
	}
	else if (dir == BACK){
		set_motor(MOTOR_LEFT, MOTOR_SPEED_MAX, MOTOR_BACKWARD);
		set_motor(MOTOR_RIGHT, MOTOR_SPEED_MAX, MOTOR_BACKWARD);
	}
	else if (dir == LEFT){
		set_motor(MOTOR_LEFT, MOTOR_SPEED_TURN, MOTOR_FORWARD);
		set_motor(MOTOR_RIGHT, MOTOR_SPEED_MAX, MOTOR_FORWARD);

	}
	else if (dir == RIGHT){
		set_motor(MOTOR_LEFT, MOTOR_SPEED_MAX, MOTOR_FORWARD);
		set_motor(MOTOR_RIGHT, MOTOR_SPEED_TURN, MOTOR_FORWARD);

	}
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
	uint8_t mic_order[NUM_MICS] = {FRONT, BACK, LEFT, RIGHT};
	int mux_num = 0;
	int saved_sample_num = 0;
	float current_mic_val = 0;
	while (!is_found){
		current_mic_val = Read_Mic(mic_order[mux_num]);
		if (current_mic_val >= MIC_THRESH || saved_sample_num){
			ret_mic_vals[mic_order[mux_num]] = current_mic_val;
			saved_sample_num++;
		}
		if (saved_sample_num >3)
			is_found = true;
		if (mux_num++ >= NUM_MICS)
			mux_num = 0;
	}

}

int Get_Direction(float *mic_vals){
	bool fb_sleep_temp = false;
	bool lr_sleep_temp = false;
	static int Direction_Counter[NUM_MICS] = {0};
	if (!LR_Sleep){
		if (mic_vals[LEFT] >= mic_vals[RIGHT] * LR_SCALAR && mic_vals[LEFT] >= MIC_THRESH){
			lr_sleep_temp = true;
			Direction_Counter[LEFT]++;
			Direction_Counter[RIGHT] = 0;
			if (Direction_Counter[LEFT] > 1){
//				printf("Left\r\n");
				Update_Motor_Direction(LEFT);
				Direction_Counter[LEFT] = 0;
			}

		}
		else if (mic_vals[RIGHT] >= mic_vals[LEFT] * LR_SCALAR && mic_vals[RIGHT] >= MIC_THRESH){
			lr_sleep_temp = true;
			Direction_Counter[RIGHT]++;
			Direction_Counter[LEFT] = 0;
			if (Direction_Counter[RIGHT] > 1){
//				printf("Right\r\n");
				Update_Motor_Direction(RIGHT);
				Direction_Counter[RIGHT] = 0;
			}
		}
	}
	if (!FB_Sleep){
		if (mic_vals[FRONT] >= mic_vals[BACK] * FB_SCALAR && mic_vals[FRONT] >= MIC_THRESH){
			fb_sleep_temp = true;
			Direction_Counter[FRONT]++;
			Direction_Counter[BACK] = 0;
			if (Direction_Counter[FRONT] > 1){
//				printf("Front\r\n");
				Update_Motor_Direction(FRONT);
				Direction_Counter[FRONT] = 0;
			}

		}
		else if (mic_vals[BACK] >= mic_vals[FRONT] * FB_SCALAR && mic_vals[BACK] >= MIC_THRESH){
			fb_sleep_temp = true;
			Direction_Counter[BACK]++;
			Direction_Counter[FRONT] = 0;
			if (Direction_Counter[BACK] > 2){
//				printf("Back\r\n");
				Update_Motor_Direction(BACK);
				Direction_Counter[BACK] = 0;
			}

		}
	}




	if (fb_sleep_temp)
		FB_Sleep = true;
	if (lr_sleep_temp)
		LR_Sleep = true;

	if (fb_sleep_temp || lr_sleep_temp){
		Chip_TIMER_Reset(SLEEP_TIMER);
		Chip_TIMER_Enable(SLEEP_TIMER);
	}


	return 0;
}


int main(){
//	printf("Killing it....\r\n");

	Initialize_Board();
	Initialize_Motor();
	Initialize_I2CADC();
	Initialize_Sleep_Timer();
	Update_Motor_Direction(NONE);
	Motor_Enable();

	bool is_turning = false;
	bool is_on = true;

	while (is_on){
		float mic_vals[NUM_MICS] = {0};
		Ping_Mics(mic_vals);
		Get_Direction(mic_vals);



	}

	return 0;
}
