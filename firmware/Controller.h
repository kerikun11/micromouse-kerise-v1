/*
 * Controller.h
 *
 *  Created on: 2016/10/28
 *      Author: kerikun11
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "mbed.h"
#include "config.h"

#define SPEED_CONTROLLER_PERIOD_US	500

class SpeedController {
public:
	SpeedController(Motor *mt, Encoders *enc, MPU6500 *mpu) :
			mt(mt), enc(enc), mpu(mpu), ctrlThread(PRIORITY_SPEED_CONTROLLER) {
		ctrlThread.start(this, &SpeedController::ctrlTask);
		for (int i = 0; i < 2; i++) {
			target_p.wheel[i] = 0;
			for (int j = 0; j < 3; j++) {
				wheel_position[j][i] = 0;
			}
			actual_p.wheel[i] = 0;
			actual_i.wheel[i] = 0;
			actual_d.wheel[i] = 0;
		}
	}
	void enable() {
		ctrlTicker.attach_us(this, &SpeedController::ctrlIsr,
		SPEED_CONTROLLER_PERIOD_US);
	}
	void disable() {
		ctrlTicker.detach();
	}
	void set_target(float trans, float rot) {
		target_p.trans = trans;
		target_p.rot = rot;
		pole2wheel(&target_p);
	}
	typedef struct {
		float trans;	//< translation
		float rot;		//< rotation
		float wheel[2];	//< wheel [0]: left, [1]: right
	} params_t;
	params_t target() {
		return target_p;
	}
	params_t actual() {
		return actual_p;
	}
	typedef struct {
		float x = 0;
		float y = 0;
		float theta = 0;
	} position_t;
	position_t position;
	params_t target_p;
	params_t actual_p;
	params_t actual_i;
	params_t actual_d;
private:
	Motor *mt;
	Encoders *enc;
	MPU6500 *mpu;
	Thread ctrlThread;
	Ticker ctrlTicker;
	float wheel_position[3][2];

	void ctrlIsr() {
		ctrlThread.signal_set(0x01);
	}
	void ctrlTask() {
		while (1) {
			Thread::signal_wait(0x01);
			for (int i = 0; i < 2; i++) {
				wheel_position[2][i] = wheel_position[1][i];
				wheel_position[1][i] = wheel_position[0][i];
				wheel_position[0][i] = enc->position(i);
			}
			for (int i = 0; i < 2; i++) {
				actual_p.wheel[i] = (wheel_position[0][i] - wheel_position[1][i])
						* 1000000/ SPEED_CONTROLLER_PERIOD_US;
				actual_i.wheel[i] += (actual_p.wheel[i] - target_p.wheel[i])
						* SPEED_CONTROLLER_PERIOD_US / 1000000;
				if (actual_i.wheel[i] > 10) actual_i.wheel[i] = 10;
				if (actual_i.wheel[i] < -10) actual_i.wheel[i] = -10;
				actual_d.wheel[i] = (wheel_position[0][i] - 2 * wheel_position[1][i]
						+ wheel_position[2][i]) * 1000000 / SPEED_CONTROLLER_PERIOD_US;
			}
			wheel2pole(&actual_p);
			actual_p.rot = mpu->gyroZ() * M_PI / 180.0f;
			pole2wheel(&actual_p);
			const float Kp = 2;
			const float Ki = 0.5;
			const float Kd = 0.5;
			float pwm_value[2];
			for (int i = 0; i < 2; i++) {
				pwm_value[i] = Kp * (target_p.wheel[i] - actual_p.wheel[i])
						+ Ki * (0 - actual_i.wheel[i]) + Kd * (0 - actual_d.wheel[i]);
			}
			mt->drive(pwm_value[0], pwm_value[1]);

			position.theta += actual_p.rot * SPEED_CONTROLLER_PERIOD_US / 1000000;
			position.x += actual_p.trans * cos(position.theta) * SPEED_CONTROLLER_PERIOD_US
					/ 1000000;
			position.y += actual_p.trans * sin(position.theta) * SPEED_CONTROLLER_PERIOD_US
					/ 1000000;
		}
	}
	void pole2wheel(params_t *params) {
		params->wheel[0] = params->trans - MACHINE_ROTATION_RADIUS * params->rot;
		params->wheel[1] = params->trans + MACHINE_ROTATION_RADIUS * params->rot;
	}
	void wheel2pole(params_t *params) {
		params->rot = (params->wheel[1] - params->wheel[0]) / 2.0f / MACHINE_ROTATION_RADIUS;
		params->trans = (params->wheel[1] + params->wheel[0]) / 2.0f;
	}
};

class MoveAction {
public:
	MoveAction(Buzzer *bz, Encoders *enc, MPU6500 *mpu, Reflector *rfl, WallDetector *wd,
			SpeedController *sc) :
			bz(bz), enc(enc), mpu(mpu), rfl(rfl), wd(wd), sc(sc), thread(PRIORITY_MOVE_ACTION) {
		_actions = 0;
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
		FAST_TURN_LEFT_45,
		FAST_TURN_LEFT_90,
		FAST_TURN_RIGHT_45,
		FAST_TURN_RIGHT_90,
		FAST_STOP,
	};
	const char* action_string(enum ACTION action) {
		static const char name[][32] = { "start_step", "start_init", "go_straight", "turn_left_90",
				"turn_right_90", "return", "fast_start_step", "fast_go_straight",
				"fast_go_diagonal", "fast_turn_left_45", "fast_turn_left_90", "fast_turn_right_45",
				"fast_turn_right_90", "fast_stop" };
		return name[action];
	}
	void enable() {
		rfl->enable();
		sc->enable();
		thread.start(this, &MoveAction::task);
	}
	void disable() {
		thread.terminate();
		sc->disable();
		rfl->disable();
		while (1) {
			osEvent evt = queue.get(0);
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

	float wall_avoid(bool side, bool flont) {
		float wall = 0;
		if (side) {
			if (wd->wall().side[0]) {
				wall += wd->wall_difference().side[0] * 0.8;
			}
			if (wd->wall().side[1]) {
				wall -= wd->wall_difference().side[1] * 0.8;
			}
		}
		if (flont) {
			if (wd->wall().flont[0] && wd->wall().flont[1]) {
//				wall += wd->wall_difference().flont[0] * 2;
//				wall -= wd->wall_difference().flont[1] * 2;
			}
		}
		return wall;
	}
	void wall_attach() {
		while (1) {
			if (wd->wall().flont[0] && wd->wall().flont[1]) {
				float trans = wd->wall_difference().flont[0] + wd->wall_difference().flont[1];
				float rot = wd->wall_difference().flont[1] - wd->wall_difference().flont[0];
				sc->set_target(trans * 1000, rot * 10);
				if (abs(trans) < 0.1 && abs(rot) < 0.1) break;
				Thread::wait(1);
			} else {
				break;
			}
		}
	}
	void acceleration(float speed, float target_position, float accel = 2000, float wall_gain = 1) {
		timer.reset();
		timer.start();
		float v0 = sc->actual().trans;
		while (1) {
			float wall = wall_avoid(true, false);
			sc->set_target(v0 + timer.read() * accel, wall * wall_gain);
			Thread::wait(1);
			if (sc->actual().trans > speed) break;
			if (sc->position.x > target_position) break;
		}
		while (1) {
			float wall = wall_avoid(true, false);
			sc->set_target(speed, wall * wall_gain);
			Thread::wait(1);
			if (sc->position.x > target_position) break;
		}
	}
	void deceleration(float speed, float target_position, float accel = 2000, float wall_gain = 1) {
		while (1) {
			float wall = wall_avoid(true, true);
			float extra = target_position - sc->position.x;
			float target_speed = sqrt(2 * accel * abs(extra));
			target_speed = (target_speed > speed) ? speed : target_speed;
			if (extra > 0) {
				sc->set_target(target_speed, wall * wall_gain);
			} else {
				sc->set_target(-target_speed, wall * wall_gain);
			}
			Thread::wait(1);
			if (abs(sc->actual().trans) < 1) break;
		}
	}
	void straight(float speed, float target_position) {
		sc->position.x = 0;
		sc->position.y = 0;
		sc->position.theta = 0;
		acceleration(speed, target_position / 2);
		deceleration(speed, target_position);
	}
	void turn(float speed, float target_angle, float accel = 8 * M_PI) {
		sc->position.x = 0;
		sc->position.y = 0;
		sc->position.theta = 0;
		timer.reset();
		timer.start();
		while (1) {
			if (target_angle > 0) {
				sc->set_target(0, timer.read() * accel);
			} else {
				sc->set_target(0, -timer.read() * accel);
			}
			Thread::wait(1);
			if (abs(sc->actual().rot) > speed) break;
		}
		while (1) {
			float extra = target_angle - sc->position.theta;
			float target_speed = sqrt(2 * accel * abs(extra));
			target_speed = (target_speed > speed) ? speed : target_speed;
			if (extra > 0) {
				sc->set_target(0, target_speed);
			} else {
				sc->set_target(0, -target_speed);
			}
			Thread::wait(1);
			if (abs(sc->actual().rot) < 0.1) break;
		}
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
			sc->position.x = 0;
			sc->position.y = 0;
			sc->position.theta = 0;
			const float fast_accel = 3000;
			const float fast_speed = 1000;
			switch (action) {
				case START_STEP:
					straight(200, 90 - 24);
					sc->set_target(0, 0);
					break;
				case START_INIT:
					straight(200, 30);
					turn(1.2 * M_PI, M_PI);
					sc->position.x = 0;
					sc->position.y = 0;
					sc->position.theta = 0;
					while (1) {
						sc->set_target(-100, 0);
						if (sc->position.x < 24 - 60) break;
					}
					sc->set_target(0, 0);
					break;
				case GO_STRAIGHT:
					straight(500, 180);
					wall_attach();
					sc->set_target(0, 0);
					break;
				case TURN_LEFT_90:
					turn(1.2 * M_PI, M_PI / 2);
					straight(500, 180);
					wall_attach();
					sc->set_target(0, 0);
					break;
				case TURN_RIGHT_90:
					turn(1.2 * M_PI, -M_PI / 2);
					straight(500, 180);
					wall_attach();
					sc->set_target(0, 0);
					break;
				case RETURN:
					turn(1.2 * M_PI, M_PI);
					straight(500, 180);
					wall_attach();
					sc->set_target(0, 0);
					break;
				case FAST_START_STEP:
					acceleration(fast_speed, 180 - 24, fast_accel);
					break;
				case FAST_GO_STRAIGHT:
					acceleration(fast_speed, 180, fast_accel);
					break;
				case FAST_GO_DIAGONAL:
					acceleration(fast_speed, 180 * 1.4142, fast_accel);
					break;
				case FAST_TURN_LEFT_45:
					break;
				case FAST_TURN_LEFT_90:
					deceleration(fast_speed, 90, fast_accel);
					wall_attach();
					sc->position.x = 0;
					sc->position.y = 0;
					sc->position.theta = 0;
					turn(3 * M_PI, M_PI / 2, 16 * M_PI);
					sc->position.x = 0;
					sc->position.y = 0;
					sc->position.theta = 0;
					acceleration(fast_speed, 90, fast_accel);
					break;
				case FAST_TURN_RIGHT_45:
					break;
				case FAST_TURN_RIGHT_90:
					deceleration(fast_speed, 90, fast_accel);
					wall_attach();
					sc->position.x = 0;
					sc->position.y = 0;
					sc->position.theta = 0;
					turn(3 * M_PI, -M_PI / 2, 16 * M_PI);
					sc->position.x = 0;
					sc->position.y = 0;
					sc->position.theta = 0;
					acceleration(fast_speed, 90, fast_accel);
					break;
				case FAST_STOP:
					deceleration(fast_speed, 120, fast_accel);
					sc->set_target(0, 0);
					break;
			}
			_actions--;
		}
	}
};

#endif /* CONTROLLER_H_ */
