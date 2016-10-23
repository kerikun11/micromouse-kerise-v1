#include "mbed.h"
#include "config.h"
#include "Battery.h"
#include "Interface.h"
#include "Encoder.h"
#include "Motor.h"
#include "MPU6500.h"

BusOut led(LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN);
Battery battery(BATTERY_PIN);
Encoders encoder(ENCODER_L_TIMER, ENCODER_R_TIMER);
Motors motor(MOTOR_L_CTRL1_PIN, MOTOR_L_CTRL2_PIN, MOTOR_R_CTRL1_PIN,
MOTOR_R_CTRL2_PIN);
Buzzer buzzer(BUZZER_PIN);
Button button(BUTTON_PIN);
MPU6500 mpu(MPU6500_MOSI_PIN, MPU6500_MISO_PIN, MPU6500_SCLK_PIN,
MPU6500_SSEL_PIN);

void print_gyro() {
	printf("Gz: %.2f\tAngle: %.2f\tInt: %.2f\n", mpu.gz, mpu.angle,
			mpu.int_angle);
}

void ctrl_arts() {
	float turn = (0 - mpu.gz) * 1 + (0 - mpu.angle) * 100
			+ (0 - mpu.int_angle) * 1;
//				turn = 0;
	float straight = (300000 - encoder.left() - encoder.right()) * 0.01;
	if (straight > 100)
		straight = 100;
	straight = 0;
	int32_t valueL = straight - turn / 2;
	int32_t valueR = straight + turn / 2;
	if (valueL > 100) {
		valueL = 100;
	} else if (valueL > -100) {
	} else {
		valueL = -100;
	}
	if (valueR > 100) {
		valueR = 100;
	} else if (valueR > -100) {
	} else {
		valueR = -100;
	}
	motor.drive(valueL, valueR);
}

int main() {
	printf("\nHello World!\n");
	printf("Battery Voltage: %.3f [V]\n", battery.voltage());
	if (!battery.check()) {
		buzzer.playLowBattery();
		printf("Battery Low!\n");
		while (1) {
			wait(1);
		}
	}
	buzzer.playBoot();

	button.init();
	mpu.init();

	Ticker t;
	t.attach(print_gyro, 0.2);

	Ticker ctrl;

	while (true) {
//		static int32_t prev_enc_L, prev_enc_R;
//		int32_t enc_L = encoder.left();
//		int32_t enc_R = encoder.right();
//		if (enc_L != prev_enc_L || enc_R != prev_enc_R) {
//			prev_enc_L = enc_L;
//			prev_enc_R = enc_R;
//			printf("L: %ld\tR: %ld\n", enc_L, enc_R);
//		}

		if (button.pressed) {
			button.flags = 0;
			buzzer.playSelect();
			ctrl.detach();
			motor.free();
		}
		if (button.long_pressed_1) {
			button.flags = 0;
			buzzer.playConfirm();
			ctrl.attach_us(ctrl_arts, 100);
		}
	}
}

