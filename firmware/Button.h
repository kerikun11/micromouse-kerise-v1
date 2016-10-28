/*
 * Button.h
 *
 *  Created on: 2016/10/26
 *      Author: kerikun11
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#define BUTTON_PRESS_LEVEL			2
#define BUTTON_LONG_PRESS_LEVEL_1	40
#define BUTTON_LONG_PRESS_LEVEL_2	200
#define BUTTON_LONG_PRESS_LEVEL_3	1000

#define BUTTON_SAMPLING_PERIOD_US	10000

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
		ticker.attach_us(this, &Button::timerIsr, BUTTON_SAMPLING_PERIOD_US);
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
private:
	DigitalIn pin;
	const int pressedState;
	Ticker ticker;
	volatile int counter;

	void timerIsr() {
		if (pin == pressedState) {
			if (counter < BUTTON_LONG_PRESS_LEVEL_3 + 1)
				counter++;
			if (counter == BUTTON_LONG_PRESS_LEVEL_3)
				long_pressing_3 = 1;
			if (counter == BUTTON_LONG_PRESS_LEVEL_2)
				long_pressing_2 = 1;
			if (counter == BUTTON_LONG_PRESS_LEVEL_1)
				long_pressing_1 = 1;
			if (counter == BUTTON_PRESS_LEVEL)
				pressing = 1;
		} else {
			if (counter >= BUTTON_LONG_PRESS_LEVEL_3)
				long_pressed_3 = 1;
			else if (counter >= BUTTON_LONG_PRESS_LEVEL_2)
				long_pressed_2 = 1;
			else if (counter >= BUTTON_LONG_PRESS_LEVEL_1)
				long_pressed_1 = 1;
			else if (counter >= BUTTON_PRESS_LEVEL)
				pressed = 1;
			counter = 0;
			flags &= 0x0F;
		}
	}
};

#endif /* BUTTON_H_ */
