/*
 * Buzzer.h
 *
 *  Created on: 2016/10/26
 *      Author: kerikun11
 */

#ifndef BUZZER_H_
#define BUZZER_H_

enum BUZZER_MUSIC {
	BUZZER_MUSIC_BOOT,
	BUZZER_MUSIC_LOW_BATTERY,
	BUZZER_MUSIC_CONFIRM,
	BUZZER_MUSIC_CANCEL,
	BUZZER_MUSIC_SELECT,
	BUZZER_MUSIC_EMERGENCY,
	BUZZER_MUSIC_DOREMI,
};

class Buzzer {
public:
	Buzzer(PinName pin) :
			out(pin) {
		thread.start(this, &Buzzer::task);
	}
	void play(enum BUZZER_MUSIC new_music) {
		queue.put((enum BUZZER_MUSIC *) new_music);
	}
private:
	Thread thread;
	Queue<enum BUZZER_MUSIC, 2> queue;
	PwmOut out;

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
			if (evt.status == osEventMessage) {
				enum BUZZER_MUSIC music = (enum BUZZER_MUSIC) evt.value.v;
				switch (music) {
				case BUZZER_MUSIC_BOOT:
					playC(2);
					Thread::wait(100);
					playE(2);
					Thread::wait(100);
					playG(2);
					Thread::wait(100);
					break;
				case BUZZER_MUSIC_LOW_BATTERY:
					playC(4);
					Thread::wait(100);
					mute();
					Thread::wait(100);
					playC(4);
					Thread::wait(600);
					break;
				case BUZZER_MUSIC_CONFIRM:
					playC(2);
					Thread::wait(100);
					playE(2);
					Thread::wait(100);
					break;
				case BUZZER_MUSIC_CANCEL:
					playE(2);
					Thread::wait(100);
					playC(2);
					Thread::wait(100);
					break;
				case BUZZER_MUSIC_SELECT:
					playC(2);
					Thread::wait(100);
					break;
				case BUZZER_MUSIC_EMERGENCY:
					for (int i = 0; i < 4; i++) {
						playC(4);
						Thread::wait(100);
						playE(4);
						Thread::wait(100);
					}
					break;
				case BUZZER_MUSIC_DOREMI:
					playC(2);
					Thread::wait(500);
					playD(2);
					Thread::wait(500);
					playE(2);
					Thread::wait(500);
					playF(2);
					Thread::wait(500);
					playG(2);
					Thread::wait(500);
					playA(2);
					Thread::wait(500);
					playB(2);
					Thread::wait(500);
					playC(3);
					Thread::wait(500);
					playD(3);
					Thread::wait(500);
					playE(3);
					Thread::wait(500);
					playF(3);
					Thread::wait(500);
					playG(3);
					Thread::wait(500);
					playA(3);
					Thread::wait(500);
					playB(3);
					Thread::wait(500);
					playC(4);
					Thread::wait(500);
					break;
				}
				mute();
			}
		}
	}
};

#endif /* BUZZER_H_ */
