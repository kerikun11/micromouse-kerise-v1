/*
 * MoveAction.h
 *
 *  Created on: 2016/11/11
 *      Author: kerikun11
 */

#ifndef MOVEACTION_H_
#define MOVEACTION_H_

#include "mbed.h"
#include "config.h"

class MoveAction {
public:
	MoveAction(Buzzer *bz, Encoders *enc, MPU6500 *mpu, Reflector *rfl, WallDetector *wd,
			SpeedController *sc) :
			bz(bz), enc(enc), mpu(mpu), rfl(rfl), wd(wd), sc(sc),
					thread(PRIORITY_MOVE_ACTION, STACK_SIZE_MOVE_ACTION) {
		_actions = 0;
		set_params(1000, 4000);
	}
	enum ACTION {
		START_STEP,
		START_INIT,
		GO_STRAIGHT,
		TURN_LEFT_90,
		TURN_RIGHT_90,
		RETURN,
		FAST_START_STEP,
		FAST_GO_STRAIGHT,
		FAST_GO_DIAGONAL,
		FAST_GO_HALF,
		FAST_TURN_LEFT_45,
		FAST_TURN_LEFT_90,
		FAST_TURN_RIGHT_45,
		FAST_TURN_RIGHT_90,
		FAST_STOP,
	};
	const char* action_string(enum ACTION action) {
		static const char name[][32] = { "start_step", "start_init", "go_straight", "turn_left_90",
				"turn_right_90", "return", "fast_start_step", "fast_go_straight",
				"fast_go_diagonal", "fast_go_half", "fast_turn_left_45", "fast_turn_left_90",
				"fast_turn_right_45", "fast_turn_right_90", "fast_stop" };
		return name[action];
	}
	void enable() {
		error.reset();
		sc->position = error;
		rfl->enable();
		sc->enable();
		thread.start(this, &MoveAction::task);
		printf("0x%08X: Move Action\n", (unsigned int) thread.gettid());
	}
	void disable() {
		thread.terminate();
		sc->disable();
		rfl->disable();
		while (1) {
			osEvent evt = queue.get(1);
			if (evt.status != osEventMessage) {
				break;
			}
		}
		_actions = 0;
	}
	void set_action(enum ACTION action) {
		_actions++;
		queue.put((enum ACTION*) action);
	}
	void set_params(float fast_speed, float fast_accel) {
		this->fast_speed = fast_speed;
//		this->fast_accel = fast_accel;
		this->fast_accel = 6000;
	}
	void set_params(float add) {
		this->fast_speed += add;
//		this->fast_accel += add * 2;
		this->fast_accel = 6000;
	}
	int actions() const {
		return _actions;
	}
private:
	Buzzer *bz;
	Encoders *enc;
	MPU6500 *mpu;
	Reflector *rfl;
	WallDetector *wd;
	SpeedController *sc;
	Thread thread;
	Queue<enum ACTION, 128> queue;
	Timer timer;
	int _actions;
	struct WallDetector::WALL start_wall;
	Position error;
	float fast_speed;
	float fast_accel;

