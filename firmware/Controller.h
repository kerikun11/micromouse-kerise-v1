/*
 * Controller.h
 *
 *  Created on: 2016/10/28
 *      Author: kerikun11
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

class StraightControl {
public:
	StraightControl(Motor *mt, Encoders *enc, EncoderMeasure *em, MPU6500 *mpu,
			GyroMeasure *gm, Reflector *rfl, WallDetector *wd) :
			mt(mt), enc(enc), em(em), mpu(mpu), gm(gm), rfl(rfl), wd(wd) {
	}
	void enable(bool wall = true) {
		ctrlTicker.attach_us(this, &StraightControl::motor_drive, 100);
		if (wall)
			wallTicker.attach_us(this, &StraightControl::wall_avoid, 100);
	}
	void disable() {
		ctrlTicker.detach();
		wallTicker.detach();
	}
private:
	Ticker ctrlTicker;
	Ticker wallTicker;
	Motor *mt;
	Encoders *enc;
	EncoderMeasure *em;
	MPU6500 *mpu;
	GyroMeasure *gm;
	Reflector *rfl;
	WallDetector *wd;

	void motor_drive() {
		double angle = 0;
		double straight = 0;

		angle = gm->get_pid(100, 1, 1);
		angle = (angle > 100) ? 100 : angle;
		angle = (angle < -100) ? -100 : angle;

		straight = em->get_pid(100, 1, 1);
		straight = (straight > 100) ? 100 : straight;
		straight = (straight < -100) ? -100 : straight;

		int32_t valueL = straight - angle / 2;
		int32_t valueR = straight + angle / 2;
		mt->drive(valueL, valueR);
	}
	void wall_avoid() {
		double wall = 0;
		if (wd->wall().side[0] && wd->wall().side[1]) {
			wall = (rfl->side(1) - rfl->side(0)) * 0.00001;
			wall = (wall > 0.01) ? 0.01 : wall;
			wall = (wall < -0.01) ? -0.01 : wall;
			gm->set_target_relative(wall);
		} else if (wd->wall().side[0]) {
			wall = (800 - rfl->side(0)) * 0.00001;
			wall = (wall > 0.01) ? 0.01 : wall;
			wall = (wall < -0.01) ? -0.01 : wall;
			gm->set_target_relative(wall);
		} else if (wd->wall().side[1]) {
			wall = (800 - rfl->side(1)) * 0.00001;
			wall = (wall > 0.01) ? 0.01 : wall;
			wall = (wall < -0.01) ? -0.01 : wall;
			gm->set_target_relative(-wall);
		}

//		if (wd->wall().flont[0] && wd->wall().flont[1]) {
//			wall = (rfl->flont(0) - rfl->flont(1)) * 0.00001;
//			wall = (wall > 0.01) ? 0.01 : wall;
//			wall = (wall < -0.01) ? -0.01 : wall;
//			gm->set_target_relative(wall);
//		}
	}
};

class MoveAction {
public:
	MoveAction(Motor *mt, Encoders *enc, EncoderMeasure *em, MPU6500 *mpu,
			GyroMeasure *gm, Reflector *rfl, WallDetector *wd,
			StraightControl *sc) :
			mt(mt), enc(enc), em(em), mpu(mpu), gm(gm), rfl(rfl), wd(wd), sc(sc) {
		state = STATE_IDOLE;
		thread.start(this, &MoveAction::task);
	}
	enum ACTION {
		ACTION_GO_STRAIGHT,
		ACTION_GO_HALF,
		ACTION_GO_DIAGONAL,
		ACTION_TURN_LEFT_90,
		ACTION_TURN_RIGHT_90,
		ACTION_TURN_LEFT_45,
		ACTION_TURN_RIGHT_45,
	};
	enum STATE {
		STATE_IDOLE, STATE_IN_PROCESS,
	};
	void set_action(enum ACTION action) {
		queue.put((enum ACTION*) action);
	}
private:
	Thread thread;
	Queue<enum ACTION, 16> queue;
	Motor *mt;
	Encoders *enc;
	EncoderMeasure *em;
	MPU6500 *mpu;
	GyroMeasure *gm;
	Reflector *rfl;
	WallDetector *wd;
	StraightControl *sc;
	enum STATE state;

	void task() {
		while (1) {
			osEvent evt = queue.get();
			state = STATE_IN_PROCESS;
			if (evt.status == osEventMessage) {
				enum ACTION action = (enum ACTION) evt.value.v;
				switch (action) {
				case ACTION_GO_STRAIGHT:
					sc->disable();
					em->set_target_relative(180);
					sc->enable(true);
					while (abs(em->get_p()) > 20)
						;
					break;
				case ACTION_GO_HALF:
					sc->disable();
					em->set_target_relative(90);
					sc->enable(true);
					while (abs(em->get_p()) > 20)
						;
					break;
				case ACTION_GO_DIAGONAL:
					sc->disable();
					em->set_target_relative(180 * 1.4142);
					sc->enable(false);
					while (abs(em->get_p()) > 20)
						;
					break;
				case ACTION_TURN_LEFT_90:
					sc->disable();
					gm->set_target_relative(90);
					sc->enable(false);
					while (abs(gm->get_p()) > 2)
						;
					break;
				case ACTION_TURN_RIGHT_90:
					sc->disable();
					gm->set_target_relative(-90);
					sc->enable(false);
					while (abs(gm->get_p()) > 2)
						;
					break;
				case ACTION_TURN_LEFT_45:
					sc->disable();
					gm->set_target_relative(45);
					sc->enable(false);
					while (abs(gm->get_p()) > 2)
						;
					break;
				case ACTION_TURN_RIGHT_45:
					sc->disable();
					gm->set_target_relative(-45);
					sc->enable(false);
					while (abs(gm->get_p()) > 2)
						;
					break;
				}
			}
			state = STATE_IDOLE;
		}
	}
};

#endif /* CONTROLLER_H_ */
