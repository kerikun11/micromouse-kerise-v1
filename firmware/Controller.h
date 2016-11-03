/*
 * Controller.h
 *
 *  Created on: 2016/10/28
 *      Author: kerikun11
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#define SPEED_CONTROLLER_PRIORITY	osPriorityAboveNormal
#define SPEED_CONTROLLER_PERIOD_US	1000

#define MACHINE_ROTATION_RADIUS		33.0f	// [mm]

class SpeedController {
public:
	SpeedController(Motor *mt, Encoders *enc) :
			mt(mt), enc(enc), ctrlThread(SPEED_CONTROLLER_PRIORITY) {
		ctrlThread.start(this, &SpeedController::ctrlTask);
		for (int i = 0; i < 2; i++) {
			target.wheel[i] = 0;
			for (int j = 0; j < 3; j++) {
				wheel_position[j][i] = 0;
			}
			wheel_p[i] = 0;
			wheel_i[i] = 0;
			wheel_d[i] = 0;
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
		target.trans = trans;
		target.rot = rot;
		pole2wheel(&target);
	}
	typedef struct {
		float trans;	//< translation
		float rot;		//< rotation
		float wheel[2];	//< wheel [0]: left, [1]: right
	} params_t;
	params_t target;
	float wheel_p[2];
	float wheel_i[2];
	float wheel_d[2];
private:
	Motor *mt;
	Encoders *enc;
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
				wheel_p[i] = (wheel_position[0][i] - wheel_position[1][i])
						* 1000000 / SPEED_CONTROLLER_PERIOD_US;
				wheel_i[i] += (wheel_p[i] - target.wheel[i])
						* SPEED_CONTROLLER_PERIOD_US / 1000000;
				wheel_d[i] = (wheel_position[0][i] - 2 * wheel_position[1][i]
						+ wheel_position[2][i])
						* 1000000/ SPEED_CONTROLLER_PERIOD_US;
			}
			const float Kp = 2;
			const float Ki = 0.01;
			const float Kd = 0.2;
			float pwm_value[2];
			for (int i = 0; i < 2; i++) {
				pwm_value[i] = Kp * (target.wheel[i] - wheel_p[i])
						+ Ki * (0 - wheel_i[i]) + Kd * (0 - wheel_d[i]);
			}
			mt->drive(pwm_value[0], pwm_value[1]);
		}
	}
	void pole2wheel(params_t *params) {
		params->wheel[0] = params->trans
				- MACHINE_ROTATION_RADIUS * params->rot / 2;
		params->wheel[1] = params->trans
				+ MACHINE_ROTATION_RADIUS * params->rot / 2;
	}
	void wheel2pole(params_t *params) {
		params->rot = (params->wheel[1] - params->wheel[0])
				/ MACHINE_ROTATION_RADIUS;
		params->trans = (params->wheel[1] + params->wheel[0]) / 2.0f;
	}
};

class MoveAction {
public:
	MoveAction(Encoders *enc, MPU6500 *mpu, Reflector *rfl, WallDetector *wd,
			SpeedController *sc) :
			enc(enc), mpu(mpu), rfl(rfl), wd(wd), sc(sc) {
		thread.start(this, &MoveAction::task);
	}
	enum ACTION {
		GO_STRAIGHT,
		GO_HALF,
		GO_DIAGONAL,
		TURN_LEFT_90,
		TURN_RIGHT_90,
		CURVE_LEFT_90,
		CURVE_RIGHT_90,
	};
	void set_action(enum ACTION action) {
		queue.put((enum ACTION*) action);
	}
private:
	Thread thread;
	Queue<enum ACTION, 128> queue;
	Encoders *enc;
	MPU6500 *mpu;
	Reflector *rfl;
	WallDetector *wd;
	SpeedController *sc;

	float wall_avoid(bool side, bool flont) {
		float wall = 0;
		if (side) {
			if (wd->wall().side[0]) {
				wall += wd->wall_difference().side[0] * 10;
			}
			if (wd->wall().side[1]) {
				wall -= wd->wall_difference().side[1] * 10;
			}
		}
		if (flont) {
			if (wd->wall().flont[0] && wd->wall().flont[1]) {
				wall += wd->wall_difference().flont[0] * 10;
				wall -= wd->wall_difference().flont[1] * 10;
			}
		}
		return wall;
	}
	void task() {
		while (1) {
			osEvent evt = queue.get();
			if (evt.status == osEventMessage) {
				enum ACTION action = (enum ACTION) evt.value.v;
				float start_position = enc->position();
				float start_angle = mpu->angleZ();
				struct WallDetector::WALL start_wall = wd->wall();
				switch (action) {
				case GO_STRAIGHT:
					while (1) {
						float wall = wall_avoid(true, false);
						sc->set_target(360, wall);
						Thread::wait(1);
						if (enc->position() - start_position > 90) {
							break;
						}
					}
					while (1) {
						float wall = wall_avoid(true, false);
						sc->set_target(360, wall);
						Thread::wait(1);
						if (start_wall.side[0] && !wd->wall().side[0]) {
							break;
						}
						if (start_wall.side[1] && !wd->wall().side[1]) {
							break;
						}
						if (enc->position() - start_position > 180) {
							break;
						}
					}
					sc->set_target(0, 0);
					break;
				case GO_HALF:
					while (1) {
						float wall = wall_avoid(true, false);
						sc->set_target(360, wall);
						Thread::wait(1);
						if (enc->position() - start_position > 90) {
							break;
						}
					}
					sc->set_target(0, 0);
					break;
				case GO_DIAGONAL:
					break;
				case TURN_LEFT_90:
					while (mpu->angleZ() - start_angle < 90) {
						sc->set_target(0, 240 / 30);
						Thread::wait(1);
					}
					sc->set_target(0, 0);
					break;
				case TURN_RIGHT_90:
					while (mpu->angleZ() - start_angle > -90) {
						sc->set_target(0, -240 / 30);
						Thread::wait(1);
					}
					sc->set_target(0, 0);
					break;
				case CURVE_LEFT_90:
					while (1) {
						float wall = wall_avoid(true, true);
						sc->set_target(240, wall);
						Thread::wait(1);
						if (wd->wall().side_flont[0]
								&& wd->wall().side_flont[1]) {
							if (wd->wall_difference().flont[0] < 0
									|| wd->wall_difference().flont[1] < 0) {
								break;
							}
						} else {
							if (enc->position() - start_position > 90) {
								break;
							}
						}
					}
					start_angle = mpu->angleZ();
					while (mpu->angleZ() - start_angle < 90) {
						sc->set_target(200, 200 / 20);
						Thread::wait(1);
					}
					sc->set_target(0, 0);
					break;
				case CURVE_RIGHT_90:
					while (1) {
						float wall = wall_avoid(true, true);
						sc->set_target(240, wall);
						Thread::wait(1);
						if (wd->wall().side_flont[0]
								&& wd->wall().side_flont[1]) {
							if (wd->wall_difference().flont[0] < 0
									|| wd->wall_difference().flont[1] < 0) {
								break;
							}
						} else {
							if (enc->position() - start_position > 90) {
								break;
							}
						}
					}
					start_angle = mpu->angleZ();
					while (mpu->angleZ() - start_angle > -90) {
						sc->set_target(200, -200 / 20);
						Thread::wait(1);
					}
					sc->set_target(0, 0);
					break;
				}
			}
		}
	}
}
;

#endif /* CONTROLLER_H_ */
