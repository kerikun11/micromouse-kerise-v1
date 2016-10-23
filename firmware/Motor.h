/*
 * Motor.h
 *
 *  Created on: 2016/10/23
 *      Author: kerikun11
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#define PWM_PERIOD_US	1000

enum MOTOR_DIR {
	MOTOR_DIR_GO, MOTOR_DIR_BACK, MOTOR_DIR_BRAKE, MOTOR_DIR_FREE,
};

class Motor {
public:
	Motor(PinName ctrl1_pin, PinName ctr2_pin) :
			ctrl1(ctrl1_pin), ctrl2(ctr2_pin) {
		ctrl1.period_us(PWM_PERIOD_US);
		ctrl2.period_us(PWM_PERIOD_US);
	}
	void free() {
		ctrl1.pulsewidth_us(0);
		ctrl2.pulsewidth_us(0);
	}
	void drive(int16_t width) {
		if (width > PWM_PERIOD_US / 2) {
			width = PWM_PERIOD_US / 2;
		}
		if (width < -PWM_PERIOD_US / 2) {
			width = -PWM_PERIOD_US / 2;
		}
		if (width > 0) {
			ctrl1.pulsewidth_us(PWM_PERIOD_US - width);
			ctrl2.pulsewidth_us(PWM_PERIOD_US);
		} else {
			ctrl1.pulsewidth_us(PWM_PERIOD_US);
			ctrl2.pulsewidth_us(PWM_PERIOD_US + width);
		}
	}
private:
	PwmOut ctrl1, ctrl2;
};

#endif /* MOTOR_H_ */