	void wall_avoid() {
//		if (wd->wall().side[0]) {
//			sc->position.y -= wd->wall_difference().side[0] * 0.01;
//		}
//		if (wd->wall().side[1]) {
//			sc->position.y += wd->wall_difference().side[1] * 0.01;
//		}
	}
	float fix_y() {
		float diff = -sc->position.y * 100;
		const float saturation = 0.15f;
		if (diff > saturation) {
			return saturation;
		} else if (diff < -saturation) {
			return -saturation;
		} else {
			return diff;
		}
	}
	void wall_attach() {
		//		printf("Position:\t(%05.1f, %05.1f, %05.2f)\n", sc->position.x, sc->position.y,
		//				sc->position.theta);
		if (wd->wall().flont[0] && wd->wall().flont[1]) {
			while (1) {
				float trans = wd->wall_difference().flont[0] + wd->wall_difference().flont[1];
				float rot = wd->wall_difference().flont[1] - wd->wall_difference().flont[0];
				const float trans_saturation = 0.4f;
				const float rot_saturation = 0.4f;
				if (trans > trans_saturation) trans = trans_saturation;
				if (trans < -trans_saturation) trans = -trans_saturation;
				if (rot > rot_saturation) rot = rot_saturation;
				if (rot < -rot_saturation) rot = -rot_saturation;
				sc->set_target(trans * 500, rot * 10);
				if (fabs(trans) < 0.1 && fabs(rot) < 0.1f) break;
				Thread::wait(1);
			}
			sc->set_target(0, 0);
			error = sc->position;
			error.x = 0;
//			error.theta = 0;
			sc->position = error;
//			printf("Wall Attach:\t(%05.1f, %05.1f, %04.2f)\n", error.x, error.y, error.theta);
		}
	}
	void acceleration(float speed, float target_distance, float accel = 6000) {
		timer.reset();
		timer.start();
		float v0 = sc->actual().trans;
		while (1) {
			wall_avoid();
			float trans = v0 + timer.read() * accel;
			sc->set_target(trans, fix_y());
			Thread::wait(1);
			if (sc->actual().trans > speed) break;
			if (sc->position.x > target_distance) break;
		}
		while (1) {
			wall_avoid();
			sc->set_target(speed, fix_y());
			Thread::wait(1);
			if (sc->position.x > target_distance) break;
		}
		error = sc->position - Position(target_distance, 0, 0);
		sc->position = error;
//		printf("Accel %04.0f:\t(%05.1f, %05.1f, %04.2f)\n", target_distance, error.x, error.y,
//				error.theta);
	}
	void deceleration(float speed, float target_distance, float accel = 3000) {
		while (1) {
			wall_avoid();
			float extra = target_distance - sc->position.x;
			float target_speed = sqrt(2 * accel * fabs(extra));
			target_speed = (target_speed > speed) ? speed : target_speed;
			if (extra > 0) {
				sc->set_target(target_speed, fix_y());
			} else {
				sc->set_target(-target_speed, fix_y());
			}
			Thread::wait(1);
			if (fabs(sc->actual().trans) < 1) break;
		}
		sc->set_target(0, 0);
		error = sc->position - Position(target_distance, 0, 0);
		sc->position = error;
//		printf("Decel %04.0f:\t(%05.1f, %05.1f, %04.2f)\n", target_distance, error.x, error.y,
//				error.theta);
	}
	void straight(float speed, float target_distance) {
		acceleration(speed, target_distance / 2);
		deceleration(speed, target_distance / 2);
	}
	void turn(float speed, float target_angle, float accel = 8 * M_PI) {
		timer.reset();
		timer.start();
		while (1) {
			if (target_angle > 0) {
				sc->set_target(0, timer.read() * accel);
			} else {
				sc->set_target(0, -timer.read() * accel);
			}
			Thread::wait(1);
			if (fabs(sc->actual().rot) > speed) break;
		}
		while (1) {
			float extra = target_angle - sc->position.theta;
			float target_speed = sqrt(2 * accel * fabs(extra));
			target_speed = (target_speed > speed) ? speed : target_speed;
			if (extra > 0) {
				sc->set_target(0, target_speed);
			} else {
				sc->set_target(0, -target_speed);
			}
			Thread::wait(1);
			if (fabs(sc->actual().rot) < 0.5) break;
		}
		error = sc->position.rotate(-target_angle);
		sc->position = error;
//		printf("Turn %04.2f:\t(%05.1f, %05.1f, %05.2f)\n", target_angle, error.x, error.y,
//				error.theta);
	}
	void task() {
		while (1) {
			osEvent evt = queue.get();
			if (evt.status != osEventMessage) {
				printf("Error!\n");
				continue;
			}
			enum ACTION action = (enum ACTION) evt.value.v;
			printf("Action: %s\n", action_string(action));
			start_wall = wd->wall();
			const float rot_speed = 3.0f * M_PI;
			const float rot_accel = 24.0f * M_PI;
			const float rot_speed_fast = 4.0f * M_PI;
			const float rot_accel_fast = 32.0f * M_PI;
			const float trans_speed = 700;
			switch (action) {
				case START_STEP:
					straight(200, 90 - 24 - 6);
					sc->set_target(0, 0);
					break;
				case START_INIT:
					turn(rot_speed, M_PI / 2, rot_accel);
					wall_attach();
					turn(rot_speed, M_PI / 2, rot_accel);
					sc->position.reset();
					sc->set_target(-10, 0);
					Thread::wait(100);
					sc->set_target(-200, 0);
					Thread::wait(1000);
					sc->position.reset();
					sc->set_target(0, 0);
					break;
				case GO_STRAIGHT:
					straight(trans_speed, 180);
					wall_attach();
					sc->set_target(0, 0);
					break;
				case TURN_LEFT_90:
					turn(rot_speed, M_PI / 2, rot_accel);
					straight(trans_speed, 180);
					wall_attach();
					sc->set_target(0, 0);
					break;
				case TURN_RIGHT_90:
					turn(rot_speed, -M_PI / 2, rot_accel);
					straight(trans_speed, 180);
					wall_attach();
					sc->set_target(0, 0);
					break;
				case RETURN:
					turn(rot_speed, M_PI / 2, rot_accel);
					wall_attach();
					turn(rot_speed, M_PI / 2, rot_accel);
//					straight(trans_speed, 180);
//					wall_attach();
					sc->set_target(0, 0);
					break;
				case FAST_START_STEP:
					acceleration(fast_speed, 180 - 24 - 6, fast_accel);
					break;
				case FAST_GO_STRAIGHT:
					acceleration(fast_speed, 180, fast_accel);
					break;
				case FAST_GO_DIAGONAL:
					acceleration(fast_speed, 90 * 1.4142, fast_accel);
					break;
				case FAST_GO_HALF:
					acceleration(fast_speed, 90, fast_accel);
					break;
				case FAST_TURN_LEFT_45:
					break;
				case FAST_TURN_LEFT_90:
					deceleration(fast_speed, 90, fast_accel);
					wall_attach();
					turn(rot_speed_fast, M_PI / 2, rot_accel_fast);
					acceleration(fast_speed, 90, fast_accel);
					break;
				case FAST_TURN_RIGHT_45:
					break;
				case FAST_TURN_RIGHT_90:
					deceleration(fast_speed, 90, fast_accel);
					wall_attach();
					turn(rot_speed_fast, -M_PI / 2, rot_accel_fast);
					acceleration(fast_speed, 90, fast_accel);
					break;
				case FAST_STOP:
					deceleration(fast_speed, 90, fast_accel);
					wall_attach();
					sc->set_target(0, 0);
					break;
			}
			_actions--;
		}
	}
};

#endif /* MOVEACTION_H_ */
