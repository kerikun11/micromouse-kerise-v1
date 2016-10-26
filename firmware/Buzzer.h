/*
 * Buzzer.h
 *
 *  Created on: 2016/10/26
 *      Author: kerikun11
 */

#ifndef BUZZER_H_
#define BUZZER_H_

class Buzzer {
public:
	Buzzer(PinName pin) :
			out(pin) {
	}
	void mute() {
		out.pulsewidth_us(0);
	}
	void playBoot() {
		playC6();
		t1.attach_us(this, &Buzzer::playE6, 100000);
		t2.attach_us(this, &Buzzer::playG6, 200000);
		t3.attach_us(this, &Buzzer::mute, 300000);
	}
	void playLowBattery() {
		playC7();
		t1.attach_us(this, &Buzzer::mute, 500000);
	}
	void playConfirm() {
		playC6();
		t1.attach_us(this, &Buzzer::playE6, 50000);
		t2.attach_us(this, &Buzzer::mute, 100000);
	}
	void playCancel() {
		playC6();
		t1.attach_us(this, &Buzzer::playA5, 50000);
		t2.attach_us(this, &Buzzer::mute, 100000);
	}
	void playSelect() {
		playC6();
		t2.attach_us(this, &Buzzer::mute, 100000);
	}
	void playDoremi() {
		playC6();
		t1.attach_us(this, &Buzzer::playD6, 500000);
		t2.attach_us(this, &Buzzer::playE6, 1000000);
		t3.attach_us(this, &Buzzer::playF6, 1500000);
		t4.attach_us(this, &Buzzer::playG6, 2000000);
		t5.attach_us(this, &Buzzer::playA6, 2500000);
		t6.attach_us(this, &Buzzer::playB6, 3000000);
		t7.attach_us(this, &Buzzer::playC7, 3500000);
		t8.attach_us(this, &Buzzer::mute, 4000000);
	}
private:
	PwmOut out;
	Timeout t1, t2, t3, t4, t5, t6, t7, t8;

	void setFrequency(uint32_t freq) {
		out.period_us(1000000 / freq);
		out.pulsewidth_us(1000000 / freq / 100);
	}
	void playC(uint8_t octave) {
		setFrequency(440 * pow(2, octave) * 3 / 5);
	}
	void playD(uint8_t octave) {
		setFrequency(440 * pow(2, octave) * 27 / 40);
	}
	void playE(uint8_t octave) {
		setFrequency(440 * pow(2, octave) * 3 / 4);
	}
	void playF(uint8_t octave) {
		setFrequency(440 * pow(2, octave) * 4 / 5);
	}
	void playG(uint8_t octave) {
		setFrequency(440 * pow(2, octave) * 9 / 10);
	}
	void playA(uint8_t octave) {
		setFrequency(440 * pow(2, octave));
	}
	void playB(uint8_t octave) {
		setFrequency(440 * pow(2, octave) * 9 / 8);
	}

	void playC5() {
		playC(1);
	}
	void playD5() {
		playD(1);
	}
	void playE5() {
		playE(1);
	}
	void playF5() {
		playF(1);
	}
	void playG5() {
		playG(1);
	}
	void playA5() {
		playA(1);
	}
	void playB5() {
		playB(1);
	}
	void playC6() {
		playC(2);
	}
	void playD6() {
		playD(2);
	}
	void playE6() {
		playE(2);
	}
	void playF6() {
		playF(2);
	}
	void playG6() {
		playG(2);
	}
	void playA6() {
		playA(2);
	}
	void playB6() {
		playB(2);
	}
	void playC7() {
		playC(3);
	}
};

#endif /* BUZZER_H_ */
