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
#include "MazeSolver.h"

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
MazeSolver *ms;

void debug_info() {
	while (1) {
		Thread::wait(100);

//		printf("%05u\t%05u\t%05u\t%05u\t", rfl->sl(), rfl->fl(), rfl->fr(), rfl->sr());
//		printf("%s %s %s %s\n", wd->wall().side[0] ? "X" : ".", wd->wall().flont[0] ? "X" : ".",
//				wd->wall().flont[1] ? "X" : ".", wd->wall().side[1] ? "X" : ".");

//		printf("x: %07.3f\ty: %07.3f\ttheta: %07.3f\ttrans: %07.3f\tomega: %07.3f\n",
//				sc->position.x, sc->position.y, sc->position.theta / M_PI * 180, sc->actual().trans,
//				sc->actual().rot);

//		printf("trans: %07.3f\n", sc->actual().trans);

//		printf("Gyro: %9.3f\tAngle: %09.3f\n", mpu->gyroZ(), mpu->angleZ());

//		printf("L: %ld\tR: %ld\n", enc->left(), enc->right());

//		printf("Acc Y: %lf\n", mpu->accelY());

//		for (int i = 0; i < 2; i++) {
//			printf("P:%09.2f\tI:%09.2f\tD:%09.2f\t", sc->actual_p.wheel[i],
//					sc->actual_i.wheel[i], sc->actual_d.wheel[i]);
//		}
//		printf("L:%09.2f\tR:%09.2f\t", sc->target_p.wheel[0],
//				sc->target_p.wheel[1]);
//		printf("\n");
	}
}

void serial_ctrl() {
	while (1) {
		Thread::wait(100);
		int c = getchar();
		if (c == EOF) continue;
		printf("%c\n", (char) c);
		switch (c) {
			case 'g':
				bz->play(Buzzer::BUZZER_MUSIC_CONFIRM);
				sc->enable();
				ma->enable();
				break;
			case 'f':
				bz->play(Buzzer::BUZZER_MUSIC_CANCEL);
				ma->disable();
				sc->disable();
				mt->free();
				break;
			case 'w':
				ma->set_action(MoveAction::GO_STRAIGHT);
				break;
			case 'q':
				ma->set_action(MoveAction::TURN_LEFT_90);
				break;
			case 'e':
				ma->set_action(MoveAction::TURN_RIGHT_90);
				break;
			case 'r':
				ma->set_action(MoveAction::RETURN);
				break;
			case 'i':
				ma->set_action(MoveAction::START_INIT);
				break;
			case 's':
				ma->set_action(MoveAction::START_STEP);
				break;
			case 'm':
				bz->play(Buzzer::BUZZER_MUSIC_CONFIRM);
				mpu->calibration();
				ms->start();
				break;
			case 'u':
				mpu->calibration();
				break;
			case 'p':
				printf("%05u\t%05u\t%05u\t%05u\t", rfl->sl(), rfl->fl(), rfl->fr(), rfl->sr());
				printf("%s %s "
						"%s %s\n", wd->wall().side[0] ? "X" : ".", wd->wall().flont[0] ? "X" : ".",
						wd->wall().flont[1] ? "X" : ".", wd->wall().side[1] ? "X" : ".");
				printf("x: %07.3f\ty: %07.3f\ttheta: %07.3f\ttrans: %07.3f\tomega: %07.3f\n",
						sc->position.x, sc->position.y, sc->position.theta / M_PI * 180,
						sc->actual().trans, sc->actual().rot);
				printf("Gyro: %7.4f\tAngle: %07.4f\n", mpu->gyroZ(), mpu->angleZ());
				printf("L: %ld\tR: %ld\n", enc->left(), enc->right());
				break;
			case 'z':
				ma->set_action(MoveAction::FAST_START_STEP);
				break;
			case 'x':
				ma->set_action(MoveAction::FAST_TURN_LEFT_90);
				break;
			case 'c':
				ma->set_action(MoveAction::FAST_GO_STRAIGHT);
				break;
			case 'v':
				ma->set_action(MoveAction::FAST_TURN_RIGHT_90);
				break;
			case 'b':
				ma->set_action(MoveAction::FAST_STOP);
				break;
		}
	}
}

void emergencyTask() {
	while (1) {
		Thread::wait(1);
		if (mpu->accelY() < -2) {
			mt->emergency_stop();
			ma->disable();
			bz->play(Buzzer::BUZZER_MUSIC_EMERGENCY);
//			bz->play(Buzzer::BUZZER_MUSIC_FROG);
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

	sc = new SpeedController(mt, enc, mpu);
	ma = new MoveAction(bz, enc, mpu, rfl, wd, sc);
	ms = new MazeSolver(bz, ma, wd);

	/* boot */
	{
		printf("\nHello World!\n");
		if (!bat->check()) {
			bz->play(Buzzer::BUZZER_MUSIC_LOW_BATTERY);
			printf("Battery Low!\n");
			while (1) {
				Thread::wait(1000);
			}
		}
		bz->play(Buzzer::BUZZER_MUSIC_BOOT);
		*led = bat->gage(16);
	}

	/* for debug */
	Thread debugInfoThread(PRIORITY_DEBUG_INFO);
	debugInfoThread.start(debug_info);
	Thread serialCtrlThread(PRIORITY_SERIAL_CTRL);
	serialCtrlThread.start(serial_ctrl);
	Thread emergencyThread(PRIORITY_EMERGENCY_STOP);
	emergencyThread.start(emergencyTask);

	while (true) {
		Thread::wait(100);
		if (btn->pressed) {
			btn->flags = 0;
			bz->play(Buzzer::BUZZER_MUSIC_CANCEL);
			sc->disable();
			mt->free();
		}
		if (btn->long_pressed_1) {
			btn->flags = 0;
			bz->play(Buzzer::BUZZER_MUSIC_CONFIRM);
			Thread::wait(2000);
			rfl->enable();
			while (rfl->side(1) < 1024) {
				Thread::wait(100);
			}
			bz->play(Buzzer::BUZZER_MUSIC_CONFIRM);
			mpu->calibration();
			ms->start();
		}
	}
}
