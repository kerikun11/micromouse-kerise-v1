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

#define MOVE_ACTION_PERIOD		1000
#define WALL_AVOID_ENABLED		false
#define WALL_ATTACH_ENABLED		false

class Trajectory {
public:
	Trajectory() {
	}
	Position getNextPoint(const Position& pos) {
		int index = findNextIndex(pos);
		return get_data(index);
	}
	const int data_len = 314;
	const float straight_len = 12.8f;
private:
	int findNextIndex(const Position& pos) {
		const int look_ahead = 5;
		for (int i = 0; i < data_len - look_ahead; i++) {
			if (pos.x < get_data(i).x && pos.y < get_data(i).y) return i + look_ahead;
		}
		return data_len - 1;
	}
//	float get_angle(int index) {
//		if (index < data_len / 2) {
//			Position data1 = get_data(index);
//			Position data2 = get_data(index + 1);
//			return atan((data2.y - data1.y) / (data2.x - data1.x));
//		} else if (index < data_len) {
//			Position data1 = get_data(index);
//			Position data2 = get_data(index + 1);
//			return M_PI - atan((data2.x - data1.x) / (data2.y - data1.y));
//		} else {
//			return M_PI / 2;
//		}
//	}
	Position get_data(int index) {
		static const float data[][2] = { { 0, 0 }, { 0, 0.4 }, { 0, 0.8 }, { 4.0864e-05, 1.2 }, {
				0.00016345, 1.6 }, { 0.00040864, 2 }, { 0.00081727, 2.4 }, { 0.0014302, 2.8 }, {
				0.0022884, 3.2 }, { 0.0034325, 3.6 }, { 0.0049036, 4 }, { 0.0067425, 4.4 }, {
				0.00899, 4.8 }, { 0.011687, 5.2 }, { 0.014874, 5.6 }, { 0.018593, 5.9999 }, {
				0.022883, 6.3999 }, { 0.027787, 6.7999 }, { 0.033344, 7.1999 },
				{ 0.039596, 7.5998 }, { 0.046583, 7.9997 }, { 0.054347, 8.3997 },
				{ 0.062928, 8.7996 }, { 0.072366, 9.1995 }, { 0.082704, 9.5993 },
				{ 0.09398, 9.9992 }, { 0.10624, 10.399 }, { 0.11952, 10.799 }, { 0.13386, 11.199 },
				{ 0.1493, 11.598 }, { 0.16588, 11.998 }, { 0.18365, 12.397 }, { 0.20265, 12.797 }, {
						0.22291, 13.197 }, { 0.24447, 13.596 }, { 0.26739, 13.995 }, { 0.29168,
						14.395 }, { 0.31741, 14.794 }, { 0.34461, 15.193 }, { 0.37331, 15.592 }, {
						0.40356, 15.991 }, { 0.4354, 16.389 }, { 0.46887, 16.788 }, { 0.50401,
						17.186 }, { 0.54085, 17.585 }, { 0.57945, 17.983 }, { 0.61984, 18.381 }, {
						0.66205, 18.779 }, { 0.70613, 19.176 }, { 0.75213, 19.573 }, { 0.80007,
						19.971 }, { 0.84999, 20.367 }, { 0.90195, 20.764 }, { 0.95597, 21.16 }, {
						1.0121, 21.556 }, { 1.0704, 21.952 }, { 1.1308, 22.348 },
				{ 1.1935, 22.743 }, { 1.2584, 23.137 }, { 1.3256, 23.532 }, { 1.3952, 23.926 }, {
						1.4671, 24.319 }, { 1.5415, 24.712 }, { 1.6183, 25.105 },
				{ 1.6975, 25.497 }, { 1.7793, 25.888 }, { 1.8635, 26.279 }, { 1.9501, 26.67 }, {
						2.0392, 27.06 }, { 2.1308, 27.449 }, { 2.2248, 27.838 }, { 2.3213, 28.226 },
				{ 2.4202, 28.614 }, { 2.5216, 29.001 }, { 2.6254, 29.387 }, { 2.7316, 29.773 }, {
						2.8403, 30.157 }, { 2.9513, 30.542 }, { 3.0648, 30.925 },
				{ 3.1807, 31.308 }, { 3.299, 31.69 }, { 3.4198, 32.072 }, { 3.5429, 32.452 }, {
						3.6684, 32.832 }, { 3.7963, 33.211 }, { 3.9266, 33.589 },
				{ 4.0593, 33.967 }, { 4.1943, 34.343 }, { 4.3317, 34.719 }, { 4.4715, 35.093 }, {
						4.6136, 35.467 }, { 4.7581, 35.84 }, { 4.9049, 36.212 }, { 5.0541, 36.584 },
				{ 5.2056, 36.954 }, { 5.3595, 37.323 }, { 5.5156, 37.691 }, { 5.6741, 38.059 }, {
						5.8349, 38.425 }, { 5.998, 38.79 }, { 6.1634, 39.154 }, { 6.3311, 39.517 },
				{ 6.5011, 39.879 }, { 6.6733, 40.241 }, { 6.8478, 40.6 }, { 7.0246, 40.959 }, {
						7.2036, 41.317 }, { 7.3849, 41.674 }, { 7.5685, 42.029 },
				{ 7.7542, 42.383 }, { 7.9422, 42.736 }, { 8.1324, 43.088 }, { 8.3249, 43.439 }, {
						8.5195, 43.788 }, { 8.7163, 44.136 }, { 8.9153, 44.483 },
				{ 9.1165, 44.829 }, { 9.3199, 45.174 }, { 9.5255, 45.517 }, { 9.7332, 45.859 }, {
						9.943, 46.199 }, { 10.155, 46.538 }, { 10.369, 46.876 }, { 10.585, 47.213 },
				{ 10.804, 47.548 }, { 11.024, 47.882 }, { 11.247, 48.214 }, { 11.471, 48.545 }, {
						11.698, 48.874 }, { 11.927, 49.203 }, { 12.158, 49.529 },
				{ 12.391, 49.854 }, { 12.626, 50.178 }, { 12.863, 50.5 }, { 13.102, 50.821 }, {
						13.343, 51.14 }, { 13.586, 51.458 }, { 13.831, 51.774 }, { 14.078, 52.089 },
				{ 14.327, 52.402 }, { 14.578, 52.713 }, { 14.831, 53.023 }, { 15.086, 53.331 }, {
						15.343, 53.638 }, { 15.601, 53.943 }, { 15.862, 54.246 },
				{ 16.125, 54.548 }, { 16.389, 54.848 }, { 16.655, 55.147 }, { 16.924, 55.443 }, {
						17.194, 55.739 }, { 17.466, 56.032 }, { 17.739, 56.323 },
				{ 18.015, 56.613 }, { 18.292, 56.902 }, { 18.572, 57.188 }, { 18.853, 57.473 }, {
						19.136, 57.755 }, { 19.42, 58.036 }, { 19.707, 58.316 }, { 19.995, 58.593 },
				{ 20.285, 58.869 }, { 20.576, 59.143 }, { 20.869, 59.415 }, { 21.165, 59.685 }, {
						21.461, 59.953 }, { 21.76, 60.219 }, { 22.06, 60.484 }, { 22.361, 60.746 },
				{ 22.665, 61.007 }, { 22.97, 61.266 }, { 23.277, 61.523 }, { 23.585, 61.778 }, {
						23.895, 62.031 }, { 24.206, 62.282 }, { 24.519, 62.531 },
				{ 24.834, 62.778 }, { 25.15, 63.023 }, { 25.468, 63.266 }, { 25.787, 63.507 }, {
						26.107, 63.746 }, { 26.43, 63.983 }, { 26.753, 64.218 }, { 27.078, 64.451 },
				{ 27.405, 64.682 }, { 27.733, 64.911 }, { 28.063, 65.137 }, { 28.394, 65.362 }, {
						28.726, 65.585 }, { 29.06, 65.805 }, { 29.395, 66.023 }, { 29.731, 66.24 },
				{ 30.069, 66.454 }, { 30.408, 66.666 }, { 30.749, 66.876 }, { 31.091, 67.083 }, {
						31.434, 67.289 }, { 31.778, 67.492 }, { 32.124, 67.694 },
				{ 32.471, 67.893 }, { 32.819, 68.09 }, { 33.169, 68.284 }, { 33.519, 68.477 }, {
						33.871, 68.667 }, { 34.224, 68.855 }, { 34.578, 69.041 },
				{ 34.934, 69.224 }, { 35.29, 69.406 }, { 35.648, 69.585 }, { 36.007, 69.761 }, {
						36.367, 69.936 }, { 36.728, 70.108 }, { 37.09, 70.278 }, { 37.453, 70.446 },
				{ 37.817, 70.611 }, { 38.182, 70.775 }, { 38.549, 70.935 }, { 38.916, 71.094 }, {
						39.284, 71.25 }, { 39.653, 71.404 }, { 40.024, 71.555 }, { 40.395, 71.705 },
				{ 40.767, 71.851 }, { 41.14, 71.996 }, { 41.514, 72.138 }, { 41.888, 72.278 }, {
						42.264, 72.415 }, { 42.641, 72.55 }, { 43.018, 72.683 }, { 43.396, 72.813 },
				{ 43.775, 72.941 }, { 44.155, 73.067 }, { 44.535, 73.19 }, { 44.917, 73.311 }, {
						45.299, 73.429 }, { 45.682, 73.545 }, { 46.065, 73.659 }, { 46.45, 73.77 },
				{ 46.834, 73.878 }, { 47.22, 73.985 }, { 47.606, 74.088 }, { 47.993, 74.19 }, {
						48.381, 74.289 }, { 48.769, 74.385 }, { 49.158, 74.479 },
				{ 49.547, 74.571 }, { 49.937, 74.66 }, { 50.328, 74.747 }, { 50.719, 74.831 }, {
						51.11, 74.913 }, { 51.502, 74.992 }, { 51.895, 75.069 }, { 52.288, 75.143 },
				{ 52.681, 75.215 }, { 53.075, 75.285 }, { 53.47, 75.352 }, { 53.864, 75.417 }, {
						54.259, 75.48 }, { 54.655, 75.54 }, { 55.051, 75.598 }, { 55.447, 75.654 },
				{ 55.843, 75.708 }, { 56.24, 75.76 }, { 56.636, 75.81 }, { 57.033, 75.858 }, {
						57.431, 75.904 }, { 57.828, 75.948 }, { 58.226, 75.991 },
				{ 58.624, 76.031 }, { 59.022, 76.07 }, { 59.421, 76.106 }, { 59.819, 76.141 }, {
						60.218, 76.175 }, { 60.616, 76.207 }, { 61.015, 76.237 },
				{ 61.414, 76.266 }, { 61.813, 76.293 }, { 62.212, 76.319 }, { 62.612, 76.343 }, {
						63.011, 76.366 }, { 63.41, 76.387 }, { 63.81, 76.408 }, { 64.209, 76.427 },
				{ 64.609, 76.444 }, { 65.009, 76.461 }, { 65.408, 76.477 }, { 65.808, 76.491 }, {
						66.208, 76.504 }, { 66.608, 76.516 }, { 67.008, 76.528 },
				{ 67.407, 76.538 }, { 67.807, 76.547 }, { 68.207, 76.556 }, { 68.607, 76.564 }, {
						69.007, 76.571 }, { 69.407, 76.577 }, { 69.807, 76.583 },
				{ 70.207, 76.587 }, { 70.607, 76.592 }, { 71.007, 76.595 }, { 71.407, 76.599 }, {
						71.807, 76.601 }, { 72.207, 76.604 }, { 72.607, 76.605 },
				{ 73.007, 76.607 }, { 73.407, 76.608 }, { 73.807, 76.609 }, { 74.207, 76.61 }, {
						74.607, 76.61 }, { 75.007, 76.61 }, { 75.407, 76.61 }, { 75.807, 76.61 }, {
						76.207, 76.61 }, { 76.607, 76.61 }, };
		return Position(data[index][1] + straight_len, data[index][0]);
	}
}
;

