/*
 * Battery.h
 *
 *  Created on: 2016/10/23
 *      Author: kerikun11
 */
#ifndef BATTERY_H_
#define BATTERY_H_

#include "mbed.h"

class Battery{
public:
	Battery(PinName pin, float threshold_voltage = 3.8f*2) : adc(pin), _threshold_voltage(threshold_voltage){

	}
	float voltage(){
		return adc.read()*3.3*(10+20)/10;
	}
	bool check(){
		return (voltage() > _threshold_voltage);
	}
private:
	AnalogIn adc;
	float _threshold_voltage;
};



#endif /* BATTERY_H_ */
