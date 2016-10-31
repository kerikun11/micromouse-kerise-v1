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
Motor *mt;
Buzzer *bz;
Button *btn;
Encoders *enc;
EncoderMeasure *em;
MPU6500 *mpu;
GyroMeasure *gm;
Reflector *rfl;
WallDetector *wd;

Ticker driveTicker;
Ticker ctrlTicker;
Ticker infoTicker;

void print_info() {
//	printf("Gyr: %.2lf\tAng: %.2lf\tInt: %.2lf\tTar: %.2lf\t", gm->gyro(),
//			gm->angle(), gm->int_angle(), gm->target());
//	printf("Vel: %.2lf\tPos: %.2lf\tInt: %.2lf\tTar: %.2lf\t\n",
//			em->dif_position(), em->position(), em->int_position(),
//			em->target());

	printf("%05u\t%05u\t%05u\t%05u\n", rfl->sl(), rfl->fl(), rfl->fr(),
			rfl->sr());

	printf("%s %s %s %s\n", wd->wall().side[0] ? "X" : ".",
			wd->wall().flont[0] ? "X" : ".", wd->wall().flont[1] ? "X" : ".",
			wd->wall().side[1] ? "X" : ".");
}

void motor_drive() {
	double angle = 0;
	double straight = 0;

	angle = gm->get_pid(100, 0.1, 1);
	angle = (angle > 100) ? 100 : angle;
	angle = (angle < -100) ? -100 : angle;

	straight = em->get_pid(100, 0.1, 1);
	straight = (straight > 100) ? 100 : straight;
	straight = (straight < -100) ? -100 : straight;

	int32_t valueL = straight - angle / 2;
	int32_t valueR = straight + angle / 2;
	mt->drive(valueL, valueR);
}

void ctrl_machine() {
	double wall = 0;
	if (wd->wall().side[0] && wd->wall().side[1]) {
		wall = (rfl->side(1) - rfl->side(0)) * 0.01;
		wall = (wall > 0.01) ? 0.01 : wall;
		wall = (wall < -0.01) ? -0.01 : wall;
		gm->set_target_relative(wall);
	} else if (wd->wall().side[0]) {
		wall = (800 - rfl->side(0)) * 0.01;
		wall = (wall > 0.01) ? 0.01 : wall;
		wall = (wall < -0.01) ? -0.01 : wall;
		gm->set_target_relative(wall);
	} else if (wd->wall().side[1]) {
		wall = (800 - rfl->side(1)) * 0.01;
		wall = (wall > 0.01) ? 0.01 : wall;
		wall = (wall < -0.01) ? -0.01 : wall;
		gm->set_target_relative(-wall);
	}

	if (wd->wall().flont[0] && wd->wall().flont[1]) {
		wall = (rfl->flont(0) - rfl->flont(1)) * 0.01;
		wall = (wall > 0.01) ? 0.01 : wall;
		wall = (wall < -0.01) ? -0.01 : wall;
		gm->set_target_relative(wall);
	}
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
			driveTicker.attach_us(motor_drive, 1000);
			break;
		case 'f':
			bz->play(BUZZER_MUSIC_SELECT);
			driveTicker.detach();
			mt->free();
			break;
		case 'h':
			gm->set_target_relative(10);
			break;
		case 'j':
			em->set_target_relative(-10);
			break;
		case 'k':
			em->set_target_relative(10);
			break;
		case 'l':
			gm->set_target_relative(-10);
			break;
		}
	}
}

int main() {
	/* make instances */
	led = new BusOut(LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN);
	bat = new Battery(BATTERY_PIN);
	mt = new Motor;
	bz = new Buzzer(BUZZER_PIN);
	btn = new Button(BUTTON_PIN);
	enc = new Encoders(ENCODER_L_TIMx, ENCODER_R_TIMx);
	em = new EncoderMeasure(enc);
	mpu = new MPU6500(MPU6500_MOSI_PIN, MPU6500_MISO_PIN, MPU6500_SCLK_PIN,
	MPU6500_SSEL_PIN);
	gm = new GyroMeasure(mpu, 0);
	rfl = new Reflector(IR_LED_SL_FR_PIN, IR_LED_SR_FL_PIN);
	wd = new WallDetector(rfl);

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
	ctrlTicker.attach_us(ctrl_machine, 1000);

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
			driveTicker.attach_us(motor_drive, 1000);
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
