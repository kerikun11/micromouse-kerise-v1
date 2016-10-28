#include "mbed.h"
#include "config.h"
#include "Battery.h"
#include "Buzzer.h"
#include "Button.h"
#include "Encoder.h"
#include "Motor.h"
#include "MPU6500.h"
#include "Reflector.h"
#include "Controller.h"

BusOut leds(LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN);
Battery battery(BATTERY_PIN);
Encoders encoder(ENCODER_L_TIMER, ENCODER_R_TIMER);
Motor motor;
Buzzer buzzer(BUZZER_PIN);
Button button(BUTTON_PIN);
MPU6500 mpu(MPU6500_MOSI_PIN, MPU6500_MISO_PIN, MPU6500_SCLK_PIN,
MPU6500_SSEL_PIN);
Reflector reflector(IR_LED_SL_FR_PIN, IR_LED_SR_FL_PIN);
Controller controller;

Ticker ctrl;

void print_info() {
	printf("Gz: %.2f\tAngle: %.2f\tInt: %.2f\n", mpu.gz, mpu.angle,
			mpu.int_angle);
//	printf("ADC: %d\n", uhADCxConvertedValue);
//	printf("%.1f\t\t%.1f\t\t%.1f\t\t%.1f\n", reflector.sl(), reflector.fl(),
//			reflector.fr(), reflector.sr());
//	printf("%d\n", mpu.ay);
}

void ctrl_arts() {
	static float turn = 0;
	static float left = 0;
	static float right = 0;
	static int state = 0;
	static float target = 0;
	switch (state) {
	case 0:
		target = 100000;
		state = 1;
		break;
	case 1:
		turn = (0 - mpu.gz) * 1 + (0 - mpu.angle) * 100
				+ (0 - mpu.int_angle) * 10;
		left = target - encoder.left();
		right = target - encoder.left();
		left = (left > 100) ? 100 : left;
		right = (right > 100) ? 100 : right;
		if (abs(right) < 10 && abs(left) < 10) {
			mpu.angle -= 180;
			state = 2;
		}
		break;
	case 2:
		turn = (0 - mpu.gz) * 1 + (0 - mpu.angle) * 100
				+ (0 - mpu.int_angle) * 10;
		turn = (turn > 100) ? 100 : turn;
		turn = (turn < -100) ? -100 : turn;
		left = 0;
		right = 0;
		if (abs(turn) < 10 && abs(turn) < 10) {
			target += 100000;
			state = 1;
		}
		break;
	}
	int32_t valueL = left - turn / 2;
	int32_t valueR = right + turn / 2;
	motor.drive(valueL, valueR);
}

int main() {
	printf("\nHello World!\n");
	float voltage = battery.voltage();
	printf("Battery Voltage: %.3f [V]\n", voltage);
	if (!battery.check()) {
		buzzer.play(BUZZER_MUSIC_LOW_BATTERY);
		printf("Battery Low!\n");
		while (1) {
			wait(1);
		}
	}
	buzzer.play(BUZZER_MUSIC_BOOT);
	leds = 0x6;
	wait(0.2);
	leds = 0x9;

	/* initializations */
	button.init();
	mpu.init();
	motor.init();
	reflector.init();

	Ticker t;
	t.attach_us(print_info, 100000);

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
			buzzer.play(BUZZER_MUSIC_SELECT);
			ctrl.detach();
			motor.free();
		}
		if (button.long_pressed_1) {
			button.flags = 0;
			buzzer.play(BUZZER_MUSIC_CONFIRM);
			ctrl.attach_us(ctrl_arts, 100);
		}
		if (mpu.ay < -20000) {
			ctrl.detach();
			motor.free();
			buzzer.play(BUZZER_MUSIC_EMERGENCY);
			while (1) {
				wait(1);
			}
		}
	}
}
