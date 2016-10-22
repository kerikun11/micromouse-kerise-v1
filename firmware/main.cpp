#include "mbed.h"
#include "Encoder.h"

#define BATTERY_PIN		PB_1

DigitalOut led1(LED1);
DigitalOut led2(LED2);
AnalogIn battery(BATTERY_PIN);
Serial pc(USBTX, USBRX);

TIM_Encoder_InitTypeDef encoderL, encoderR;
TIM_HandleTypeDef timer3, timer4;

int main() {
	pc.baud(115200);
	printf("\nHello!\n");
	printf("SystemCoreClock = %ld [Hz]\n", SystemCoreClock);
	float voltage = battery.read() * 3.3 * 3;
	printf("Vattery Voltage: %f [V]\n", voltage);
	if (voltage < 3.7 * 2) {
		printf("Low Battery!");
		while (1)
			;
	}

	EncoderInit(&encoderL, &timer3, TIM3, 0xffff, TIM_ENCODERMODE_TI12);
	EncoderInit(&encoderR, &timer4, TIM4, 0xffff, TIM_ENCODERMODE_TI12);

	while (true) {
		static uint16_t prev_enc_L, prev_enc_R;
		uint16_t enc_L = __HAL_TIM_GET_COUNTER(&timer3);
		uint16_t enc_R = __HAL_TIM_GET_COUNTER(&timer4);
		if(enc_L!=prev_enc_L || enc_R!=prev_enc_R){
			prev_enc_L = enc_L;
			prev_enc_R = enc_R;
			printf("L: %d, R: %d\n", enc_L, enc_R);
		}
	}
}
