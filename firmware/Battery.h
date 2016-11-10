/*
 * Battery.h
 *
 *  Created on: 2016/10/23
 *      Author: kerikun11
 */
#ifndef BATTERY_H_
#define BATTERY_H_

#include "mbed.h"
#include "config.h"

class Battery {
public:
	Battery(PinName pin, float threshold_voltage = 3.8f * 2) :
			adc(pin), _threshold_voltage(threshold_voltage) {

	}
	float voltage() {
		return adc.read() * 3.3 * (10 + 20) / 10;
	}
	bool check() {
		float _voltage = voltage();
		printf("Battery Voltage: %.3f [V]\n", _voltage);
		return (_voltage > _threshold_voltage);
	}
	int gage(int range) {
		int value = (float) range * (voltage() - _threshold_voltage) / (8.2f - _threshold_voltage);
		if (value < 0) {
			return 0;
		} else if (value < range) {
			return value;
		} else {
			return range - 1;
		}
	}
private:
	AnalogIn adc;
	const float _threshold_voltage;
};

#endif /* BATTERY_H_ */
