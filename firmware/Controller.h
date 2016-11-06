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
private:
	Motor *mt;
	Encoders *enc;
	MPU6500 *mpu;
	Thread ctrlThread;
	Ticker ctrlTicker;
	float wheel_position[3][2];
	params_t target_p;
	params_t actual_p;
	params_t actual_i;
	params_t actual_d;

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
				actual_p.wheel[i] =
						(wheel_position[0][i] - wheel_position[1][i])
								* 1000000/ SPEED_CONTROLLER_PERIOD_US;
				actual_i.wheel[i] += (actual_p.wheel[i] - target_p.wheel[i])
						* SPEED_CONTROLLER_PERIOD_US / 1000000;
				actual_d.wheel[i] = (wheel_position[0][i]
						- 2 * wheel_position[1][i] + wheel_position[2][i])
						* 1000000 / SPEED_CONTROLLER_PERIOD_US;
			}
			wheel2pole(&actual_p);
			actual_p.rot = mpu->gyroZ() * M_PI / 180.0f;
			pole2wheel(&actual_p);
			const float Kp = 2;
			const float Ki = 0.01;
			const float Kd = 0.5;
			float pwm_value[2];
			for (int i = 0; i < 2; i++) {
				pwm_value[i] = Kp * (target_p.wheel[i] - actual_p.wheel[i])
						+ Ki * (0 - actual_i.wheel[i])
						+ Kd * (0 - actual_d.wheel[i]);
			}
			mt->drive(pwm_value[0], pwm_value[1]);

			position.theta += actual_p.rot * SPEED_CONTROLLER_PERIOD_US
					/ 1000000;
			position.x += actual_p.trans * cos(actual_p.rot)
					* SPEED_CONTROLLER_PERIOD_US / 1000000;
			position.y += actual_p.trans * sin(actual_p.rot)
					* SPEED_CONTROLLER_PERIOD_US / 1000000;
		}
	}
	void pole2wheel(params_t *params) {
		params->wheel[0] = params->trans
				- MACHINE_ROTATION_RADIUS * params->rot;
		params->wheel[1] = params->trans
				+ MACHINE_ROTATION_RADIUS * params->rot;
	}
	void wheel2pole(params_t *params) {
		params->rot = (params->wheel[1] - params->wheel[0]) / 2.0f
				/ MACHINE_ROTATION_RADIUS;
		params->trans = (params->wheel[1] + params->wheel[0]) / 2.0f;
	}
};

class MoveAction {
public:
	MoveAction(Buzzer *bz, Encoders *enc, MPU6500 *mpu, Reflector *rfl,
			WallDetector *wd, SpeedController *sc) :
			bz(bz), enc(enc), mpu(mpu), rfl(rfl), wd(wd), sc(sc),
					thread(PRIORITY_MOVE_ACTION) {
		_tasks = 0;
		thread.start(this, &MoveAction::task);
	}
	enum ACTION {
		START_STEP,
		START_RETURN,
		GO_STRAIGHT,
		TURN_LEFT_90,
		TURN_RIGHT_90,
		RETURN,
//		GO_DIAGONAL,
//		TURN_LEFT_45,
//		TURN_RIGHT_45,
	};
	void set_action(enum ACTION action) {
		queue.put((enum ACTION*) action);
		_tasks++;
	}
	int tasks() const {
		return _tasks;
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
	int _tasks;

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
	void task() {
		while (1) {
			osEvent evt = queue.get();
			if (evt.status != osEventMessage) {
				printf("Error!\n");
				continue;
			}
			enum ACTION action = (enum ACTION) evt.value.v;
			float start_position = enc->position();
			float start_angle = mpu->angleZ();
			struct WallDetector::WALL start_wall = wd->wall();
//			sc->position.x = 0;
//			sc->position.y = 0;
//			sc->position.theta = 0;
			switch (action) {
				case START_STEP:
					timer.reset();
					timer.start();
					while (1) {
						float wall = wall_avoid(true, false);
						sc->set_target(timer.read() * 2000, wall);
						Thread::wait(1);
						if (timer.read() * 2000 > 500) break;
					}
					while (1) {
						float wall = wall_avoid(true, false);
						sc->set_target(500, wall);
						Thread::wait(1);
						if (sc->position.x > 140) break;
					}
					while (1) {
						float wall = wall_avoid(true, true);
						float trans = sqrt(
								2 * 2000
										* (180 - enc->position()
												+ start_position));
						sc->set_target(trans, wall);
						Thread::wait(1);
						if (trans < 1) break;
					}
					printf("trans:%07.3f\n", sc->actual().trans);
					sc->set_target(0, 0);
					break;
				case START_RETURN:
					break;
				case GO_STRAIGHT:
					break;
				case TURN_LEFT_90:
					break;
				case TURN_RIGHT_90:
					break;
				case RETURN:
//					timer.reset();
//					timer.start();
//					while (1) {
//						sc->set_target(0, timer.read() * 8 * M_PI);
//						Thread::wait(1);
//						if (sc->actual().rot > M_PI / 4) break;
//					}
//					while (1) {
//						sc->set_target(0, 2 * M_PI);
//						Thread::wait(1);
//						if (sc->position.theta > M_PI * 3 / 4) break;
//					}
//					while (1) {
//						float rot = sqrt(
//								2 * 8 * M_PI * (M_PI - sc->position.theta));
//						sc->set_target(0, rot);
//						Thread::wait(1);
//						if (abs(rot) < 0.1) break;
//					}
//					sc->set_target(0, 0);
					break;
			}
			_tasks--;
		}
	}
};

#endif /* CONTROLLER_H_ */
