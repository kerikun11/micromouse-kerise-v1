#include "mbed.h"
#include "config.h"
#include "Battery.h"
#include "Encoder.h"
#include "Motor.h"

BusOut led(LED1, LED2, LED3, LED4);
Battery battery(BATTERY_PIN);
Encoder encoderL(ENCODER_TIMER_L);
Encoder encoderR(ENCODER_TIMER_R);
Motor motorL(MOTOR_CTRL1_L, MOTOR_CTRL2_L);
Motor motorR(MOTOR_CTRL1_R, MOTOR_CTRL2_R);

int main() {
	printf("\nHello World!\n");
	printf("Battery Voltage: %f\n", battery.voltage());
	if (!battery.check()) {
		printf("Battery Low!\n");
		while (1) {
			wait(1);
		}
	}
	printf("Drive\n");
	motorL.drive(100);
	motorR.drive(-100);
	wait(2);
	printf("Brake\n");
	motorL.drive(0);
	motorR.drive(0);
	wait(2);
	printf("Free\n");
	motorL.free();
	motorR.free();

	while (true) {
		static uint32_t prev_enc_L,prev_enc_R;
		uint32_t enc_L = encoderL.value();
		uint32_t enc_R = encoderR.value();
		if(enc_L != prev_enc_L || enc_R != prev_enc_R){
			prev_enc_L = enc_L;
			prev_enc_R = enc_R;
			printf("L: %ld\tR: %ld\n", enc_L, enc_R);
		}
	}
}

