/*
 * Buzzer.h
 *
 *  Created on: 2016/10/26
 *      Author: kerikun11
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include "mbed.h"
#include "config.h"

class Buzzer {
public:
	Buzzer(PinName pin) :
			out(pin), thread(PRIORITY_BUZZER, STACK_SIZE_BUZZER) {
		thread.start(this, &Buzzer::task);
	}
	enum BUZZER_MUSIC {
		BOOT,
		LOW_BATTERY,
		CONFIRM,
		CANCEL,
		SELECT,
		ERROR,
		START,
		EMERGENCY,
		COMPLETE,
	};
	void play(enum BUZZER_MUSIC new_music) {
		queue.put((enum BUZZER_MUSIC *) new_music);
	}
private:
	PwmOut out;
	Thread thread;
	Queue<enum BUZZER_MUSIC, 4> queue;

	void setFrequency(uint32_t freq) {
		out.period_us(1000000 / freq);
		out.pulsewidth_us(1000000 / freq / 10);
	}
	void mute() {
		out.pulsewidth_us(0);
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
	void task() {
		while (1) {
			osEvent evt = queue.get();
			if (evt.status != osEventMessage) {
			}
			enum BUZZER_MUSIC music = (enum BUZZER_MUSIC) evt.value.v;
			switch (music) {
			case BOOT:
				playC(2);
				Thread::wait(100);
				playE(2);
				Thread::wait(100);
				playG(2);
				Thread::wait(100);
				mute();
				Thread::wait(100);
				break;
			case LOW_BATTERY:
				playC(4);
				Thread::wait(100);
				mute();
				Thread::wait(100);
				playC(4);
				Thread::wait(600);
				mute();
				Thread::wait(100);
				break;
			case CONFIRM:
				playC(2);
				Thread::wait(100);
				playE(2);
				Thread::wait(100);
				mute();
				Thread::wait(100);
				break;
			case CANCEL:
				playE(2);
				Thread::wait(100);
				playC(2);
				Thread::wait(100);
				mute();
				Thread::wait(100);
				break;
			case SELECT:
				playC(2);
				Thread::wait(100);
				mute();
				Thread::wait(100);
				break;
			case ERROR:
				for (int i = 0; i < 6; i++) {
					playC(3);
					Thread::wait(100);
					playE(3);
					Thread::wait(100);
				}
				mute();
				Thread::wait(100);
				break;
			case START:
				playC(2);
				Thread::wait(200);
				playE(2);
				Thread::wait(200);
				playG(2);
				Thread::wait(200);
				mute();
				Thread::wait(200);
				break;
			case EMERGENCY:
//					for (int i = 0; i < 4; i++) {
//						playC(4);
//						Thread::wait(100);
//						playE(4);
//						Thread::wait(100);
//					}
				playC(2);
				Thread::wait(100);
				playF(2);
				Thread::wait(100);
				mute();
				Thread::wait(100);
				playF(2);
				Thread::wait(75);
				mute();
				Thread::wait(25);

				playF(2);
				Thread::wait(167);
				playE(2);
				Thread::wait(167);
				playD(2);
				Thread::wait(167);
				playC(2);
				Thread::wait(200);
				mute();
				Thread::wait(100);
				break;
			case COMPLETE:
				playC(3);
				Thread::wait(100);
				playE(3);
				Thread::wait(100);
				playC(3);
				Thread::wait(100);
				playE(3);
				Thread::wait(100);
				mute();
				Thread::wait(100);
				break;
			}
			mute();
		}
	}
};

#endif /* BUZZER_H_ */
