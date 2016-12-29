/*
 * Motor.h
 *
 *  Created on: 2016/10/23
 *      Author: kerikun11
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "mbed.h"
#include "config.h"

#define MOTOR_PERIOD_VALUE				(1000 - 1)

class Motor {
public:
	Motor() {
		GPIO_InitTypeDef GPIO_InitStruct;
		MOTOR_TIMx_CLK_ENABLE();
		MOTOR_TIMx_CHANNEL_GPIO_PORT()
		;

		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

		GPIO_InitStruct.Alternate = MOTOR_TIMx_GPIO_AF_CHANNEL;
		GPIO_InitStruct.Pin = MOTOR_TIMx_GPIO_PIN_CHANNEL1;
		HAL_GPIO_Init(MOTOR_TIMx_GPIO_PORT_CHANNEL1, &GPIO_InitStruct);

		GPIO_InitStruct.Alternate = MOTOR_TIMx_GPIO_AF_CHANNEL;
		GPIO_InitStruct.Pin = MOTOR_TIMx_GPIO_PIN_CHANNEL2;
		HAL_GPIO_Init(MOTOR_TIMx_GPIO_PORT_CHANNEL2, &GPIO_InitStruct);

		GPIO_InitStruct.Alternate = MOTOR_TIMx_GPIO_AF_CHANNEL;
		GPIO_InitStruct.Pin = MOTOR_TIMx_GPIO_PIN_CHANNEL3;
		HAL_GPIO_Init(MOTOR_TIMx_GPIO_PORT_CHANNEL3, &GPIO_InitStruct);

		GPIO_InitStruct.Alternate = MOTOR_TIMx_GPIO_AF_CHANNEL;
		GPIO_InitStruct.Pin = MOTOR_TIMx_GPIO_PIN_CHANNEL4;
		HAL_GPIO_Init(MOTOR_TIMx_GPIO_PORT_CHANNEL4, &GPIO_InitStruct);

		TimHandle.Instance = MOTOR_TIMx;
		TimHandle.Init.Prescaler = 0;
		TimHandle.Init.Period = MOTOR_PERIOD_VALUE;
		TimHandle.Init.ClockDivision = 0;
		TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
		TimHandle.Init.RepetitionCounter = 0;
		if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK) {
			printf("Couldn't Init PWM\r\n");
			while (1) {
			}
		}

		sConfig.OCMode = TIM_OCMODE_PWM1;
		sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfig.OCFastMode = TIM_OCFAST_DISABLE;
		sConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
		sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
		sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;

		emergency = false;
		free();
	}
	void left(int16_t width) {
		if (!emergency) {
			if (width > MOTOR_PERIOD_VALUE) {
				width = MOTOR_PERIOD_VALUE;
			}
			if (width < -MOTOR_PERIOD_VALUE) {
				width = -MOTOR_PERIOD_VALUE;
			}
			if (width > 0) {
				sConfig.Pulse = MOTOR_PERIOD_VALUE - width;
				HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
				HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
				sConfig.Pulse = MOTOR_PERIOD_VALUE;
				HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);
				HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2);
			} else {
				sConfig.Pulse = MOTOR_PERIOD_VALUE;
				HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
				HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
				sConfig.Pulse = MOTOR_PERIOD_VALUE + width;
				HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);
				HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2);
			}
		}
	}
	void right(int16_t width) {
		if (!emergency) {
			if (width > MOTOR_PERIOD_VALUE) {
				width = MOTOR_PERIOD_VALUE;
			}
			if (width < -MOTOR_PERIOD_VALUE) {
				width = -MOTOR_PERIOD_VALUE;
			}
			if (width > 0) {
				sConfig.Pulse = MOTOR_PERIOD_VALUE;
				HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3);
				HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3);
				sConfig.Pulse = MOTOR_PERIOD_VALUE - width;
				HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4);
				HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4);
			} else {
				sConfig.Pulse = MOTOR_PERIOD_VALUE + width;
				HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3);
				HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3);
				sConfig.Pulse = MOTOR_PERIOD_VALUE;
				HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4);
				HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4);
			}
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
	void emergency_stop() {
		emergency = true;
		free();
	}
	void emergency_release() {
		emergency = false;
		free();
	}
	bool isEmergency() {
		return emergency;
	}
private:
	TIM_HandleTypeDef TimHandle;
	TIM_OC_InitTypeDef sConfig;
	bool emergency;
};

#endif /* MOTOR_H_ */
