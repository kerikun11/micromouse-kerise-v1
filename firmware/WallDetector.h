/*
 * WallDetector.h
 *
 *  Created on: 2016/11/11
 *      Author: kerikun11
 */

#ifndef WALLDETECTOR_H_
#define WALLDETECTOR_H_

#include "mbed.h"
#include "config.h"
#include "Reflector.h"

#define WALL_UPDATE_PERIOD_US		1000

class WallDetector {
public:
	WallDetector(Reflector *rfl) :
			rfl(rfl), updateThread(PRIORITY_WALL_UPDATE, STACK_SIZE_WALL_UPDATE) {

//		_wall_ref.side[0] = 260;
//		_wall_ref.side[1] = 260;
		_wall_ref.flont[0] = 200;
		_wall_ref.flont[1] = 200;

//		_wall_distance.side[0] = 900;
//		_wall_distance.side[1] = 700;
		_wall_distance.flont[0] = 560;
		_wall_distance.flont[1] = 560;

		updateThread.start(this, &WallDetector::updateTask);
		printf("0x%08X: Wall Detector\n", (unsigned int) updateThread.gettid());
		updateTicker.attach_us(this, &WallDetector::updateIsr,
		WALL_UPDATE_PERIOD_US);
	}
	void calibration() {
		rfl->enable();
		Thread::wait(1);
		for (int i = 0; i < 2; i++) {
			_wall_distance.side[i] = 0;
		}
		const int ave_cnt = 500;
		for (int j = 0; j < ave_cnt; j++) {
			Thread::wait(1);
			for (int i = 0; i < 2; i++) {
				_wall_distance.side[i] += rfl->side(i);
			}
		}
		for (int i = 0; i < 2; i++) {
			_wall_distance.side[i] /= ave_cnt;
			_wall_ref.side[i] = _wall_distance.side[i] / 3;
		}
		for (int i = 0; i < 2; i++) {
			_wall_distance.flont[i] = (_wall_distance.side[0] + _wall_distance.side[1]) * 0.56;
			_wall_ref.flont[i] = _wall_distance.flont[i] / 3;
		}
	}
	struct WALL {
		bool side[2];
		bool flont[2];
	};
	struct WALL_VALUE {
		float side[2];
		float flont[2];
	};
	struct WALL wall() {
		return _wall;
	}
	struct WALL_VALUE wall_distance() {
		return _wall_distance;
	}
	struct WALL_VALUE wall_difference() {
		return _wall_difference;
	}
private:
	Reflector *rfl;
	Thread updateThread;
	Ticker updateTicker;
	struct WALL _wall;
	struct WALL_VALUE _wall_ref;
	struct WALL_VALUE _wall_distance;
	struct WALL_VALUE _wall_difference;
	void updateIsr() {
		updateThread.signal_set(0x01);
	}
	void updateTask() {
		while (1) {
			Thread::signal_wait(0x01);

			for (int i = 0; i < 2; i++) {
				int16_t value = rfl->side(i);
				if (value > _wall_ref.side[i] * 1.02) _wall.side[i] = true;
				else if (value < _wall_ref.side[i] * 0.98) _wall.side[i] = false;
				_wall_difference.side[i] = (_wall_distance.side[i] - value)
						/ _wall_distance.side[i];
			}
			for (int i = 0; i < 2; i++) {
				int16_t value = rfl->flont(i);
				if (value > _wall_ref.flont[i] * 1.02) _wall.flont[i] = true;
				else if (value < _wall_ref.flont[i] * 0.98) _wall.flont[i] = false;
				_wall_difference.flont[i] = (_wall_distance.flont[i] - value)
						/ _wall_distance.flont[i];
			}
		}
	}
}
;

#endif /* WALLDETECTOR_H_ */
