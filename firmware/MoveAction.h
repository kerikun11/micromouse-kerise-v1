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

#define LOOK_AHEAD_COUNT		10

class Straight90 {
public:
	Straight90() {
		last_index = 0;
	}
	Position getNextPoint(const Position& pos) {
		for (int i = last_index; i < length + 1000; i++) {
			Position diff = get(i) - pos;
			if (diff.x > 0) {
				last_index = i;
				return get(last_index + LOOK_AHEAD_COUNT);
			}
		}
		return Position(180, 0, 0);
	}
private:
	const int length = 150;
	const float interval = 0.6;
	int last_index;
	Position get(int index) {
		return interval * index / length;
	}
};

class Curve90 {
public:
	Curve90() {
		last_index = 0;
	}
	Position getNextPoint(const Position& pos) {
		for (int i = last_index; i < length + 1000; i++) {
			Position diff = get(i) - pos;
			if (diff.x > 0 && diff.y > 0) {
				last_index = i;
				return get(last_index + LOOK_AHEAD_COUNT);
			}
		}
		return Position(90, 90, M_PI / 2);
	}
private:
	const int length = 254;
	const float interval = 0.6;
	int last_index;
	Position get(int index) {
		if (index > length - 1) {
			return Position(90, 90.0 + interval * (index - length), M_PI / 2);
		}
		static const float data[][3] = { { 0, 0, 0 }, { 0.6, 0, 0 }, { 1.2, 0, 0 }, { 1.8, 0, 0 }, {
				2.4, 0, 0 }, { 3, 0, 0 }, { 3.6, 0, 0 }, { 4.2, 0, 0 }, { 4.8, 0, 0 },
				{ 5.4, 0, 0 }, { 6, 0, 0 }, { 6.1853, 0, 0 }, { 6.7853, 0, 0 }, { 7.3853, 0, 0 }, {
						7.9853, 9.0588e-05, 0.00015098 }, { 8.5853, 0.00036235, 0.00045294 }, {
						9.1853, 0.00090588, 0.00090588 }, { 9.7853, 0.0018118, 0.0015098 }, {
						10.385, 0.0031706, 0.0022647 }, { 10.985, 0.0050729, 0.0031706 }, { 11.585,
						0.0076094, 0.0042274 }, { 12.185, 0.010871, 0.0054353 }, { 12.785, 0.014947,
						0.0067941 }, { 13.385, 0.019929, 0.0083039 },
				{ 13.985, 0.025908, 0.0099647 }, { 14.585, 0.032974, 0.011776 }, { 15.185, 0.041217,
						0.013739 }, { 15.785, 0.050728, 0.015853 }, { 16.385, 0.061598, 0.018118 },
				{ 16.985, 0.073917, 0.020533 }, { 17.585, 0.087776, 0.0231 }, { 18.184, 0.10326,
						0.025818 }, { 18.784, 0.12047, 0.028686 }, { 19.384, 0.13949, 0.031706 }, {
						19.984, 0.16042, 0.034876 }, { 20.583, 0.18333, 0.038198 }, { 21.183,
						0.20832, 0.04167 }, { 21.782, 0.23549, 0.045294 }, { 22.381, 0.26492,
						0.049069 }, { 22.98, 0.2967, 0.052994 }, { 23.579, 0.33093, 0.05707 }, {
						24.178, 0.36768, 0.061298 }, { 24.777, 0.40706, 0.065676 }, { 25.376,
						0.44915, 0.070206 }, { 25.974, 0.49404, 0.074886 }, { 26.572, 0.54182,
						0.079717 }, { 27.17, 0.59258, 0.0847 }, { 27.767, 0.6464, 0.089833 }, {
						28.365, 0.70339, 0.095117 }, { 28.962, 0.76362, 0.10055 }, { 29.558,
						0.82718, 0.10614 }, { 30.155, 0.89417, 0.11188 },
				{ 30.75, 0.96466, 0.11776 }, { 31.346, 1.0388, 0.1238 },
				{ 31.941, 1.1165, 0.12999 }, { 32.535, 1.1981, 0.13633 },
				{ 33.129, 1.2835, 0.14283 }, { 33.722, 1.3728, 0.14947 },
				{ 34.315, 1.4662, 0.15626 }, { 34.907, 1.5637, 0.16321 },
				{ 35.498, 1.6654, 0.17031 }, { 36.089, 1.7714, 0.17755 },
				{ 36.679, 1.8817, 0.18495 }, { 37.268, 1.9965, 0.1925 }, { 37.856, 2.1158, 0.2002 },
				{ 38.443, 2.2397, 0.20805 }, { 39.029, 2.3684, 0.21605 },
				{ 39.614, 2.5018, 0.22421 }, { 40.198, 2.64, 0.23251 }, { 40.78, 2.7832, 0.24096 },
				{ 41.362, 2.9314, 0.24957 }, { 41.942, 3.0847, 0.25833 },
				{ 42.52, 3.2431, 0.26723 }, { 43.098, 3.4068, 0.27629 }, { 43.673, 3.5758, 0.2855 },
				{ 44.248, 3.7501, 0.29486 }, { 44.82, 3.9299, 0.3043 }, { 45.391, 4.1151, 0.31373 },
				{ 45.96, 4.3056, 0.32316 }, { 46.527, 4.5015, 0.33259 },
				{ 47.092, 4.7028, 0.34203 }, { 47.655, 4.9093, 0.35146 },
				{ 48.217, 5.1212, 0.36089 }, { 48.776, 5.3383, 0.37033 },
				{ 49.333, 5.5607, 0.37976 }, { 49.888, 5.7884, 0.38919 },
				{ 50.441, 6.0213, 0.39862 }, { 50.992, 6.2594, 0.40806 },
				{ 51.541, 6.5027, 0.41749 }, { 52.087, 6.7511, 0.42692 },
				{ 52.63, 7.0047, 0.43635 }, { 53.172, 7.2634, 0.44579 },
				{ 53.711, 7.5272, 0.45522 }, { 54.247, 7.7961, 0.46465 }, { 54.781, 8.07, 0.47408 },
				{ 55.312, 8.3489, 0.48352 }, { 55.841, 8.6329, 0.49295 },
				{ 56.367, 8.9218, 0.50238 }, { 56.89, 9.2156, 0.51182 }, { 57.41, 9.5144, 0.52125 },
				{ 57.927, 9.8181, 0.53068 }, { 58.442, 10.127, 0.54011 },
				{ 58.954, 10.44, 0.54955 }, { 59.462, 10.758, 0.55898 },
				{ 59.968, 11.081, 0.56841 }, { 60.471, 11.409, 0.57784 },
				{ 60.97, 11.741, 0.58728 }, { 61.466, 12.078, 0.59671 }, { 61.96, 12.42, 0.60614 },
				{ 62.449, 12.767, 0.61557 }, { 62.936, 13.118, 0.62501 },
				{ 63.419, 13.473, 0.63444 }, { 63.899, 13.834, 0.64387 },
				{ 64.376, 14.198, 0.65331 }, { 64.849, 14.568, 0.66274 },
				{ 65.318, 14.941, 0.67217 }, { 65.784, 15.319, 0.6816 },
				{ 66.246, 15.702, 0.69104 }, { 66.705, 16.088, 0.70047 }, { 67.16, 16.479, 0.7099 },
				{ 67.611, 16.875, 0.71933 }, { 68.059, 17.274, 0.72877 },
				{ 68.503, 17.678, 0.7382 }, { 68.943, 18.086, 0.74763 },
				{ 69.379, 18.498, 0.75706 }, { 69.811, 18.914, 0.7665 },
				{ 70.239, 19.334, 0.77593 }, { 70.664, 19.759, 0.78536 },
				{ 71.084, 20.187, 0.79479 }, { 71.5, 20.619, 0.80423 }, { 71.912, 21.055, 0.81366 },
				{ 72.32, 21.495, 0.82309 }, { 72.724, 21.939, 0.83253 },
				{ 73.124, 22.386, 0.84196 }, { 73.519, 22.838, 0.85139 },
				{ 73.91, 23.293, 0.86082 }, { 74.297, 23.752, 0.87026 },
				{ 74.679, 24.214, 0.87969 }, { 75.057, 24.68, 0.88912 },
				{ 75.431, 25.149, 0.89855 }, { 75.8, 25.622, 0.90799 }, { 76.165, 26.099, 0.91742 },
				{ 76.525, 26.578, 0.92685 }, { 76.881, 27.062, 0.93628 },
				{ 77.232, 27.548, 0.94572 }, { 77.578, 28.038, 0.95515 },
				{ 77.92, 28.531, 0.96458 }, { 78.257, 29.027, 0.97402 }, { 78.59, 29.527, 0.98345 },
				{ 78.918, 30.029, 0.99288 }, { 79.241, 30.535, 1.0023 }, { 79.559, 31.044, 1.0117 },
				{ 79.872, 31.555, 1.0212 }, { 80.181, 32.07, 1.0306 }, { 80.485, 32.587, 1.04 }, {
						80.783, 33.108, 1.0495 }, { 81.077, 33.631, 1.0589 }, { 81.366, 34.157,
						1.0683 }, { 81.65, 34.685, 1.0778 }, { 81.929, 35.216, 1.0872 }, { 82.203,
						35.75, 1.0966 }, { 82.472, 36.287, 1.1061 }, { 82.736, 36.825, 1.1155 }, {
						82.995, 37.367, 1.1249 }, { 83.248, 37.911, 1.1344 }, { 83.497, 38.457,
						1.1438 }, { 83.74, 39.005, 1.1532 }, { 83.978, 39.556, 1.1627 }, { 84.211,
						40.109, 1.1721 }, { 84.439, 40.664, 1.1815 }, { 84.661, 41.221, 1.191 }, {
						84.878, 41.78, 1.2004 }, { 85.09, 42.342, 1.2098 },
				{ 85.297, 42.905, 1.2193 }, { 85.498, 43.47, 1.2287 }, { 85.694, 44.037, 1.2381 }, {
						85.885, 44.606, 1.2476 }, { 86.07, 45.177, 1.257 },
				{ 86.25, 45.749, 1.2664 }, { 86.424, 46.324, 1.2759 }, { 86.593, 46.899, 1.2853 }, {
						86.757, 47.476, 1.2945 }, { 86.915, 48.055, 1.3036 }, { 87.069, 48.635,
						1.3125 }, { 87.217, 49.217, 1.3212 }, { 87.36, 49.799, 1.3298 }, { 87.498,
						50.383, 1.3383 }, { 87.632, 50.968, 1.3466 }, { 87.76, 51.554, 1.3547 }, {
						87.884, 52.141, 1.3627 }, { 88.004, 52.729, 1.3706 }, { 88.118, 53.318,
						1.3783 }, { 88.229, 53.908, 1.3858 }, { 88.335, 54.499, 1.3932 }, { 88.436,
						55.09, 1.4005 }, { 88.534, 55.682, 1.4076 }, { 88.627, 56.275, 1.4145 }, {
						88.717, 56.868, 1.4213 }, { 88.802, 57.462, 1.428 }, { 88.883, 58.056,
						1.4345 }, { 88.961, 58.651, 1.4408 }, { 89.035, 59.247, 1.447 }, { 89.106,
						59.842, 1.453 }, { 89.173, 60.439, 1.4589 }, { 89.236, 61.035, 1.4647 }, {
						89.297, 61.632, 1.4702 }, { 89.354, 62.23, 1.4757 },
				{ 89.407, 62.827, 1.481 }, { 89.458, 63.425, 1.4861 }, { 89.506, 64.023, 1.4911 }, {
						89.551, 64.621, 1.4959 }, { 89.593, 65.22, 1.5006 }, { 89.632, 65.819,
						1.5051 }, { 89.669, 66.417, 1.5095 }, { 89.703, 67.017, 1.5137 }, { 89.735,
						67.616, 1.5178 }, { 89.765, 68.215, 1.5217 }, { 89.792, 68.814, 1.5255 }, {
						89.817, 69.414, 1.5291 }, { 89.84, 70.013, 1.5326 }, { 89.861, 70.613,
						1.5359 }, { 89.88, 71.213, 1.5391 }, { 89.897, 71.812, 1.5421 }, { 89.912,
						72.412, 1.545 }, { 89.926, 73.012, 1.5477 }, { 89.938, 73.612, 1.5503 }, {
						89.949, 74.212, 1.5527 }, { 89.959, 74.812, 1.5549 }, { 89.967, 75.412,
						1.5571 }, { 89.974, 76.012, 1.559 }, { 89.98, 76.612, 1.5608 }, { 89.985,
						77.212, 1.5625 }, { 89.989, 77.812, 1.564 }, { 89.992, 78.412, 1.5654 }, {
						89.995, 79.012, 1.5666 }, { 89.997, 79.612, 1.5676 }, { 89.998, 80.212,
						1.5685 }, { 89.999, 80.812, 1.5693 }, { 90, 81.412, 1.5699 }, { 90, 82.012,
						1.5703 }, { 90, 82.612, 1.5706 }, { 90, 83.212, 1.5708 }, { 90, 83.812,
						1.5708 }, { 90, 83.812, 1.5708 }, { 90, 84.412, 1.5708 }, { 90, 85.012,
						1.5708 }, { 90, 85.612, 1.5708 }, { 90, 86.212, 1.5708 }, { 90, 86.812,
						1.5708 }, { 90, 87.412, 1.5708 }, { 90, 88.012, 1.5708 }, { 90, 88.612,
						1.5708 }, { 90, 89.212, 1.5708 }, { 90, 89.812, 1.5708 }, };
		return Position(data[index][0], data[index][1], data[index][2]);
	}
};

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
		Curve90 cv90;
		int cnt = 0;
		float integral = 0;
		while (1) {
			if (sc->position.y > 90) break;
			Thread::signal_wait(0x01);
			Position cur = sc->position;
			Position dir = (cv90.getNextPoint(cur) - cur).rotate(-cur.theta);
			float prop = atan(dir.y / dir.x);
			integral += prop;
			sc->set_target(velocity, prop * 50 + integral * 0.1);
			cnt++;
			if (cnt % 10 == 0) {
				printf("%.2f\t%.2f\t%.2f\n", dir.y, prop, integral);
			}
		}
		printf("Position2:\t(%06.1f, %06.1f, %06.3f)\n", sc->position.x, sc->position.y,
				sc->position.theta);
		sc->position = (sc->position - Position(90, 90, 0)).rotate(-M_PI / 2);
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
