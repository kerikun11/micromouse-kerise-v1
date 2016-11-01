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
EncoderMeasure *em;

MPU6500 *mpu;
GyroMeasure *gm;

Reflector *rfl;
WallDetector *wd;

StraightControl *sc;
MoveAction *ma;

void debug_info() {
	while (1) {
		Thread::wait(100);
//		printf("Gyr: %.2lf\tAng: %.2lf\tInt: %.2lf\tTar: %.2lf\t", gm->gyro(),
//				gm->angle(), gm->int_angle(), gm->target());
//		printf("Vel: %.2lf\tPos: %.2lf\tInt: %.2lf\tTar: %.2lf\t\n",
//				em->dif_position(), em->position(), em->int_position(),
//				em->target());

		printf("%05u\t%05u\t%05u\t%05u\n", rfl->sl(), rfl->fl(), rfl->fr(),
				rfl->sr());

		printf("%s %s %s %s\n", wd->wall().side[0] ? "X" : ".",
				wd->wall().flont[0] ? "X" : ".",
				wd->wall().flont[1] ? "X" : ".",
				wd->wall().side[1] ? "X" : ".");

//		printf("L: %ld\tR: %ld\n", enc->left(), enc->right());
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
			bz->play(Buzzer::BUZZER_MUSIC_CONFIRM);
			sc->enable();
			break;
		case 'f':
			bz->play(Buzzer::BUZZER_MUSIC_SELECT);
			sc->disable();
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
		case 'w':
			ma->set_action(MoveAction::ACTION_GO_STRAIGHT);
			break;
		case 's':
			ma->set_action(MoveAction::ACTION_GO_HALF);
			break;
		case 'x':
			ma->set_action(MoveAction::ACTION_GO_DIAGONAL);
			break;
		case 'a':
			ma->set_action(MoveAction::ACTION_TURN_LEFT_90);
			break;
		case 'd':
			ma->set_action(MoveAction::ACTION_TURN_RIGHT_90);
			break;
		case 'q':
			ma->set_action(MoveAction::ACTION_TURN_LEFT_45);
			break;
		case 'e':
			ma->set_action(MoveAction::ACTION_TURN_RIGHT_45);
			break;
		case 'm':
			ma->set_action(MoveAction::ACTION_GO_STRAIGHT);
			ma->set_action(MoveAction::ACTION_TURN_RIGHT_90);
			ma->set_action(MoveAction::ACTION_GO_STRAIGHT);
			ma->set_action(MoveAction::ACTION_TURN_RIGHT_90);
			ma->set_action(MoveAction::ACTION_GO_STRAIGHT);
			ma->set_action(MoveAction::ACTION_TURN_RIGHT_90);
			ma->set_action(MoveAction::ACTION_TURN_RIGHT_90);
			ma->set_action(MoveAction::ACTION_GO_STRAIGHT);
			ma->set_action(MoveAction::ACTION_TURN_LEFT_90);
			ma->set_action(MoveAction::ACTION_GO_STRAIGHT);
			ma->set_action(MoveAction::ACTION_TURN_LEFT_90);
			ma->set_action(MoveAction::ACTION_GO_STRAIGHT);
			ma->set_action(MoveAction::ACTION_TURN_LEFT_90);
			ma->set_action(MoveAction::ACTION_TURN_LEFT_90);
			break;
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
	em = new EncoderMeasure(enc);

	mpu = new MPU6500(MPU6500_MOSI_PIN, MPU6500_MISO_PIN,
	MPU6500_SCLK_PIN,
	MPU6500_SSEL_PIN);
	gm = new GyroMeasure(mpu, 0);

	rfl = new Reflector(IR_LED_SL_FR_PIN, IR_LED_SR_FL_PIN);
	wd = new WallDetector(rfl);

	sc = new StraightControl(mt, enc, em, mpu, gm, rfl, wd);
	ma = new MoveAction(mt, enc, em, mpu, gm, rfl, wd, sc);

	/* boot */
	{
		printf("\nHello World!\n");
		float voltage = bat->voltage();
		printf("Battery Voltage: %.3f [V]\n", voltage);
		if (!bat->check()) {
			bz->play(Buzzer::BUZZER_MUSIC_LOW_BATTERY);
			printf("Battery Low!\n");
			while (1) {
				wait(1);
			}
		}
		bz->play(Buzzer::BUZZER_MUSIC_BOOT);
		*led = 0x6;
		wait(0.2);
		*led = 0x9;
	}

	/* for debug */
	Thread debugInfoThread;
	debugInfoThread.start(debug_info);
	Thread serialCtrlThread;
	serialCtrlThread.start(serial_ctrl);

	while (true) {
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
		if (mpu->accelY() < -2000000) {
			mt->emergency_stop();
			bz->play(Buzzer::BUZZER_MUSIC_EMERGENCY);
			while (1) {
				Thread::wait(1);
			}
		}
	}
}
