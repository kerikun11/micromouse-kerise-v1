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
GyroMeasure *gm;

Ticker driveTicker;
Ticker infoTicker;
Ticker wallTicker;

double target = 0;
bool wall[3];

void print_info() {
//			printf(
//			"Gz: %.2lf\tAngle: %.2lf\tInt: %.2lf\tTargetAngle: %.2lf\tTarget: %.2lf\n",
//			gm->gyro(), gm->angle(), gm->int_angle(), gm->target(), target);

	printf("%08lu\t%08lu\t%08lu\t%08lu\n", (uint32_t) rfl->sl(),
			(uint32_t) rfl->fl(), (uint32_t) rfl->fr(), (uint32_t) rfl->sr());

//	printf("%s %s %s\n", wall[0] ? "X" : ".", wall[1] ? "X" : ".",
//			wall[2] ? "X" : ".");
}

void motor_drive() {
	double turn = 0;
	double left = 0;
	double right = 0;

	turn = gm->get_pid(100, 0.1, 1);
	turn = (turn > 100) ? 100 : turn;
	turn = (turn < -100) ? -100 : turn;

	left = target - enc->left() - enc->right();
	left = (left > 50) ? 50 : left;
	left = (left < -50) ? -50 : left;

	right = target - enc->left() - enc->right();
	right = (right > 50) ? 50 : right;
	right = (right < -50) ? -50 : right;

	int32_t valueL = left - turn / 2;
	int32_t valueR = right + turn / 2;
	mt->drive(valueL, valueR);
}

void wall_detect() {
	if (rfl->sl() > 1000000)
		wall[0] = true;
	else if (rfl->sl() < 500000)
		wall[0] = false;
	if (rfl->sr() > 1000000)
		wall[2] = true;
	else if (rfl->sr() < 500000)
		wall[2] = false;
}

void serial_ctrl() {
	while (1) {
		int c = getchar();
		if (c == EOF)
			continue;
		printf("%c", (char) c);
		switch (c) {
		case 'g':
			bz->play(BUZZER_MUSIC_CONFIRM);
			driveTicker.attach_us(motor_drive, 100);
			break;
		case 'f':
			bz->play(BUZZER_MUSIC_SELECT);
			driveTicker.detach();
			mt->free();
			break;
		case 'h':
			gm->target(gm->target() + 10);
			break;
		case 'j':
			target -= 5000;
			break;
		case 'k':
			target += 5000;
			break;
		case 'l':
			gm->target(gm->target() - 10);
			break;
		}
	}
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
	gm = new GyroMeasure(mpu, 0);
	rfl = new Reflector(IR_LED_SL_FR_PIN, IR_LED_SR_FL_PIN);

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

	infoTicker.attach_us(print_info, 100000);
	wallTicker.attach_us(wall_detect, 10000);

	Thread serialCtrlThread;
	serialCtrlThread.start(serial_ctrl);

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
			driveTicker.detach();
			mt->free();
		}
		if (btn->long_pressed_1) {
			btn->flags = 0;
			bz->play(BUZZER_MUSIC_CONFIRM);
			driveTicker.attach_us(motor_drive, 100);
		}

		if (mpu->accelY() < -2000000) {
			driveTicker.detach();
			mt->free();
			bz->play(BUZZER_MUSIC_EMERGENCY);
			while (1) {
				wait(1);
			}
		}
	}
}
