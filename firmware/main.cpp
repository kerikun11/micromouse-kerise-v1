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

/* define each pointer */
BusOut *led;
Battery *bat;
Buzzer *bz;
Button *btn;

Motor *mt;

Encoders *enc;
MPU6500 *mpu;

Reflector *rfl;
WallDetector *wd;

SpeedController *sc;
MoveAction *ma;

void debug_info() {
	while (1) {
		Thread::wait(100);

		printf("%05u\t%05u\t%05u\t%05u\t", rfl->sl(), rfl->fl(), rfl->fr(),
				rfl->sr());
		printf("%s %s "
				"%s %s "
				"%s %s\n", wd->wall().side[0] ? "X" : ".",
				wd->wall().flont[0] ? "X" : ".",
				wd->wall().flont_flont[0] ? "X" : ".",
				wd->wall().flont_flont[1] ? "X" : ".",
				wd->wall().flont[1] ? "X" : ".",
				wd->wall().side[1] ? "X" : ".");

//		printf("Angle: %05d\n", (int) mpu->angleZ());

//		printf("L: %ld\tR: %ld\n", enc->left(), enc->right());

//		printf("Acc Y: %lf\n", mpu->accelY());

//		for (int i = 0; i < 2; i++) {
//			printf("P:%05d\tI:%05d\tD:%05d\t", (int) sc->wheel_p[i],
//					(int) sc->wheel_i[i], (int) sc->wheel_d[i]);
//		}
//		printf("L:%05d\tR:%05d\t", (int) sc->target.wheel[0],
//				(int) sc->target.wheel[1]);
//		printf("\n");
	}
}

void serial_ctrl() {
	while (1) {
		Thread::wait(100);
		int c = getchar();
		if (c == EOF)
			continue;
		printf("%c\n", (char) c);
		switch (c) {
		case 'g':
			bz->play(Buzzer::BUZZER_MUSIC_CONFIRM);
			sc->enable();
			break;
		case 'f':
			bz->play(Buzzer::BUZZER_MUSIC_CONFIRM);
			sc->disable();
			mt->free();
			break;
		case 'h':
			sc->set_target(360, 720 / 90);
			break;
		case 'j':
			sc->set_target(-360, 0);
			break;
		case 'k':
			sc->set_target(360, 0);
			break;
		case 'l':
			sc->set_target(360, -720 / 90);
			break;
		case 'b':
			sc->set_target(0, 0);
			break;
		case 'w':
			ma->set_action(MoveAction::GO_STRAIGHT);
			break;
		case 'q':
			ma->set_action(MoveAction::CURVE_LEFT_90);
			break;
		case 'e':
			ma->set_action(MoveAction::CURVE_RIGHT_90);
			break;
		case 's':
			ma->set_action(MoveAction::GO_HALF);
			break;
		case 'a':
			ma->set_action(MoveAction::TURN_LEFT_90);
			break;
		case 'd':
			ma->set_action(MoveAction::TURN_RIGHT_90);
			break;
		case 'r':
			ma->set_action(MoveAction::RETURN);
			break;
		}
	}
}

void emergencyTask() {
	while (1) {
		Thread::wait(1);
		if (mpu->accelY() < -2000000) {
			mt->emergency_stop();
			bz->play(Buzzer::BUZZER_MUSIC_EMERGENCY);
			while (1) {
				Thread::wait(1000);
			}
		}
	}
}

int main() {
	/* make instances */
	led = new BusOut(LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN);
	bat = new Battery(BATTERY_PIN);
	bz = new Buzzer(BUZZER_PIN);
	btn = new Button(BUTTON_PIN);

	mt = new Motor;

	enc = new Encoders(ENCODER_L_TIMx, ENCODER_R_TIMx);
	mpu = new MPU6500(MPU6500_MOSI_PIN, MPU6500_MISO_PIN,
	MPU6500_SCLK_PIN,
	MPU6500_SSEL_PIN);

	rfl = new Reflector(IR_LED_SL_FR_PIN, IR_LED_SR_FL_PIN);
	wd = new WallDetector(rfl);

	sc = new SpeedController(mt, enc);
	ma = new MoveAction(bz, enc, mpu, rfl, wd, sc);

	/* boot */
	{
		printf("\nHello World!\n");
		float voltage = bat->voltage();
		printf("Battery Voltage: %.3f [V]\n", voltage);
		if (!bat->check()) {
			bz->play(Buzzer::BUZZER_MUSIC_LOW_BATTERY);
			printf("Battery Low!\n");
			while (1) {
				Thread::wait(1000);
			}
		}
		bz->play(Buzzer::BUZZER_MUSIC_BOOT);
		*led = 0x6;
		wait(0.2);
		*led = 0x9;
	}

	/* for debug */
	Thread debugInfoThread(osPriorityBelowNormal);
	debugInfoThread.start(debug_info);
	Thread serialCtrlThread(osPriorityLow);
	serialCtrlThread.start(serial_ctrl);
	Thread emergencyThread(osPriorityAboveNormal);
	emergencyThread.start(emergencyTask);

	while (true) {
		Thread::wait(100);
		if (btn->pressed) {
			btn->flags = 0;
			bz->play(Buzzer::BUZZER_MUSIC_SELECT);
			sc->disable();
			mt->free();
		}
		if (btn->long_pressed_1) {
			btn->flags = 0;
			bz->play(Buzzer::BUZZER_MUSIC_CONFIRM);
		}
	}
}
