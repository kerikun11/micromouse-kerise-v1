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

#define ENCODER_L_TIMx	TIM3
#define ENCODER_R_TIMx	TIM4

class Encoder {
public:
	Encoder(TIM_TypeDef *TIMx) {
		EncoderInit(TIMx, 0xffff, TIM_ENCODERMODE_TI12);
		ticker.attach_us(this, &Encoder::update, 10000);
		prev_count = 0;
		overflow_count = 0;
	}
	int32_t value() {
		update();
		return overflow_count * 65536 + getRawCount();
	}
private:
	TIM_Encoder_InitTypeDef encoder;
	TIM_HandleTypeDef timer;
	Ticker ticker;
	volatile int32_t overflow_count;
	volatile int16_t prev_count;

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
	void EncoderInit(TIM_TypeDef * TIMx, uint32_t maxcount, uint32_t encmode) {
		GPIO_InitTypeDef GPIO_InitStruct;
		if (TIMx == TIM1) { //PA8 PA9 = Nucleo D7 D8
			__TIM1_CLK_ENABLE();
			__GPIOA_CLK_ENABLE();
			GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
			GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		} else if (TIMx == TIM2) { //PA0 PA1 = Nucleo A0 A1
			__TIM2_CLK_ENABLE();
			__GPIOA_CLK_ENABLE();
			GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
			GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		} else if (TIMx == TIM3) { //PB4 PB5 = Nucleo D5 D4
			__TIM3_CLK_ENABLE();
			__GPIOB_CLK_ENABLE();
			GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
			GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		} else if (TIMx == TIM4) { // PB6 PB7 = Nucleo D10 MORPHO_PB7
			__TIM4_CLK_ENABLE();
			__GPIOB_CLK_ENABLE();
			GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
			GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		}

		timer.Instance = TIMx;
		timer.Instance->CNT = 0;
		timer.Init.Period = maxcount;
		timer.Init.CounterMode = TIM_COUNTERMODE_UP;
		timer.Init.Prescaler = 0;
		timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

		encoder.EncoderMode = encmode;

		encoder.IC1Filter = 0x0F;
		encoder.IC1Polarity = TIM_INPUTCHANNELPOLARITY_RISING;
		encoder.IC1Prescaler = TIM_ICPSC_DIV4;
		encoder.IC1Selection = TIM_ICSELECTION_DIRECTTI;

		encoder.IC2Filter = 0x0F;
		encoder.IC2Polarity = TIM_INPUTCHANNELPOLARITY_FALLING;
		encoder.IC2Prescaler = TIM_ICPSC_DIV4;
		encoder.IC2Selection = TIM_ICSELECTION_DIRECTTI;

		if (HAL_TIM_Encoder_Init(&timer, &encoder) != HAL_OK) {
			printf("Couldn't Init Encoder\r\n");
			while (1) {
			}
		}

		if (HAL_TIM_Encoder_Start(&timer, TIM_CHANNEL_1) != HAL_OK) {
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
