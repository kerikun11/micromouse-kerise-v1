/*
 * Interface.h
 *
 *  Created on: 2016/10/23
 *      Author: kerikun11
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "mbed.h"

class Buzzer {
public:
	Buzzer(PinName pin) :
			out(pin) {
	}
	void setFrequency(uint32_t freq) {
		out.period_us(1000000 / freq);
		out.pulsewidth_us(1000000 / freq / 10);
	}
	void mute() {
		out.pulsewidth_us(0);
	}
	void playBoot() {
		playC6();
		t1.attach_us(this, &Buzzer::playD6, 100000);
		t2.attach_us(this, &Buzzer::playC6, 200000);
		t8.attach_us(this, &Buzzer::mute, 300000);
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
private:
	PwmOut out;
	Timeout t1, t2, t3, t4, t5, t6, t7, t8;
	void playC4() {
		setFrequency(262);
	}
	void playD4() {
		setFrequency(294);
	}
	void playE4() {
		setFrequency(330);
	}
	void playF4() {
		setFrequency(349);
	}
	void playG4() {
		setFrequency(392);
	}
	void playA4() {
		setFrequency(440);
	}
	void playB4() {
		setFrequency(494);
	}
	void playC5() {
		setFrequency(523);
	}
	void playD5() {
		setFrequency(587);
	}
	void playE5() {
		setFrequency(659);
	}
	void playF5() {
		setFrequency(698);
	}
	void playG5() {
		setFrequency(784);
	}
	void playA5() {
		setFrequency(880);
	}
	void playB5() {
		setFrequency(988);
	}
	void playC6() {
		setFrequency(1047);
	}
	void playD6() {
		setFrequency(1175);
	}
	void playE6() {
		setFrequency(1319);
	}
	void playF6() {
		setFrequency(1397);
	}
	void playG6() {
		setFrequency(1568);
	}
	void playA6() {
		setFrequency(1760);
	}
	void playB6() {
		setFrequency(1976);
	}
	void playC7() {
		setFrequency(2093);
	}
};

#define PRESS_LEVEL 2
#define LONG_PRESS_LEVEL1  50
#define LONG_PRESS_LEVEL2  200
#define LONG_PRESS_LEVEL3  1000

class Button {
public:
	/** Constructor.
	 @param button_pin ボタンを取り付けるピン
	 @param pressed_state 正論理か負論理かを設定．
	 @param pin_mode プルアップなど
	 */
	Button(PinName button_pin, int pressed_state = 0, PinMode pin_mode = PullUp) :
			pin(button_pin, pin_mode), pressedState(pressed_state) {
		flags = 0x00;
	}

	~Button() {
		ticker.detach();
	}

	/** ボタンの状態フラグ.
	 状態を読み取ったらフラグをクリアすること．
	 */
	union {
		volatile uint8_t flags; /**< フラグにまとめてアクセス */
		struct {
			volatile uint8_t pressed :1; /**< ボタンが押された */
			volatile uint8_t long_pressed_1 :1; /**< ボタンが長押しされた */
			volatile uint8_t long_pressed_2 :1; /**< ボタンがもっと長押しされた */
			volatile uint8_t long_pressed_3 :1; /**< ボタンがもっともっと長押しされた */
			volatile uint8_t pressing :1; /**< ボタンが押されている */
			volatile uint8_t long_pressing_1 :1; /**< ボタンが長押しされている */
			volatile uint8_t long_pressing_2 :1; /**< ボタンがもっと長押しされている */
			volatile uint8_t long_pressing_3 :1; /**< ボタンがもっともっと長押しされている */
		};
	};

	/** ボタン初期化関数.
	 タイマー割り込みをアタッチする．始めに必ず実行すること．
	 */
	void init() {
		ticker.attach_us(this, &Button::timerIsr, 10000);
	}
private:
	DigitalIn pin;
	int pressedState;
	Ticker ticker;
	volatile int counter;

	void timerIsr() {
		if (pin == pressedState) {
			if (counter < LONG_PRESS_LEVEL3 + 1)
				counter++;
			if (counter == LONG_PRESS_LEVEL3)
				long_pressing_3 = 1;
			if (counter == LONG_PRESS_LEVEL2)
				long_pressing_2 = 1;
			if (counter == LONG_PRESS_LEVEL1)
				long_pressing_1 = 1;
			if (counter == PRESS_LEVEL)
				pressing = 1;
		} else {
			if (counter >= LONG_PRESS_LEVEL3)
				long_pressed_3 = 1;
			else if (counter >= LONG_PRESS_LEVEL2)
				long_pressed_2 = 1;
			else if (counter >= LONG_PRESS_LEVEL1)
				long_pressed_1 = 1;
			else if (counter >= PRESS_LEVEL)
				pressed = 1;
			counter = 0;
			flags &= 0x0F;
		}
	}
};

#endif /* INTERFACE_H_ */
