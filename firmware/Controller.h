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
	float wheel_p[2];
	float wheel_i[2];
	float wheel_d[2];
private:
	typedef struct {
		float trans;	//< translation
		float rot;		//< rotation
		float wheel[2];	//< wheel [0]: left, [1]: right
	} params_t;
	Motor *mt;
	Encoders *enc;
	Thread ctrlThread;
	Ticker ctrlTicker;
	float wheel_position[3][2];
	params_t target;

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
			const float Kp = 1;
			const float Ki = 0;
			const float Kd = 0;
			float pwm_value[2];
			for (int i = 0; i < 2; i++) {
				pwm_value[i] = Kp * (target.wheel[i] - wheel_p[i])
						+ Ki * (0 - wheel_i[i]) + Kd * (0 - wheel_d[i]);
			}
			mt->drive(pwm_value[0], pwm_value[1]);
		}
	}
	void pole2wheel(params_t *params) {
		params->wheel[0] = (2 * params->trans
				- MACHINE_ROTATION_RADIUS * params->rot) / 2;
		params->wheel[1] = (2 * params->trans
				+ MACHINE_ROTATION_RADIUS * params->rot) / 2;
	}
	void wheel2pole(params_t *params) {
		params->rot = (params->wheel[1] - params->wheel[0])
				/ MACHINE_ROTATION_RADIUS;
		params->trans = (params->wheel[1] + params->wheel[0]) / 2.0f;
	}
};

#endif /* CONTROLLER_H_ */
