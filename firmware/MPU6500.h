/*
 * MPU6500.h
 *
 *  Created on: 2016/10/23
 *      Author: kerikun11
 */

#ifndef MPU6500_H_
#define MPU6500_H_

#include "mbed.h"
#include "config.h"

#define MPU6500_GYRO_FACTOR			16.3835f

#define MPU6500_ACCEL_FACTOR		2048.0f
#define MPU6500_ACCEL_OFFSET		0

#define MPU6500_UPDATE_PERIOD_US	1000

class MPU6500: private SPI {
public:
	MPU6500(PinName mosi_pin, PinName miso_pin, PinName sclk_pin, PinName cs_pin) :
			SPI(mosi_pin, miso_pin, sclk_pin), cs(cs_pin, 1), updateThread(PRIORITY_MPU6500_UPDATE) {
		setup();
		updateThread.start(this, &MPU6500::updateTask);
		updateTicker.attach_us(this, &MPU6500::updateIsr,
		MPU6500_UPDATE_PERIOD_US);
		_accelY = 0;
		_gyroZ = 0;
		_angleZ = 0;
		_offset_gyroZ = 0.6f;
	}
	double accelY() {
		return _accelY;
	}
	double gyroZ() {
		return _gyroZ;
	}
	double angleZ() {
		return _angleZ;
	}
	void calibration() {
		double sum = 0;
		const int ave_count = 100;
		for (int i = 0; i < ave_count; i++) {
			sum += gyroZ();
			Thread::wait(1);
		}
		sum /= ave_count;
		_offset_gyroZ += sum;
	}
private:
	DigitalOut cs;
	Thread updateThread;
	Ticker updateTicker;
	volatile double _accelY;
	volatile double _gyroZ;
	volatile double _angleZ;
	double _offset_gyroZ;

	void updateIsr() {
		updateThread.signal_set(0x01);
	}
	void updateTask() {
		while (1) {
			Thread::signal_wait(0x01);
			_accelY = readAccY() / MPU6500_ACCEL_FACTOR;
			_gyroZ = readGyrZ() / MPU6500_GYRO_FACTOR - _offset_gyroZ;
			_angleZ += _gyroZ * MPU6500_UPDATE_PERIOD_US / 1000000;
		}
	}
	void setup() {
//		this->writeReg(0x6b, 0x80);	// reset
//		Thread::wait(100);
		this->writeReg(0x19, 0x07);	// samplerate
		this->writeReg(0x1b, 0x18); // +-2000dps
//		this->writeReg(0x1b, 0x10); // +-1000dps
//		this->writeReg(0x1b, 0x08); // +-500dps
//		this->writeReg(0x1b, 0x00); // +-250dps
		this->writeReg(0x1c, 0x18); // +-16g
//		this->writeReg(0x1c, 0x10); // +-8g
//		this->writeReg(0x1c, 0x08); // +-4g
//		this->writeReg(0x1c, 0x00); // +-2g
	}
	bool readReg(uint8_t reg, uint8_t& byte) {
		uint8_t data;
		data = reg | 0x80;
		cs = 0;
		this->write(data);
		byte = this->write(0x00);
		cs = 1;
		return true;
	}
	bool writeReg(uint8_t reg, uint8_t val) {
		cs = 0;
		this->write(reg);
		this->write(val);
		cs = 1;
		return true;
	}
	inline int16_t readInt16(uint8_t addr) {
		union {
			uint16_t u;
			int16_t i;
		} _u2i;
		addr |= 0x80;
		unsigned char rx[2];
		cs = 0;
		this->write(addr);
		rx[0] = this->write(0x00);
		rx[1] = this->write(0x00);
		cs = 1;
		_u2i.u = ((uint16_t)(rx[0]) << 8) | rx[1];
		return _u2i.i;
	}
	inline int16_t readAccX() {
		return readInt16(0x3b);
	}
	inline int16_t readAccY() {
		return readInt16(0x3d);
	}
	inline int16_t readAccZ() {
		return readInt16(0x3f);
	}
	inline int16_t readTemp() {
		return readInt16(0x41);
	}
	inline int16_t readGyrX() {
		return readInt16(0x43);
	}
	inline int16_t readGyrY() {
		return readInt16(0x45);
	}
	inline int16_t readGyrZ() {
		return readInt16(0x47);
	}
	void readGyrXYZT(int16_t &x, int16_t &y, int16_t &z, int16_t &t) {
		union {
			uint16_t u;
			int16_t i;
		} _u2i;
		uint8_t addr = 0x41 | 0x80;
		unsigned char rx[8];
		cs = 0;
		this->write(addr);
		for (int i = 0; i < 8; i++) {
			rx[i] = this->write(0x00);
		}
		cs = 1;
		_u2i.u = ((uint16_t)(rx[0]) << 8) | rx[1];
		t = _u2i.i;
		_u2i.u = ((uint16_t)(rx[2]) << 8) | rx[3];
		x = _u2i.i;
		_u2i.u = ((uint16_t)(rx[4]) << 8) | rx[5];
		y = _u2i.i;
		_u2i.u = ((uint16_t)(rx[6]) << 8) | rx[7];
		z = _u2i.i;
	}
};

#endif /* MPU6500_H_ */
