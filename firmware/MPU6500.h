/*
 * MPU6500.h
 *
 *  Created on: 2016/10/23
 *      Author: kerikun11
 */

#ifndef MPU6500_H_
#define MPU6500_H_

#include "mbed.h"

#define MPU6500_UPDATE_PERIOD_US	1000

class MPU6500: private SPI {
public:
	MPU6500(PinName mosi_pin, PinName miso_pin, PinName sclk_pin,
			PinName cs_pin) :
			SPI(mosi_pin, miso_pin, sclk_pin), cs(cs_pin, 1) {
		setup();
		ticker.attach_us(this, &MPU6500::update, MPU6500_UPDATE_PERIOD_US);
	}
	float ay = 0;
	float gz = 0;
	float angle = 0;
	float int_angle = 0;

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
private:
	DigitalOut cs;
	Ticker ticker;
	Thread thread;

	void setup() {
		this->writeReg(0x19, 0x07);	// samplerate
		this->writeReg(0x1b, 0x18); // +-2000dps
//		this->writeReg(0x1b, 0x10); // +-1000dps
//		this->writeReg(0x1b, 0x08); // +-500dps
//		this->writeReg(0x1b, 0x00); // +-250dps
		this->writeReg(0x1c, 0x18); // +-16g
//		this->writeReg(0x1c, 0x10); // +-8g
//		this->writeReg(0x1c, 0x08); // +-4g
//		this->writeReg(0x1c, 0x00); // +-2g
		this->writeReg(0x6b, 0x80);	// reset
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
	void update() {
		ay = readAccY();
		gz = (readGyrZ() * 16.4f - 1200.0) / 2000.0;
		angle += gz * MPU6500_UPDATE_PERIOD_US / 1000000;
		int_angle += angle * MPU6500_UPDATE_PERIOD_US / 1000000;
	}
};

#endif /* MPU6500_H_ */
