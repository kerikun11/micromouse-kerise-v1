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

#define MOVE_ACTION_PERIOD			1000
#define WALL_ATTACH_ENABLED			true
#define WALL_AVOID_ENABLED			false

#define LOOK_AHEAD_UNIT				20
#define TRAJECTORY_PROP_GAIN		50
#define TRAJECTORY_INTEGRAL_GAIN	0

class Straight {
public:
	Straight() {
	}
	Position getNextDir(const Position cur, const float velocity) {
		int look_ahead = LOOK_AHEAD_UNIT;
		Position dir = (getNextPoint(cur, look_ahead) - cur).rotate(-cur.theta);
		dir.theta = atan(dir.y / (dir.x + interval)) * velocity / v_const;
		dir *= velocity / LOOK_AHEAD_UNIT;
		return dir;
	}
private:
	const float interval = 1.0f;
	const float v_const = 1000.0f;
	Position getNextPoint(const Position& pos, int look_ahead) {
		return Position(pos.x + interval * (1 + look_ahead), 0, 0);
	}
};

class Curve90 {
public:
	Curve90() {
		last_index = 0;
	}
	Position getNextDir(const Position cur, const float velocity) {
		int look_ahead = LOOK_AHEAD_UNIT;
		Position dir = (getNextPoint(cur, look_ahead) - cur).rotate(-cur.theta);
		dir.theta = atan(dir.y / (dir.x + interval));
		dir *= velocity / LOOK_AHEAD_UNIT;
		return dir;
	}
private:
	const int length = 152;
	const float interval = 1.0f;
	const float v_const = 1000.0f;
	int last_index;
	Position getNextPoint(const Position& pos, int look_ahead) {
		for (int i = last_index; i < length * 100; i++) {
//			Position diff = get(i) - pos;
//			if (diff.x > 0 && diff.y > 0) {
//				last_index = i;
//				return get(last_index + LOOK_AHEAD_COUNT);
//			}
			Position target = get_position(i);
			Position dir = (target - pos).rotate(-target.theta);
			if (dir.x > 0) {
				last_index = i;
				return get_position(last_index + 1 + look_ahead);
			}
		}
		return Position(0, 0, 0);
	}
	Position get_position(int index) {
		if (index > length - 1) {
			return Position(90, 90.0 + interval * (index - length + 1), M_PI / 2);
		}
		static const float data[][3] = { { 0, 0, 0 }, { 1, 0, 0 }, { 2, 0, 0 }, { 3, 0, 0 }, { 4, 0,
				0 }, { 5, 0, 0 }, { 6, 0, 0 }, { 7, 0, 0 }, { 8, 0, 0 }, { 9.8969, 0, 0 }, { 10.897,
				0, 0 }, { 11.897, 0, 0 }, { 12.897, 0.00060392, 0.00060392 }, { 13.897, 0.0024157,
				0.0018118 }, { 14.897, 0.0060392, 0.0036235 }, { 15.897, 0.012078, 0.0060392 }, {
				16.897, 0.021137, 0.0090588 }, { 17.897, 0.033819, 0.012682 }, { 18.897, 0.050728,
				0.01691 }, { 19.896, 0.072467, 0.021741 }, { 20.896, 0.09964, 0.027176 }, { 21.895,
				0.13285, 0.033216 }, { 22.895, 0.1727, 0.039859 }, { 23.894, 0.21979, 0.047106 }, {
				24.892, 0.27472, 0.054957 }, { 25.89, 0.33808, 0.063412 }, { 26.887, 0.41049,
				0.07247 }, { 27.884, 0.49253, 0.082133 }, { 28.88, 0.5848, 0.0924 }, { 29.874,
				0.68789, 0.10327 }, { 30.868, 0.80238, 0.11474 }, { 31.86, 0.92886, 0.12682 }, {
				32.85, 1.0679, 0.13951 }, { 33.838, 1.2201, 0.15279 }, { 34.825, 1.386, 0.16668 }, {
				35.808, 1.5662, 0.18118 }, { 36.789, 1.7612, 0.19627 }, { 37.767, 1.971, 0.21137 },
				{ 38.741, 2.1956, 0.22647 }, { 39.712, 2.4348, 0.24157 },
				{ 40.679, 2.6887, 0.25667 }, { 41.643, 2.9571, 0.27176 }, { 42.602, 3.24, 0.28686 },
				{ 43.557, 3.5374, 0.30196 }, { 44.507, 3.8492, 0.31706 },
				{ 45.452, 4.1753, 0.33216 }, { 46.392, 4.5156, 0.34725 },
				{ 47.328, 4.8701, 0.36235 }, { 48.257, 5.2386, 0.37745 },
				{ 49.181, 5.6212, 0.39255 }, { 50.099, 6.0176, 0.40765 },
				{ 51.011, 6.4279, 0.42274 }, { 51.917, 6.8519, 0.43784 },
				{ 52.816, 7.2895, 0.45294 }, { 53.708, 7.7406, 0.46804 },
				{ 54.594, 8.2052, 0.48314 }, { 55.472, 8.683, 0.49823 },
				{ 56.343, 9.1741, 0.51333 }, { 57.207, 9.6783, 0.52843 },
				{ 58.063, 10.195, 0.54353 }, { 58.911, 10.725, 0.55863 },
				{ 59.751, 11.268, 0.57372 }, { 60.582, 11.824, 0.58882 },
				{ 61.406, 12.392, 0.60392 }, { 62.22, 12.972, 0.61902 },
				{ 63.026, 13.564, 0.63412 }, { 63.822, 14.169, 0.64921 },
				{ 64.609, 14.785, 0.66431 }, { 65.387, 15.414, 0.67941 },
				{ 66.156, 16.054, 0.69451 }, { 66.914, 16.705, 0.70961 },
				{ 67.663, 17.368, 0.7247 }, { 68.402, 18.042, 0.7398 }, { 69.13, 18.727, 0.7549 }, {
						69.848, 19.424, 0.77 }, { 70.555, 20.13, 0.7851 },
				{ 71.252, 20.848, 0.80019 }, { 71.938, 21.576, 0.81529 },
				{ 72.612, 22.314, 0.83039 }, { 73.275, 23.062, 0.84549 },
				{ 73.927, 23.821, 0.86059 }, { 74.568, 24.589, 0.87568 },
				{ 75.197, 25.366, 0.89078 }, { 75.814, 26.153, 0.90588 },
				{ 76.419, 26.949, 0.92098 }, { 77.012, 27.755, 0.93608 },
				{ 77.592, 28.569, 0.95117 }, { 78.161, 29.391, 0.96627 },
				{ 78.717, 30.223, 0.98137 }, { 79.26, 31.062, 0.99647 }, { 79.791, 31.91, 1.0116 },
				{ 80.308, 32.765, 1.0267 }, { 80.813, 33.629, 1.0418 }, { 81.304, 34.5, 1.0569 }, {
						81.783, 35.378, 1.072 }, { 82.248, 36.263, 1.0871 },
				{ 82.7, 37.155, 1.1022 }, { 83.138, 38.054, 1.1173 }, { 83.562, 38.959, 1.1324 }, {
						83.973, 39.871, 1.1474 }, { 84.37, 40.789, 1.1625 }, { 84.753, 41.713,
						1.1776 }, { 85.122, 42.642, 1.1927 }, { 85.477, 43.577, 1.2078 }, { 85.818,
						44.517, 1.2229 }, { 86.145, 45.462, 1.238 }, { 86.457, 46.412, 1.2531 }, {
						86.755, 47.367, 1.2682 }, { 87.039, 48.326, 1.2833 }, { 87.308, 49.289,
						1.2984 }, { 87.562, 50.256, 1.3135 }, { 87.802, 51.227, 1.3286 }, { 88.027,
						52.201, 1.3437 }, { 88.238, 53.179, 1.3588 }, { 88.433, 54.159, 1.3739 }, {
						88.614, 55.143, 1.389 }, { 88.78, 56.129, 1.4041 },
				{ 88.932, 57.117, 1.418 }, { 89.071, 58.108, 1.4313 }, { 89.198, 59.1, 1.444 }, {
						89.312, 60.093, 1.4561 }, { 89.415, 61.088, 1.4675 }, { 89.507, 62.083,
						1.4784 }, { 89.59, 63.08, 1.4887 }, { 89.662, 64.077, 1.4983 }, { 89.725,
						65.075, 1.5074 }, { 89.78, 66.074, 1.5158 }, { 89.827, 67.073, 1.5237 }, {
						89.867, 68.072, 1.5309 }, { 89.9, 69.072, 1.5376 },
				{ 89.928, 70.071, 1.5436 }, { 89.949, 71.071, 1.5491 }, { 89.966, 72.071, 1.5539 },
				{ 89.979, 73.071, 1.5581 }, { 89.988, 74.071, 1.5617 }, { 89.994, 75.071, 1.5648 },
				{ 89.998, 76.071, 1.5672 }, { 89.999, 77.071, 1.569 }, { 90, 78.071, 1.5702 }, { 90,
						79.071, 1.5708 }, { 90, 80.071, 1.5708 }, { 90, 80.071, 1.5708 }, { 90,
						81.071, 1.5708 }, { 90, 82.071, 1.5708 }, { 90, 83.071, 1.5708 }, { 90,
						84.071, 1.5708 }, { 90, 85.071, 1.5708 }, { 90, 86.071, 1.5708 }, { 90,
						87.071, 1.5708 }, { 90, 88.071, 1.5708 }, { 90, 89.071, 1.5708 }, { 90, 90,
						1.5708 }, };
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
		STOP,
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
				"turn_right_90", "return", "stop", "fast_start_step", "fast_go_straight",
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
			sc->position.x = 0;
			printf("Attach:\t(%06.1f, %06.1f, %06.3f)\n", sc->position.x, sc->position.y,
					sc->position.theta);
		}
#endif
	}
	void turn(float target_angle, float speed) {
		const float accel = 64 * M_PI;
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
			float target_speed = sqrt(2 * accel * fabs(extra));
			target_speed = (target_speed > speed) ? speed : target_speed;
			if (extra > 0) {
				sc->set_target(0, target_speed);
			} else {
				sc->set_target(0, -target_speed);
			}
		}
		sc->position = sc->position.rotate(-target_angle);
	}
	void curve_left(const float velocity) {
		Curve90 cv90;
		int cnt = 0;
		float integral = 0;
		while (1) {
			if (sc->position.y > 90) break;
			Thread::signal_wait(0x01);
			Position dir = cv90.getNextDir(sc->position, velocity);
			integral += dir.theta * TRAJECTORY_INTEGRAL_GAIN * MOVE_ACTION_PERIOD / 1000000;
			sc->set_target(dir.x, (dir.theta + integral) * TRAJECTORY_PROP_GAIN);
			if (cnt % 10 == 0) {
//				printf("%.3f\t%.3f\t%.4f\n", dir.x, dir.y, dir.theta);
			}
			cnt++;
		}
		sc->set_target(velocity, 0);
		sc->position = (sc->position - Position(90, 90, 0)).rotate(-M_PI / 2);
	}
	void curve_right(const float velocity) {
		Curve90 cv90;
		int cnt = 0;
		float integral = 0;
		while (1) {
			if (-sc->position.y > 90) break;
			Thread::signal_wait(0x01);
			Position dir = cv90.getNextDir(sc->position.mirror_x(), velocity).mirror_x();
			integral += dir.theta * TRAJECTORY_INTEGRAL_GAIN * MOVE_ACTION_PERIOD / 1000000;
			sc->set_target(dir.x, (dir.theta + integral) * TRAJECTORY_PROP_GAIN);
			if (cnt % 10 == 0) {
//				printf("%.3f\t%.3f\t%.4f\n", dir.x, dir.y, dir.theta);
			}
			cnt++;
		}
		sc->set_target(velocity, 0);
		sc->position = (sc->position - Position(90, -90, 0)).rotate(M_PI / 2);
	}
	void straight_x(const float distance, const float v0, const float v1, const float v2,
			const float accel = 9000) {
		Straight st;
		timer.reset();
		timer.start();
		int cnt = 0;
		float integral = 0;
		while (1) {
			if (sc->position.x > distance) break;
			Thread::signal_wait(0x01);
			float velocity;
			if (sc->position.x > distance / 2) {
				velocity = sqrt(2 * accel * fabs(distance - sc->position.x) + v2 * v2);
				if (velocity > v1) velocity = v1;
			} else if (sc->actual().trans < v1) {
				velocity = v0 + timer.read() * accel;
			} else {
				velocity = v1;
			}
			Position dir = st.getNextDir(sc->position, velocity);
			integral += dir.theta * TRAJECTORY_INTEGRAL_GAIN * MOVE_ACTION_PERIOD / 1000000;
			sc->set_target(dir.x, (dir.theta + integral) * TRAJECTORY_PROP_GAIN);
			if (cnt % 10 == 0) {
//				printf("%.3f\t%.3f\t%.4f\n", dir.x, dir.y, dir.theta);
			}
			cnt++;
			wall_avoid();
		}
		sc->set_target(v2, 0);
		sc->position -= Position(distance, 0, 0);
	}
	void task() {
		while (1) {
			osEvent evt = queue.get();
			if (evt.status != osEventMessage) {
				printf("Error!\n");
				continue;
			}
			enum ACTION action = (enum ACTION) evt.value.v;
			printf("Action:\t%s\n", action_string(action));
			printf("Start:\t(%06.1f, %06.1f, %06.3f)\n", sc->position.x, sc->position.y,
					sc->position.theta);
			const float velocity = 600;
			const float omega = 2.0f * M_PI;
			switch (action) {
				case START_STEP:
					sc->position.reset();
					straight_x(180 - 24 - 6, 0, velocity, velocity);
					break;
				case START_INIT:
					straight_x(90, velocity, velocity, 0);
					wall_attach();
					turn(M_PI / 2, omega);
					wall_attach();
					turn(M_PI / 2, omega);
					sc->set_target(-10, 0);
					Thread::wait(100);
					sc->set_target(-200, 0);
					Thread::wait(1000);
					sc->position.reset();
					sc->set_target(0, 0);
					break;
				case GO_STRAIGHT:
					straight_x(180, velocity, 1000, velocity);
					break;
				case TURN_LEFT_90:
					curve_left(velocity);
					break;
				case TURN_RIGHT_90:
					curve_right(velocity);
					break;
				case RETURN:
					straight_x(90, velocity, velocity, 0);
					wall_attach();
					turn(M_PI / 2, omega);
					wall_attach();
					turn(M_PI / 2, omega);
					straight_x(90, 0, velocity, velocity);
					break;
				case STOP:
					straight_x(90, velocity, velocity, 0);
					wall_attach();
					sc->set_target(0, 0);
					break;
				case FAST_START_STEP:
					straight_x(180 - 24 - 6, 0, velocity, velocity);
					break;
				case FAST_GO_STRAIGHT:
					straight_x(180, velocity, 1000, velocity);
					break;
				case FAST_GO_DIAGONAL:
					break;
				case FAST_GO_HALF:
					break;
				case FAST_TURN_LEFT_45:
					break;
				case FAST_TURN_LEFT_90:
					curve_left(velocity);
					break;
				case FAST_TURN_RIGHT_45:
					break;
				case FAST_TURN_RIGHT_90:
					curve_right(velocity);
					break;
				case FAST_STOP:
					straight_x(90, velocity, velocity, 0);
					wall_attach();
					sc->set_target(0, 0);
					break;
			}
			_actions--;
			printf("Error:\t(%06.1f, %06.1f, %06.3f)\n", sc->position.x, sc->position.y,
					sc->position.theta);
			Thread::wait(5);
		}
	}
};

#endif /* MOVEACTION_H_ */
