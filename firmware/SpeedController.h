/*
 * SpeedController.h
 *
 *  Created on: 2016/10/28
 *      Author: kerikun11
 */

#ifndef SPEEDCONTROLLER_H_
#define SPEEDCONTROLLER_H_

#include "mbed.h"
#include "config.h"

#define SPEED_CONTROLLER_PERIOD_US	1000

class Position {
public:
	Position(float x = 0, float y = 0, float theta = 0) :
			x(x), y(y), theta(theta) {
	}
	float x, y, theta;

	inline void reset() {
		x = 0;
		y = 0;
		theta = 0;
	}
	inline void set(float x = 0, float y = 0, float theta = 0) {
		this->x = x;
		this->y = y;
		this->theta = theta;
	}
	inline Position rotate(float angle) {
		float _x = x;
		float _y = y;
		x = _x * cos(angle) - _y * sin(angle);
		y = _x * sin(angle) + _y * cos(angle);
//		theta = theta + angle;
		return *this;
	}
	inline float getNorm() const {
		return sqrt(x * x + y * y);
	}
	inline Position mirror_x() {
		y = -y;
		theta = -theta;
		return *this;
	}
	inline Position operator=(const Position &obj) {
		x = obj.x;
		y = obj.y;
		theta = obj.theta;
		return *this;
	}
	inline Position operator+() const {
		return Position(x, y, theta);
	}
	inline Position operator+(const Position &obj) const {
		return Position(x + obj.x, y + obj.y, theta + obj.theta);
	}
	inline Position operator+=(const Position &obj) {
		x += obj.x;
		y += obj.y;
		theta += obj.theta;
		return *this;
	}
	inline Position operator-() const {
		return Position(-x, -y, -theta);
	}
	inline Position operator-(const Position &obj) const {
		return Position(x - obj.x, y - obj.y, theta - obj.theta);
	}
	inline Position operator-=(const Position &obj) {
		x -= obj.x;
		y -= obj.y;
		theta -= obj.theta;
		return *this;
	}
	inline Position operator/(const float &div) {
		return Position(x / div, y / div, theta);
	}
	inline Position operator/=(const float &div) {
		x /= div;
		y /= div;
		return *this;
	}
	inline Position operator*(const float &div) {
		return Position(x * div, y * div, theta);
	}
	inline Position operator*=(const float &div) {
		x *= div;
		y *= div;
		return *this;
	}
};

class WheelParameter {
public:
	WheelParameter(float trans = 0, float rot = 0) :
			trans(trans), rot(rot) {
	}
	float trans;	//< translation
	float rot;		//< rotation
	float wheel[2];	//< wheel [0]: left, [1]: right
	void pole2wheel() {
		wheel[0] = trans - MACHINE_ROTATION_RADIUS * rot;
		wheel[1] = trans + MACHINE_ROTATION_RADIUS * rot;
	}
	void wheel2pole() {
		rot = (wheel[1] - wheel[0]) / 2.0f / MACHINE_ROTATION_RADIUS;
		trans = (wheel[1] + wheel[0]) / 2.0f;
	}
};

class SpeedController {
public:
	SpeedController(Motor *mt, Encoders *enc, MPU6500 *mpu) :
			mt(mt), enc(enc), mpu(mpu),
					ctrlThread(PRIORITY_SPEED_CONTROLLER, STACK_SIZE_SPEED_CONTROLLER) {
		ctrlThread.start(this, &SpeedController::ctrlTask);
		printf("0x%08X: Speed Controller\n", (unsigned int) ctrlThread.gettid());
		for (int i = 0; i < 2; i++) {
			target_p.wheel[i] = 0;
			for (int j = 0; j < 3; j++) {
				wheel_position[j][i] = enc->position(i);
			}
			actual_p.wheel[i] = 0;
			actual_i.wheel[i] = 0;
			actual_d.wheel[i] = 0;
		}
		actual_prev.trans = 0;
		actual_prev.rot = 0;
	}
	void enable() {
		for (int i = 0; i < 2; i++) {
			target_p.wheel[i] = 0;
			for (int j = 0; j < 3; j++) {
				wheel_position[j][i] = enc->position(i);
			}
			actual_p.wheel[i] = 0;
			actual_i.wheel[i] = 0;
			actual_d.wheel[i] = 0;
		}
		position.reset();
		ctrlTicker.attach_us(this, &SpeedController::ctrlIsr,
		SPEED_CONTROLLER_PERIOD_US);
	}
	void disable() {
		ctrlTicker.detach();
		mt->free();
	}
	void set_target(float trans, float rot) {
		target_p.trans = trans;
		target_p.rot = rot;
		target_p.pole2wheel();
	}
	WheelParameter& actual_velocity() {
		return actual_p;
	}
	Position& getPosition() {
		return position;
	}
private:
	Motor *mt;
	Encoders *enc;
	MPU6500 *mpu;
	Thread ctrlThread;
	Ticker ctrlTicker;
	float wheel_position[3][2];
	WheelParameter target_p;
	WheelParameter actual_prev;
	WheelParameter actual_p;
	WheelParameter actual_i;
	WheelParameter actual_d;
	float pwm_value[2];
	Position position;

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
				actual_d.wheel[i] = (wheel_position[0][i] - 2 * wheel_position[1][i]
						+ wheel_position[2][i]) * 1000000 / SPEED_CONTROLLER_PERIOD_US;
			}
			actual_p.wheel2pole();
			actual_p.rot = mpu->gyroZ() * M_PI / 180.0f;
			actual_p.pole2wheel();
			const float Kp = 2.40f;
			const float Ki = 200.0f;
			const float Kd = 4.00f;
			for (int i = 0; i < 2; i++) {
				pwm_value[i] = Kp * (target_p.wheel[i] - actual_p.wheel[i])
						+ Ki * (0 - actual_i.wheel[i]) + Kd * (0 - actual_d.wheel[i]);
			}
			mt->drive(pwm_value[0], pwm_value[1]);

			position.theta += (actual_prev.rot + actual_p.rot) / 2 * SPEED_CONTROLLER_PERIOD_US
					/ 1000000;
			position.x += (actual_prev.trans + actual_p.trans) / 2 * cos(position.theta)
					* SPEED_CONTROLLER_PERIOD_US / 1000000;
			position.y += (actual_prev.trans + actual_p.trans) / 2 * sin(position.theta)
					* SPEED_CONTROLLER_PERIOD_US / 1000000;

			actual_prev.trans = actual_p.trans;
			actual_prev.rot = actual_p.rot;
		}
	}
};

#endif /* SPEEDCONTROLLER_H_ */
