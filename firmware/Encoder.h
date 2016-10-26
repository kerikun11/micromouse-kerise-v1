/*
 * Encoder.h
 *
 *  Created on: 2016/10/23
 *      Author: kerikun11
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#define MOTOR_CH_L	0
#define MOTOR_CH_R	1

class Encoder {
public:
	Encoder(TIM_TypeDef *TIMx) {
		EncoderInit(&encoder, &timer, TIMx, 0xffff, TIM_ENCODERMODE_TI12);
	}
	void init() {
		ticker.attach_us(this, &Encoder::update, 10000);
	}
	int32_t value() {
		update();
		return overflow_count * 65536 + getRawCount();
	}
private:
	TIM_Encoder_InitTypeDef encoder;
	TIM_HandleTypeDef timer;
	Ticker ticker;
	int32_t overflow_count;
	int16_t prev_count;

	int16_t getRawCount() {
		return __HAL_TIM_GET_COUNTER(&timer);
	}
	void update() {
		int16_t now_count = getRawCount();
		if (now_count > prev_count + 20000)
			overflow_count--;
		if (now_count < prev_count - 20000)
			overflow_count++;
		prev_count = now_count;
	}
	void EncoderInit(TIM_Encoder_InitTypeDef * encoder,
			TIM_HandleTypeDef * timer, TIM_TypeDef * TIMx, uint32_t maxcount,
			uint32_t encmode) {

		timer->Instance = TIMx;
		timer->Init.Period = maxcount;
		timer->Init.CounterMode = TIM_COUNTERMODE_UP;
		timer->Init.Prescaler = 0;
		timer->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

		encoder->EncoderMode = encmode;

		encoder->IC1Filter = 0x0F;
		encoder->IC1Polarity = TIM_INPUTCHANNELPOLARITY_RISING;
		encoder->IC1Prescaler = TIM_ICPSC_DIV4;
		encoder->IC1Selection = TIM_ICSELECTION_DIRECTTI;

		encoder->IC2Filter = 0x0F;
		encoder->IC2Polarity = TIM_INPUTCHANNELPOLARITY_FALLING;
		encoder->IC2Prescaler = TIM_ICPSC_DIV4;
		encoder->IC2Selection = TIM_ICSELECTION_DIRECTTI;

		if (HAL_TIM_Encoder_Init(timer, encoder) != HAL_OK) {
			printf("Couldn't Init Encoder\r\n");
			while (1) {
			}
		}

		if (HAL_TIM_Encoder_Start(timer, TIM_CHANNEL_1) != HAL_OK) {
			printf("Couldn't Start Encoder\r\n");
			while (1) {
			}
		}
	}
};

class Encoders {
public:
	Encoders(TIM_TypeDef *TIML, TIM_TypeDef *TIMR) :
			encoderL(TIML), encoderR(TIMR) {
	}
	int32_t left() {
		return -encoderL.value();
	}
	int32_t right() {
		return encoderR.value();
	}
	int32_t value(uint8_t ch) {
		switch (ch) {
		case 0:
			return left();
		case 1:
			return right();
		default:
			return 0;
		}
	}
private:
	Encoder encoderL;
	Encoder encoderR;
};

#endif /* ENCODER_H_ */