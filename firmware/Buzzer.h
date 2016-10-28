/*
 * Buzzer.h
 *
 *  Created on: 2016/10/26
 *      Author: kerikun11
 */

#ifndef BUZZER_H_
#define BUZZER_H_

enum BUZZER_MUSIC {
	BUZZER_MUSIC_MUTE,
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
			out(pin), _thread(&Buzzer::threadStarter, this, osPriorityNormal,
					1024) {
	}
	void play(enum BUZZER_MUSIC new_music) {
		music = new_music;
	}
private:
	PwmOut out;
	enum BUZZER_MUSIC music;

	Thread _thread;
	static void threadStarter(void const *p) {
		Buzzer *instance = (Buzzer*) p;
		instance->threadWorker();
	}
	void threadWorker() {
		while (1) {
			switch (music) {
			case BUZZER_MUSIC_MUTE:
				break;
			case BUZZER_MUSIC_BOOT:
				music = BUZZER_MUSIC_MUTE;
				playC(2);
				Thread::wait(100);
				playE(2);
				Thread::wait(100);
				playG(2);
				Thread::wait(100);
				mute();
				break;
			case BUZZER_MUSIC_LOW_BATTERY:
				music = BUZZER_MUSIC_MUTE;
				playC(4);
				Thread::wait(100);
				mute();
				Thread::wait(100);
				playC(4);
				Thread::wait(600);
				mute();
				break;
			case BUZZER_MUSIC_CONFIRM:
				music = BUZZER_MUSIC_MUTE;
				playC(2);
				Thread::wait(100);
				playE(2);
				Thread::wait(100);
				mute();
				break;
			case BUZZER_MUSIC_CANCEL:
				music = BUZZER_MUSIC_MUTE;
				playE(2);
				Thread::wait(100);
				playC(2);
				Thread::wait(100);
				mute();
				break;
			case BUZZER_MUSIC_SELECT:
				music = BUZZER_MUSIC_MUTE;
				playC(2);
				Thread::wait(100);
				mute();
				break;
			case BUZZER_MUSIC_EMERGENCY:
				playC(4);
				Thread::wait(100);
				playE(4);
				Thread::wait(100);
				playC(4);
				Thread::wait(100);
				playE(4);
				Thread::wait(100);
				playC(4);
				Thread::wait(100);
				playE(4);
				Thread::wait(100);
				playC(4);
				Thread::wait(100);
				playE(4);
				Thread::wait(100);
				mute();
				music = BUZZER_MUSIC_MUTE;
				break;
			case BUZZER_MUSIC_DOREMI:
				music = BUZZER_MUSIC_MUTE;
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
				mute();
				break;
			}
		}
	}

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

};

/*
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
 private:
 PwmOut out;
 Timeout t1, t2, t3, t4, t5, t6, t7, t8;

 void setFrequency(uint32_t freq) {
 out.period_us(1000000 / freq);
 out.pulsewidth_us(1000000 / freq / 10);
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
 */

#endif /* BUZZER_H_ */
