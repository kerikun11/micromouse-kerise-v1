/*
 * Motor.h
 *
 *  Created on: 2016/10/23
 *      Author: kerikun11
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#define TIMx						TIM2
#define TIMx_CLK_ENABLE()			__HAL_RCC_TIM2_CLK_ENABLE()

#define TIMx_CHANNEL_GPIO_PORT()	__HAL_RCC_GPIOC_CLK_ENABLE()
#define TIMx_GPIO_PORT_CHANNEL1		GPIOA
#define TIMx_GPIO_PORT_CHANNEL2		GPIOA
#define TIMx_GPIO_PORT_CHANNEL3		GPIOA
#define TIMx_GPIO_PORT_CHANNEL4		GPIOA
#define TIMx_GPIO_PIN_CHANNEL1		GPIO_PIN_0
#define TIMx_GPIO_PIN_CHANNEL2		GPIO_PIN_1
#define TIMx_GPIO_PIN_CHANNEL3		GPIO_PIN_2
#define TIMx_GPIO_PIN_CHANNEL4		GPIO_PIN_3
#define TIMx_GPIO_AF_CHANNEL1		GPIO_AF1_TIM2
#define TIMx_GPIO_AF_CHANNEL2		GPIO_AF1_TIM2
#define TIMx_GPIO_AF_CHANNEL3		GPIO_AF1_TIM2
#define TIMx_GPIO_AF_CHANNEL4		GPIO_AF1_TIM2

#define  PERIOD_VALUE				(1000 - 1)

class Motor {
public:
	Motor() {
	}
	void init() {
		GPIO_InitTypeDef GPIO_InitStruct;
		TIMx_CLK_ENABLE();
		TIMx_CHANNEL_GPIO_PORT();

		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

		GPIO_InitStruct.Alternate = TIMx_GPIO_AF_CHANNEL1;
		GPIO_InitStruct.Pin = TIMx_GPIO_PIN_CHANNEL1;
		HAL_GPIO_Init(TIMx_GPIO_PORT_CHANNEL1, &GPIO_InitStruct);

		GPIO_InitStruct.Alternate = TIMx_GPIO_AF_CHANNEL2;
		GPIO_InitStruct.Pin = TIMx_GPIO_PIN_CHANNEL2;
		HAL_GPIO_Init(TIMx_GPIO_PORT_CHANNEL2, &GPIO_InitStruct);

		GPIO_InitStruct.Alternate = TIMx_GPIO_AF_CHANNEL3;
		GPIO_InitStruct.Pin = TIMx_GPIO_PIN_CHANNEL3;
		HAL_GPIO_Init(TIMx_GPIO_PORT_CHANNEL3, &GPIO_InitStruct);

		GPIO_InitStruct.Alternate = TIMx_GPIO_AF_CHANNEL4;
		GPIO_InitStruct.Pin = TIMx_GPIO_PIN_CHANNEL4;
		HAL_GPIO_Init(TIMx_GPIO_PORT_CHANNEL4, &GPIO_InitStruct);

		TimHandle.Instance = TIMx;
		TimHandle.Init.Prescaler = 0;
		TimHandle.Init.Period = PERIOD_VALUE;
		TimHandle.Init.ClockDivision = 0;
		TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
		TimHandle.Init.RepetitionCounter = 0;
		HAL_TIM_PWM_Init(&TimHandle);

		sConfig.OCMode = TIM_OCMODE_PWM1;
		sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfig.OCFastMode = TIM_OCFAST_DISABLE;
		sConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
		sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
		sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;

//		/* Set the pulse value for channel 1 */
//		sConfig.Pulse = 0;
//		HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
//		HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
//
//		/* Set the pulse value for channel 2 */
//		sConfig.Pulse = 1000;
//		HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);
//		HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2);
//
//		/* Set the pulse value for channel 3 */
//		sConfig.Pulse = 1000;
//		HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3);
//		HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3);
//
//		/* Set the pulse value for channel 4 */
//		sConfig.Pulse = 0;
//		HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4);
//		HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4);
	}
	void left(int16_t width) {
		if (width > PERIOD_VALUE / 2) {
			width = PERIOD_VALUE / 2;
		}
		if (width < -PERIOD_VALUE / 2) {
			width = -PERIOD_VALUE / 2;
		}
		if (width > 0) {
			sConfig.Pulse = PERIOD_VALUE - width;
			HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
			sConfig.Pulse = PERIOD_VALUE;
			HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2);
		} else {
			sConfig.Pulse = PERIOD_VALUE;
			HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
			sConfig.Pulse = PERIOD_VALUE + width;
			HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2);
		}
	}
	void right(int16_t width) {
		if (width > PERIOD_VALUE / 2) {
			width = PERIOD_VALUE / 2;
		}
		if (width < -PERIOD_VALUE / 2) {
			width = -PERIOD_VALUE / 2;
		}
		if (width > 0) {
			sConfig.Pulse = PERIOD_VALUE;
			HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3);
			HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3);
			sConfig.Pulse = PERIOD_VALUE - width;
			HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4);
			HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4);
		} else {
			sConfig.Pulse = PERIOD_VALUE + width;
			HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3);
			HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3);
			sConfig.Pulse = PERIOD_VALUE;
			HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4);
			HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4);
		}
	}
	void drive(int16_t valueL, int16_t valueR) {
		left(valueL);
		right(valueR);
	}
	void free() {
		sConfig.Pulse = 0;
		HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
		sConfig.Pulse = 0;
		HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2);
		sConfig.Pulse = 0;
		HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3);
		HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3);
		sConfig.Pulse = 0;
		HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4);
		HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4);
	}
private:
	TIM_HandleTypeDef TimHandle;
	TIM_OC_InitTypeDef sConfig;
};

//class Motor {
//public:
//	Motor(PinName ctrl1_pin, PinName ctr2_pin) :
//			ctrl1(ctrl1_pin), ctrl2(ctr2_pin) {
//		ctrl1.period_us(PWM_PERIOD_US);
//		ctrl2.period_us(PWM_PERIOD_US);
//	}
//	void free() {
//		ctrl1.pulsewidth_us(0);
//		ctrl2.pulsewidth_us(0);
//	}
//	void drive(int16_t width) {
//		if (width > PWM_PERIOD_US / 2) {
//			width = PWM_PERIOD_US / 2;
//		}
//		if (width < -PWM_PERIOD_US / 2) {
//			width = -PWM_PERIOD_US / 2;
//		}
//		if (width > 0) {
//			ctrl1.pulsewidth_us(PWM_PERIOD_US - width);
//			ctrl2.pulsewidth_us(PWM_PERIOD_US);
//		} else {
//			ctrl1.pulsewidth_us(PWM_PERIOD_US);
//			ctrl2.pulsewidth_us(PWM_PERIOD_US + width);
//		}
//	}
//private:
//	PwmOut ctrl1, ctrl2;
//};
//
//class Motors {
//public:
//	Motors(PinName ctrl1_pin_l, PinName ctr2_pin_l, PinName ctrl1_pin_r,
//			PinName ctr2_pin_r) :
//			motorL(ctrl1_pin_l, ctr2_pin_l), motorR(ctrl1_pin_r, ctr2_pin_r) {
//
//	}
//	void drive(int16_t val_L, int16_t val_R) {
//		left(val_L);
//		right(val_R);
//	}
//	void left(int16_t val) {
//		motorL.drive(val);
//	}
//	void right(int16_t val) {
//		motorR.drive(-val);
//	}
//	void free() {
//		motorL.free();
//		motorR.free();
//	}
//private:
//	Motor motorL, motorR;
//};

#endif /* MOTOR_H_ */
