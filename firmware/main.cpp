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

BusOut *led;
Battery *bat;
Encoders *enc;
Motor *mt;
Buzzer *bz;
Button *btn;
MPU6500 *mpu;
Reflector *rfl;

Ticker ctrl;

void print_info() {
	printf("Gz: %.2f\tAngle: %.2f\tInt: %.2f\n", mpu->gz, mpu->angle,
			mpu->int_angle);
//	printf("%.1f\t\t%.1f\t\t%.1f\t\t%.1f\n", rfl->sl(), rfl->fl(), rfl->fr(),
//			rfl->sr());
//	printf("%d\n", mpu->ay);
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
		turn = (0 - mpu->gz) * 1 + (0 - mpu->angle) * 100
				+ (0 - mpu->int_angle) * 10;
		left = target - enc->left();
		right = target - enc->left();
		left = (left > 100) ? 100 : left;
		right = (right > 100) ? 100 : right;
		if (abs(right) < 10 && abs(left) < 10) {
			mpu->angle -= 180;
			state = 2;
		}
		break;
	case 2:
		turn = (0 - mpu->gz) * 1 + (0 - mpu->angle) * 100
				+ (0 - mpu->int_angle) * 10;
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
	mt->drive(valueL, valueR);
}

int main() {
	/* make instances */
	led = new BusOut(LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN);
	bat = new Battery(BATTERY_PIN);
	enc = new Encoders(ENCODER_L_TIMx, ENCODER_R_TIMx);
	mt = new Motor;
	bz = new Buzzer(BUZZER_PIN);
	btn = new Button(BUTTON_PIN);
	mpu = new MPU6500(MPU6500_MOSI_PIN, MPU6500_MISO_PIN, MPU6500_SCLK_PIN,
	MPU6500_SSEL_PIN);
//	rfl = new Reflector(IR_LED_SL_FR_PIN, IR_LED_SR_FL_PIN);

	printf("\nHello World!\n");
	float voltage = bat->voltage();
	printf("Battery Voltage: %.3f [V]\n", voltage);
	if (!bat->check()) {
		bz->play(BUZZER_MUSIC_LOW_BATTERY);
		printf("Battery Low!\n");
		while (1) {
			wait(1);
		}
	}
	bz->play(BUZZER_MUSIC_BOOT);
	*led = 0x6;
	wait(0.2);
	*led = 0x9;

	/* initializations */
	Ticker t;
	t.attach_us(print_info, 100000);

	while (true) {
//		static int32_t prev_enc_L, prev_enc_R;
//		int32_t enc_L = enc->left();
//		int32_t enc_R = enc->right();
//		if (enc_L != prev_enc_L || enc_R != prev_enc_R) {
//			prev_enc_L = enc_L;
//			prev_enc_R = enc_R;
//			printf("L: %ld\tR: %ld\n", enc_L, enc_R);
//		}

		if (btn->pressed) {
			btn->flags = 0;
			bz->play(BUZZER_MUSIC_SELECT);
			ctrl.detach();
			mt->free();
		}
		if (btn->long_pressed_1) {
			btn->flags = 0;
			bz->play(BUZZER_MUSIC_CONFIRM);
			ctrl.attach_us(ctrl_arts, 100);
		}
		if (mpu->ay < -20000) {
			ctrl.detach();
			mt->free();
			bz->play(BUZZER_MUSIC_EMERGENCY);
			while (1) {
				wait(1);
			}
		}
	}
}