class MoveAction {
public:
	MoveAction(Buzzer *bz, Encoders *enc, MPU6500 *mpu, Reflector *rfl, WallDetector *wd,
			SpeedController *sc) :
			bz(bz), enc(enc), mpu(mpu), rfl(rfl), wd(wd), sc(sc),
					thread(PRIORITY_MOVE_ACTION, STACK_SIZE_MOVE_ACTION) {
		_actions = 0;
		set_params(600);
		ticker.attach_us(this, &MoveAction::isr, MOVE_ACTION_PERIOD);
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
	void set_params(float fast_speed) {
		this->fast_speed = fast_speed;
	}
	void set_params_relative(float add) {
		this->fast_speed += add;
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
	Ticker ticker;
	Queue<enum ACTION, 128> queue;
	Timer timer;
	int _actions;
	Position error;
	float fast_speed;
	Trajectory trajectory;

	void isr() {
		thread.signal_set(0x01);
	}
	void wall_avoid() {
#if WALL_AVOID_ENABLED
		void wall_attach() {
			if (wd->wall().side[0]) {
				sc->position.y -= wd->wall_difference().side[0] * 0.00001 * sc->actual().trans;
			}
			if (wd->wall().side[1]) {
				sc->position.y += wd->wall_difference().side[1] * 0.00001 * sc->actual().trans;
			}
		}
#endif
	}
	float fix_y() {
		float rot = -sc->position.y * 0.01;
		return rot;
	}
	void wall_attach() {
#if WALL_ATTACH_ENABLED
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
				sc->set_target(trans * 500, rot * 5);
				if (fabs(trans) < 0.1f && fabs(rot) < 0.1f) break;
				Thread::wait(1);
			}
			sc->set_target(0, 0);
			error = sc->position;
			error.x = 0;
			sc->position = error;
//			printf("Wall Attach:\t(%05.1f, %05.1f, %04.2f)\n", error.x, error.y, error.theta);
		}
#endif
	}
	void acceleration(float speed, float target_distance, float accel = 6000) {
		timer.reset();
		timer.start();
		float v0 = sc->actual().trans;
		while (1) {
			Thread::signal_wait(0x01);
			if (sc->actual().trans > speed) break;
			if (sc->position.x > target_distance) break;
			float trans = v0 + timer.read() * accel;
			sc->set_target(trans, fix_y());
			wall_avoid();
		}
		while (1) {
			Thread::signal_wait(0x01);
			if (sc->position.x > target_distance) break;
			sc->set_target(speed, fix_y());
			wall_avoid();
		}
		error = sc->position - Position(target_distance, 0, 0);
		sc->position = error;
//		printf("Accel %04.0f:\t(%05.1f, %05.1f, %04.2f)\n", target_distance, error.x, error.y,
//				error.theta);
	}
	void deceleration(float speed, float target_distance, float accel = 6000) {
		while (1) {
			Thread::signal_wait(0x01);
			if (fabs(sc->actual().trans) < 5) break;
			float extra = target_distance - sc->position.x;
			float target_speed = sqrt(2 * accel * fabs(extra));
			target_speed = (target_speed > speed) ? speed : target_speed;
			if (extra > 0) {
				sc->set_target(target_speed, fix_y());
			} else {
//				sc->set_target(-target_speed, fix_y());
				sc->set_target(-100, 0);
			}
			wall_avoid();
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
	void turn(float speed, float target_angle) {
		const float accel = 64 * M_PI;
		const float decel = 32 * M_PI;
		timer.reset();
		timer.start();
		while (1) {
			Thread::signal_wait(0x01);
			if (fabs(sc->actual().rot) > speed) break;
			if (target_angle > 0) {
				sc->set_target(0, timer.read() * accel);
			} else {
				sc->set_target(0, -timer.read() * accel);
			}
		}
		while (1) {
			Thread::signal_wait(0x01);
			if (fabs(sc->actual().rot) < 0.5) break;
			float extra = target_angle - sc->position.theta;
			float target_speed = sqrt(2 * decel * fabs(extra));
			target_speed = (target_speed > speed) ? speed : target_speed;
			if (extra > 0) {
				sc->set_target(0, target_speed);
			} else {
				sc->set_target(0, -target_speed);
			}
		}
		error = sc->position.rotate(-target_angle);
		sc->position = error;
//		printf("Turn %04.2f:\t(%05.1f, %05.1f, %05.2f)\n", target_angle, error.x, error.y,
//				error.theta);
	}
	void curve_left(float velocity) {
		while (1) {
			if (sc->position.x > trajectory.straight_len) break;
			Thread::signal_wait(0x01);
			sc->set_target(velocity, -sc->position.y * 0.1);
		}
		printf("Position1:\t(%06.1f, %06.1f, %06.3f)\n", sc->position.x, sc->position.y,
				sc->position.theta);
		while (1) {
			if ((sc->position.x > 90) || (sc->position.y > 90 - trajectory.straight_len)) break;
			Thread::signal_wait(0x01);
			Position u = trajectory.getNextPoint(sc->position) - sc->position;
			u.rotate(-sc->position.theta);
			sc->set_target(velocity, atan(u.y / u.x) * 10);
		}
		printf("Position2:\t(%06.1f, %06.1f, %06.3f)\n", sc->position.x, sc->position.y,
				sc->position.theta);
		while (1) {
			if (sc->position.y > 90) break;
			Thread::signal_wait(0x01);
			sc->set_target(velocity, (90 - sc->position.x) * 0.1);
		}
		printf("Position3:\t(%06.1f, %06.1f, %06.3f)\n", sc->position.x, sc->position.y,
				sc->position.theta);
		sc->position = sc->position - Position(90, 90, 0);
		error = sc->position.rotate(-M_PI / 2);
		sc->position = error;
		printf("Position4:\t(%06.1f, %06.1f, %06.3f)\n", sc->position.x, sc->position.y,
				sc->position.theta);
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
			printf("Position:\t(%06.1f, %06.1f, %06.3f)\n", sc->position.x, sc->position.y,
					sc->position.theta);
			const float rot_speed = 4.0f * M_PI;
			const float trans_speed = 400;
			switch (action) {
				case START_STEP:
					error.reset();
					sc->position = error;
					straight(200, 90 - 24 - 6);
					sc->set_target(0, 0);
					break;
				case START_INIT:
					turn(rot_speed, M_PI / 2);
					wall_attach();
					turn(rot_speed, M_PI / 2);
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
					turn(rot_speed, M_PI / 2);
					straight(trans_speed, 180);
					wall_attach();
					sc->set_target(0, 0);
					break;
				case TURN_RIGHT_90:
					turn(rot_speed, -M_PI / 2);
					straight(trans_speed, 180);
					wall_attach();
					sc->set_target(0, 0);
					break;
				case RETURN:
					if (mpu->angleZ() > 0) {
						turn(rot_speed, -M_PI / 2);
						wall_attach();
						turn(rot_speed, -M_PI / 2);
					} else {
						turn(rot_speed, M_PI / 2);
						wall_attach();
						turn(rot_speed, M_PI / 2);
					}
					sc->set_target(0, 0);
					break;
				case FAST_START_STEP:
					error.reset();
					sc->position = error;
					acceleration(fast_speed, 180 - 24 - 6);
					break;
				case FAST_GO_STRAIGHT:
					acceleration(fast_speed, 180);
					break;
				case FAST_GO_DIAGONAL:
					acceleration(fast_speed, 90 * 1.4142);
					break;
				case FAST_GO_HALF:
					acceleration(fast_speed, 90);
					break;
				case FAST_TURN_LEFT_45:
					break;
				case FAST_TURN_LEFT_90:
					curve_left(trans_speed);
					break;
				case FAST_TURN_RIGHT_45:
					break;
				case FAST_TURN_RIGHT_90:
					break;
				case FAST_STOP:
					deceleration(fast_speed, 90);
					wall_attach();
					sc->set_target(0, 0);
					break;
			}
			_actions--;
		}
	}
};

#endif /* MOVEACTION_H_ */
