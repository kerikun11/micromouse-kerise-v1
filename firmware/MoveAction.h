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
		return Position(get_x(index) + straight_len, get_y(index));
	}
	const int data_len = 261;
	const float straight_len = 27.2f;
private:
	int findNextIndex(const Position& pos) {
		for (int i = 0; i < data_len - 1; i++) {
			if (pos.x - straight_len < get_x(i) && pos.y < get_y(i)) return i + 1;
		}
		return data_len - 1;
	}
	float get_x(int index) {
		static const float data[] = { 0, 0.4, 0.8, 1.2, 1.6, 2, 2.4, 2.8, 3.2, 3.6, 4, 4.4, 4.8,
				5.2, 5.6, 5.9999, 6.3999, 6.7999, 7.1999, 7.5998, 7.9997, 8.3997, 8.7996, 9.1995,
				9.5993, 9.9992, 10.399, 10.799, 11.199, 11.598, 11.998, 12.397, 12.797, 13.197,
				13.596, 13.995, 14.395, 14.794, 15.193, 15.592, 15.991, 16.389, 16.788, 17.186,
				17.585, 17.983, 18.381, 18.779, 19.176, 19.573, 19.971, 20.367, 20.764, 21.16,
				21.556, 21.952, 22.348, 22.743, 23.137, 23.532, 23.926, 24.319, 24.712, 25.105,
				25.497, 25.888, 26.279, 26.67, 27.059, 27.449, 27.837, 28.225, 28.612, 28.999,
				29.384, 29.769, 30.153, 30.537, 30.919, 31.3, 31.68, 32.06, 32.438, 32.815, 33.191,
				33.566, 33.94, 34.312, 34.684, 35.053, 35.422, 35.789, 36.154, 36.518, 36.881,
				37.241, 37.6, 37.958, 38.314, 38.668, 39.02, 39.37, 39.718, 40.065, 40.41, 40.752,
				41.093, 41.432, 41.768, 42.103, 42.435, 42.765, 43.094, 43.42, 43.743, 44.065,
				44.384, 44.701, 45.015, 45.327, 45.637, 45.944, 46.249, 46.552, 46.851, 47.149,
				47.444, 47.736, 48.025, 48.312, 48.596, 48.878, 49.157, 49.433, 49.706, 49.976,
				50.244, 50.509, 50.771, 51.03, 51.286, 51.539, 51.789, 52.036, 52.281, 52.522,
				52.76, 52.995, 53.227, 53.456, 53.681, 53.904, 54.123, 54.339, 54.552, 54.762,
				54.968, 55.171, 55.371, 55.567, 55.761, 55.95, 56.137, 56.32, 56.5, 56.676, 56.849,
				57.018, 57.184, 57.346, 57.506, 57.661, 57.814, 57.963, 58.109, 58.252, 58.391,
				58.527, 58.66, 58.79, 58.917, 59.041, 59.162, 59.28, 59.395, 59.506, 59.615, 59.721,
				59.825, 59.925, 60.023, 60.118, 60.21, 60.3, 60.387, 60.471, 60.553, 60.632, 60.709,
				60.783, 60.855, 60.925, 60.992, 61.057, 61.12, 61.18, 61.238, 61.294, 61.348, 61.4,
				61.45, 61.498, 61.544, 61.588, 61.63, 61.671, 61.709, 61.746, 61.781, 61.815,
				61.847, 61.877, 61.906, 61.933, 61.959, 61.983, 62.006, 62.027, 62.048, 62.067,
				62.084, 62.101, 62.116, 62.131, 62.144, 62.156, 62.168, 62.178, 62.187, 62.196,
				62.204, 62.211, 62.217, 62.223, 62.227, 62.232, 62.235, 62.239, 62.241, 62.244,
				62.245, 62.247, 62.248, 62.249, 62.25, 62.25, 62.25, 62.25, 62.25, 62.25, 62.25 };
		return data[index];
	}
	float get_y(int index) {
		static const float data[] = { 0, 0, 0, 4.0864e-05, 0.00016345, 0.00040864, 0.00081727,
				0.0014302, 0.0022884, 0.0034325, 0.0049036, 0.0067425, 0.00899, 0.011687, 0.014874,
				0.018593, 0.022883, 0.027787, 0.033344, 0.039596, 0.046583, 0.054347, 0.062928,
				0.072366, 0.082704, 0.09398, 0.10624, 0.11952, 0.13386, 0.1493, 0.16588, 0.18365,
				0.20265, 0.22291, 0.24447, 0.26739, 0.29168, 0.31741, 0.34461, 0.37331, 0.40356,
				0.4354, 0.46887, 0.50401, 0.54085, 0.57945, 0.61984, 0.66205, 0.70613, 0.75213,
				0.80007, 0.84999, 0.90195, 0.95597, 1.0121, 1.0704, 1.1308, 1.1935, 1.2584, 1.3256,
				1.3952, 1.4671, 1.5415, 1.6183, 1.6976, 1.7794, 1.8637, 1.9507, 2.0402, 2.1325,
				2.2274, 2.3251, 2.4256, 2.5289, 2.635, 2.7439, 2.8558, 2.9706, 3.0884, 3.2092,
				3.3331, 3.46, 3.59, 3.7231, 3.8594, 3.9988, 4.1415, 4.2874, 4.4366, 4.589, 4.7448,
				4.904, 5.0665, 5.2323, 5.4016, 5.5744, 5.7506, 5.9302, 6.1132, 6.2996, 6.4894,
				6.6825, 6.8789, 7.0787, 7.2817, 7.4881, 7.6977, 7.9105, 8.1266, 8.3459, 8.5683,
				8.7939, 9.0227, 9.2545, 9.4895, 9.7276, 9.9687, 10.213, 10.46, 10.71, 10.963,
				11.219, 11.478, 11.74, 12.005, 12.272, 12.543, 12.816, 13.092, 13.371, 13.652,
				13.936, 14.223, 14.513, 14.805, 15.1, 15.397, 15.697, 15.999, 16.304, 16.611,
				16.921, 17.233, 17.547, 17.864, 18.183, 18.505, 18.828, 19.154, 19.482, 19.813,
				20.145, 20.48, 20.816, 21.155, 21.496, 21.838, 22.183, 22.529, 22.878, 23.228,
				23.58, 23.934, 24.29, 24.647, 25.006, 25.367, 25.729, 26.093, 26.459, 26.826,
				27.194, 27.564, 27.935, 28.307, 28.681, 29.056, 29.432, 29.809, 30.188, 30.567,
				30.947, 31.329, 31.711, 32.094, 32.478, 32.863, 33.249, 33.635, 34.022, 34.41,
				34.799, 35.188, 35.578, 35.968, 36.359, 36.751, 37.143, 37.535, 37.928, 38.322,
				38.716, 39.11, 39.505, 39.9, 40.295, 40.691, 41.087, 41.483, 41.88, 42.277, 42.674,
				43.071, 43.469, 43.867, 44.265, 44.663, 45.061, 45.459, 45.858, 46.257, 46.656,
				47.055, 47.454, 47.853, 48.252, 48.651, 49.051, 49.45, 49.85, 50.25, 50.649, 51.049,
				51.449, 51.848, 52.248, 52.648, 53.048, 53.448, 53.848, 54.248, 54.648, 55.048,
				55.447, 55.847, 56.247, 56.647, 57.047, 57.447, 57.847, 58.247, 58.647, 59.047,
				59.447, 59.847, 60.247, 60.647, 61.047, 61.447, 61.847, 62.247 };
		return data[index];
	}
};

