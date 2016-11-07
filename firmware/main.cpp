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

		printf("%05u\t%05u\t%05u\t%05u\t", rfl->sl(), rfl->fl(), rfl->fr(), rfl->sr());
		printf("%s %s "
				"%s %s "
				"%s %s\n", wd->wall().side[0] ? "X" : ".", wd->wall().flont[0] ? "X" : ".",
				wd->wall().flont_flont[0] ? "X" : ".", wd->wall().flont_flont[1] ? "X" : ".",
				wd->wall().flont[1] ? "X" : ".", wd->wall().side[1] ? "X" : ".");

//		printf("x: %07.3f\ty: %07.3f\ttheta: %07.3f\ttrans: %07.3f\tomega: %07.3f\n",
//				sc->position.x, sc->position.y, sc->position.theta / M_PI * 180, sc->actual().trans,
//				sc->actual().rot);

//		printf("trans: %07.3f\n", sc->actual().trans);

//		printf("Angle: %09.3f\n", mpu->angleZ());

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
				bz->play(Buzzer::BUZZER_MUSIC_CONFIRM);
				ma->disable();
				sc->disable();
				mt->free();
				break;
			case 'b':
				sc->set_target(0, 0);
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
				ma->set_action(MoveAction::START_RETURN);
				break;
			case 's':
				ma->set_action(MoveAction::START_STEP);
				break;
			case 'z':
				sc->set_target(500, 0);
				break;
			case 'x':
				sc->set_target(-500, 0);
				break;
			case 'c':
				sc->set_target(0, 10);
				break;
		}
	}
}

void emergencyTask() {
	while (1) {
		Thread::wait(1);
		if (mpu->accelY() < -2500000) {
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

	sc = new SpeedController(mt, enc, mpu);
	ma = new MoveAction(bz, enc, mpu, rfl, wd, sc);
	ms = new MazeSolver(ma);

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
