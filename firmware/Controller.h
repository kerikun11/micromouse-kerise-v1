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

#define SPEED_CONTROLLER_PERIOD_US	1000

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
				if (actual_i.wheel[i] > 100) actual_i.wheel[i] = 100;
				if (actual_i.wheel[i] < -100) actual_i.wheel[i] = -100;
				actual_d.wheel[i] = (wheel_position[0][i] - 2 * wheel_position[1][i]
						+ wheel_position[2][i]) * 1000000 / SPEED_CONTROLLER_PERIOD_US;
			}
			wheel2pole(&actual_p);
			actual_p.rot = mpu->gyroZ() * M_PI / 180.0f;
			pole2wheel(&actual_p);
			const float Kp = 2;
			const float Ki = 0.2;
			const float Kd = 0.4;
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
		error.x = 0;
		error.y = 0;
		error.theta = 0;
		thread.start(this, &MoveAction::task);
	}
	enum ACTION {
		START_STEP, START_RETURN, GO_STRAIGHT, TURN_LEFT_90, TURN_RIGHT_90, RETURN,
//		GO_DIAGONAL,
//		TURN_LEFT_45,
//		TURN_RIGHT_45,
	};
	void set_action(enum ACTION action) {
		queue.put((enum ACTION*) action);
		_actions++;
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
	SpeedController::position_t error;
	struct WallDetector::WALL start_wall;

	float wall_avoid(bool side, bool flont) {
		float wall = 0;
		if (side) {
			if (wd->wall().side[0]) {
				wall += wd->wall_difference().side[0] * 1;
			}
			if (wd->wall().side[1]) {
				wall -= wd->wall_difference().side[1] * 1;
			}
		}
		if (flont) {
			if (wd->wall().flont[0] && wd->wall().flont[1]) {
				wall += wd->wall_difference().flont[0] * 1;
				wall -= wd->wall_difference().flont[1] * 1;
			}
		}
		return wall;
	}
	void straight(float speed, float target_position) {
		timer.reset();
		timer.start();
		while (1) {
			float wall = wall_avoid(true, false);
			sc->set_target(timer.read() * 2000, wall);
			Thread::wait(1);
			if (sc->actual().trans > speed) break;
		}
		while (1) {
			float wall = wall_avoid(true, true);
			float extra = target_position - sc->position.x;
			if (extra > 0) {
				float target_speed = sqrt(2 * 2000 * extra);
				target_speed = (target_speed > speed) ? speed : target_speed;
				sc->set_target(target_speed, wall);
			} else {
				break;
			}
			Thread::wait(1);
			if (sc->actual().trans < 0.1) break;
		}
		error.x = sc->position.x - target_position;
//		error.y = sc->position.y - 0;
		error.y = 0;
		error.theta = sc->position.theta - 0;
		printf("Position Error: x:%07.2f\ty:%07.2f\tth:%07.2f\n", error.x, error.y, error.theta);
	}
	void uturn() {
		timer.reset();
		timer.start();
		float speed = 1.2 * M_PI;
		while (1) {
			sc->set_target(0, timer.read() * 16 * M_PI);
			Thread::wait(1);
			if (sc->actual().rot > speed) break;
		}
		float target_position = M_PI;
		while (1) {
			float extra = target_position - sc->position.theta;
			float target_speed = sqrt(2 * 2 * M_PI * extra);
			target_speed = (target_speed > speed) ? speed : target_speed;
			if (extra > 0) {
				sc->set_target(0, target_speed);
			} else {
				break;
			}
			Thread::wait(1);
			if (sc->actual().rot < 0.01) break;
		}
		error.x = (sc->position.x - 0) * (-1);
		error.y = (sc->position.y - 0) * (-1);
		error.theta = sc->position.theta - M_PI;
		printf("Position Error: x:%07.2f\ty:%07.2f\tth:%07.2f\n", error.x, error.y, error.theta);
	}
	void turn(bool isLeft) {
		timer.reset();
		timer.start();
		float speed = 270.0f;
		float sign = isLeft ? (1) : (-1);
		while (1) {
			float x = sc->position.x;
			float y = sign * sc->position.y;
//			float theta = sign * sc->position.theta;
			float trans = timer.read() * 1800;
			float err = 1 - (x * x + (y - 90) * (y - 90)) / 90 / 90;
			sc->set_target(trans, sign * err + sign * trans / 90);
			Thread::wait(1);
			if (sc->actual().trans > speed) break;
		}
		while (1) {
			float x = sc->position.x;
			float y = sign * sc->position.y;
//			float theta = sign * sc->position.theta;
			float trans = speed;
			float err = 1 - (x * x + (y - 90) * (y - 90)) / 90 / 90;
			sc->set_target(trans, sign * err + sign * trans / 90);
			Thread::wait(1);
			if (y > 80) break;
		}
		while (1) {
//			float x = sc->position.x;
			float y = sign * sc->position.y;
//			float theta = sign * sc->position.theta;
			float extra = 90 - y;
			if (extra > 0) {
				float trans = sqrt(2 * 1800 * extra);
				trans = (trans > speed) ? speed : trans;
				sc->set_target(trans, sign * trans / 90);
			} else {
				break;
			}
			Thread::wait(1);
			if (sc->actual().trans < 0.1) break;
		}
		float error_x = sc->position.x - 90;
		float error_y = sc->position.y - sign * 90;
		float error_theta = sign * M_PI / 2 - sc->position.theta;
		printf("Position Error: x:%07.2f\ty:%07.2f\tth:%07.2f\n", error_x, error_y, error_theta);
		error.x = isLeft ? error_y : (-error_y);
		error.y = isLeft ? (-error_x) : error_x;
		error.theta = error_theta;
		printf("Position Error: x:%07.2f\ty:%07.2f\tth:%07.2f\n", error.x, error.y, error.theta);
	}
	void task() {
		while (1) {
			osEvent evt = queue.get();
			if (evt.status != osEventMessage) {
				printf("Error!\n");
				continue;
			}
			enum ACTION action = (enum ACTION) evt.value.v;
			start_wall = wd->wall();
			sc->position.x = error.x;
			sc->position.y = error.y;
			sc->position.theta = error.theta;
			switch (action) {
				case START_STEP:
					straight(500, 180 - 24);
					sc->set_target(0, 0);
					break;
				case START_RETURN:
					straight(500, 120);
					uturn();
					break;
				case GO_STRAIGHT:
					straight(500, 180);
					sc->set_target(0, 0);
					break;
				case TURN_LEFT_90:
					turn(true);
					sc->set_target(0, 0);
					break;
				case TURN_RIGHT_90:
					turn(false);
					sc->set_target(0, 0);
					break;
				case RETURN:
					uturn();
					sc->set_target(0, 0);
					break;
			}
			_actions--;
		}
	}
}
;

#endif /* CONTROLLER_H_ */