class MoveAction {
public:
	MoveAction(Buzzer *bz, Encoders *enc, MPU6500 *mpu, Reflector *rfl, WallDetector *wd,
			SpeedController *sc) :
			bz(bz), enc(enc), mpu(mpu), rfl(rfl), wd(wd), sc(sc),
					thread(PRIORITY_MOVE_ACTION, STACK_SIZE_MOVE_ACTION) {
		_actions = 0;
		set_params(400);
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
			sc->set_target(velocity, -sc->position.y * 0.01);
		}
		printf("Position1:\t(%06.1f, %06.1f, %06.3f)\n", sc->position.x, sc->position.y,
				sc->position.theta);
		while (1) {
			if ((sc->position.x > 90) || (sc->position.y > 90 - trajectory.straight_len)) break;
			Thread::signal_wait(0x01);
			Position u = trajectory.getNextPoint(sc->position) - sc->position;
			u.rotate(-sc->position.theta);
			sc->set_target(velocity, u.y * 1);
		}
		printf("Position2:\t(%06.1f, %06.1f, %06.3f)\n", sc->position.x, sc->position.y,
				sc->position.theta);
		while (1) {
			if (sc->position.y > 90) break;
			Thread::signal_wait(0x01);
			sc->set_target(velocity, (90 - sc->position.x) * 0.01);
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
