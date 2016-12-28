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

#define MPU6500_ACCEL_FACTOR		2048.0f
#define MPU6500_GYRO_FACTOR			16.3835f

#define MPU6500_UPDATE_PERIOD_US	1000

class MPU6500: private SPI {
public:
	MPU6500(PinName mosi_pin, PinName miso_pin, PinName sclk_pin, PinName cs_pin) :
			SPI(mosi_pin, miso_pin, sclk_pin), cs(cs_pin, 1),
					updateThread(PRIORITY_MPU6500_UPDATE, STACK_SIZE_MPU6500_UPDATE) {
		setup();
		updateThread.start(this, &MPU6500::updateTask);
		DBG("0x%08X: MPU6500\n", (unsigned int ) updateThread.gettid());
		updateTicker.attach_us(this, &MPU6500::updateIsr, MPU6500_UPDATE_PERIOD_US);
	}
	struct Parameter {
		float x;
		float y;
		float z;
		Parameter(float x = 0, float y = 0, float z = 0) :
				x(x), y(y), z(z) {
		}
		inline Parameter operator+(const Parameter& obj) const {
			return Parameter(x + obj.x, y + obj.y, z + obj.z);
		}
		inline Parameter operator*(const float mul) const {
			return Parameter(x * mul, y * mul, z * mul);
		}
		inline Parameter operator/(const float div) const {
			return Parameter(x / div, y / div, z / div);
		}
		inline const Parameter& operator+=(const Parameter& obj) {
			x += obj.x;
			y += obj.y;
			z += obj.z;
			return *this;
		}
		inline const Parameter& operator/=(const float& div) {
			x /= div;
			y /= div;
			z /= div;
			return *this;
		}
	};
	Parameter accel;
	Parameter gyro;
	Parameter angle;
	Parameter velocity;
	void calibration() {
		updateTicker.detach();
		angle = Parameter();
		velocity = Parameter();
		Parameter accel_sum, gyro_sum;
		const int ave_count = 500;
		for (int i = 0; i < ave_count; i++) {
			readAll();
			accel_sum += accel;
			gyro_sum += gyro;
			Thread::wait(1);
		}
		accel_sum /= ave_count;
		gyro_sum /= ave_count;
		accel_offset += accel_sum;
		gyro_offset += gyro_sum;
		DBG("MPU6500 Calibration: %.3f\t%.3f\n", gyro_offset.z, accel_offset.y);
		updateTicker.attach_us(this, &MPU6500::updateIsr, MPU6500_UPDATE_PERIOD_US);
		angle = Parameter();
		velocity = Parameter();
	}
private:
	DigitalOut cs;
	Thread updateThread;
	Ticker updateTicker;
	Parameter accel_offset;
	Parameter gyro_offset;
	Parameter accel_prev;
	Parameter gyro_prev;

	void updateIsr() {
		updateThread.signal_set(0x01);
	}
	void updateTask() {
		while (1) {
			Thread::signal_wait(0x01);
			readAll();
			velocity += (accel + accel_prev) / 2 * MPU6500_UPDATE_PERIOD_US / 1000000;
			accel_prev = accel;
			angle += (gyro + gyro_prev) / 2 * MPU6500_UPDATE_PERIOD_US / 1000000;
			gyro_prev = gyro;
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
	void readAll() {
		union {
			int16_t i;
			struct {
				uint8_t l :8;
				uint8_t h :8;
			};
		} bond;
		const int readBytes = 14;
		unsigned char rx[readBytes];
		cs = 0;
		this->write(0x3B | 0x80);
		for (int i = 0; i < readBytes; i++) {
			rx[i] = this->write(0x00);
		}
		cs = 1;
		bond.h = rx[0];
		bond.l = rx[1];
		accel.x = bond.i / MPU6500_ACCEL_FACTOR * 1000 * 9.80665 - accel_offset.x;
		bond.h = rx[2];
		bond.l = rx[3];
		accel.y = bond.i / MPU6500_ACCEL_FACTOR * 1000 * 9.80665 - accel_offset.y;
		bond.h = rx[4];
		bond.l = rx[5];
		accel.z = bond.i / MPU6500_ACCEL_FACTOR * 1000 * 9.80665 - accel_offset.z;

		bond.h = rx[8];
		bond.l = rx[9];
		gyro.x = bond.i / MPU6500_GYRO_FACTOR * M_PI / 180 - gyro_offset.x;
		bond.h = rx[10];
		bond.l = rx[11];
		gyro.y = bond.i / MPU6500_GYRO_FACTOR * M_PI / 180 - gyro_offset.y;
		bond.h = rx[12];
		bond.l = rx[13];
		gyro.z = bond.i / MPU6500_GYRO_FACTOR * M_PI / 180 - gyro_offset.z;
	}
};

#endif /* MPU6500_H_ */
