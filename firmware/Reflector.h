/*
 * Reflector.h
 *
 *  Created on: 2016/10/25
 *      Author: kerikun11
 */

#ifndef REFLECTOR_H_
#define REFLECTOR_H_

#include "mbed.h"
#include "FFT.h"

/* Definition for ADCx clock resources */
#define ADCx_S							ADC1
#define ADCx_F							ADC2
#define ADCx_S_CLK_ENABLE()				__HAL_RCC_ADC1_CLK_ENABLE()
#define ADCx_F_CLK_ENABLE()				__HAL_RCC_ADC2_CLK_ENABLE()
#define ADCx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()

#define ADCx_FORCE_RESET()              __HAL_RCC_ADC_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __HAL_RCC_ADC_RELEASE_RESET()

/* Definition for ADCx Channel Pin */
#define ADCx_CHANNEL_SL_PIN             GPIO_PIN_0
#define ADCx_CHANNEL_FL_PIN             GPIO_PIN_1
#define ADCx_CHANNEL_FR_PIN             GPIO_PIN_2
#define ADCx_CHANNEL_SR_PIN             GPIO_PIN_3
#define ADCx_CHANNEL_GPIO_PORT          GPIOC

/* Definition for ADCx's Channel */
#define ADCx_CHANNEL_SL					ADC_CHANNEL_10
#define ADCx_CHANNEL_FL					ADC_CHANNEL_11
#define ADCx_CHANNEL_FR					ADC_CHANNEL_12
#define ADCx_CHANNEL_SR					ADC_CHANNEL_13

#define IR_LED_PERIOD_US				100
#define IR_LED_DUTY_US					20

#define IR_RECEIVER_SAMPLING_PERIOD_US	10
#define IR_RECEIVER_UPDATE_PERIOD_US	100000

class Reflector {
public:
	Reflector(PinName led_sl_fr_pin, PinName led_sr_fl_pin) :
			led_sl_fr(led_sl_fr_pin), led_sr_fl(led_sr_fl_pin) {
		led_sl_fr.period_us(IR_LED_PERIOD_US);
		led_sr_fl.period_us(IR_LED_PERIOD_US);

		/*##-1- Enable peripherals and GPIO Clocks #################################*/
		/* ADCx Periph clock enable */
		ADCx_S_CLK_ENABLE();
		ADCx_F_CLK_ENABLE();
		/* Enable GPIO clock ****************************************/
		ADCx_CHANNEL_GPIO_CLK_ENABLE();

		/*##-2- Configure peripheral GPIO ##########################################*/
		GPIO_InitTypeDef GPIO_InitStruct;
		/* ADC Channel GPIO pin configuration */
		GPIO_InitStruct.Pin = ADCx_CHANNEL_SL_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
		/* ADC Channel GPIO pin configuration */
		GPIO_InitStruct.Pin = ADCx_CHANNEL_FL_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
		/* ADC Channel GPIO pin configuration */
		GPIO_InitStruct.Pin = ADCx_CHANNEL_FR_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
		/* ADC Channel GPIO pin configuration */
		GPIO_InitStruct.Pin = ADCx_CHANNEL_SR_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);

		/*##-1- Configure the ADC peripheral #######################################*/
		AdcHandle_S.Instance = ADCx_S;
		AdcHandle_S.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
		AdcHandle_S.Init.Resolution = ADC_RESOLUTION12b;
		AdcHandle_S.Init.ScanConvMode = DISABLE; /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
		AdcHandle_S.Init.ContinuousConvMode = ENABLE; /* Continuous mode disabled to have only 1 conversion at each conversion trig */
		AdcHandle_S.Init.DiscontinuousConvMode = DISABLE; /* Parameter discarded because sequencer is disabled */
		AdcHandle_S.Init.NbrOfDiscConversion = 0;
		AdcHandle_S.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Conversion start trigged at each external event */
		AdcHandle_S.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
		AdcHandle_S.Init.DataAlign = ADC_DATAALIGN_RIGHT;
		AdcHandle_S.Init.NbrOfConversion = 1;
		AdcHandle_S.Init.DMAContinuousRequests = DISABLE;
		AdcHandle_S.Init.EOCSelection = DISABLE;
		HAL_ADC_Init(&AdcHandle_S);

		/*##-1- Configure the ADC peripheral #######################################*/
		AdcHandle_F.Instance = ADCx_F;
		AdcHandle_F.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
		AdcHandle_F.Init.Resolution = ADC_RESOLUTION12b;
		AdcHandle_F.Init.ScanConvMode = DISABLE; /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
		AdcHandle_F.Init.ContinuousConvMode = ENABLE; /* Continuous mode disabled to have only 1 conversion at each conversion trig */
		AdcHandle_F.Init.DiscontinuousConvMode = DISABLE; /* Parameter discarded because sequencer is disabled */
		AdcHandle_F.Init.NbrOfDiscConversion = 0;
		AdcHandle_F.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Conversion start trigged at each external event */
		AdcHandle_F.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
		AdcHandle_F.Init.DataAlign = ADC_DATAALIGN_RIGHT;
		AdcHandle_F.Init.NbrOfConversion = 1;
		AdcHandle_F.Init.DMAContinuousRequests = DISABLE;
		AdcHandle_F.Init.EOCSelection = DISABLE;
		HAL_ADC_Init(&AdcHandle_F);

		/*##-2- Configure ADC regular channel ######################################*/
		sConfig.Rank = 1;
		sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
		sConfig.Offset = 0;

		sample.state = 0;
		samplingTicker.attach_us(this, &Reflector::sampling,
		IR_RECEIVER_SAMPLING_PERIOD_US);
		updateTicker.attach_us(this, &Reflector::update,
		IR_RECEIVER_UPDATE_PERIOD_US);
	}
	float sl() {
		return distance[0];
	}
	float fl() {
		return distance[1];
	}
	float fr() {
		return distance[2];
	}
	float sr() {
		return distance[3];
	}
private:
	PwmOut led_sl_fr, led_sr_fl;
	ADC_HandleTypeDef AdcHandle_S;
	ADC_HandleTypeDef AdcHandle_F;
	ADC_ChannelConfTypeDef sConfig;
	Ticker samplingTicker;
	Ticker updateTicker;
	float distance[4];
	static const int buffer_size = 64;
	struct sampling_buffer {
		uint16_t buffer[4][buffer_size];
		int state;
		int pointer;
	} sample;

	void ir_led(bool sl_fr, bool sr_fl) {
		if (sl_fr)
			led_sl_fr.pulsewidth_us(IR_LED_DUTY_US);
		else
			led_sl_fr.pulsewidth_us(0);
		if (sr_fl)
			led_sr_fl.pulsewidth_us(IR_LED_DUTY_US);
		else
			led_sr_fl.pulsewidth_us(0);
	}
	void sampling() {
		switch (sample.state) {
		case 0:
			sample.pointer = 0;
			sample.state = 1;
			ir_led(true, false);
			sConfig.Channel = ADCx_CHANNEL_SL;
			HAL_ADC_ConfigChannel(&AdcHandle_S, &sConfig);
			sConfig.Channel = ADCx_CHANNEL_FR;
			HAL_ADC_ConfigChannel(&AdcHandle_F, &sConfig);
			break;
		case 1:
			sample.buffer[0][sample.pointer] = HAL_ADC_GetValue(&AdcHandle_S);
			sample.buffer[2][sample.pointer] = HAL_ADC_GetValue(&AdcHandle_F);
			sample.pointer++;
			if (sample.pointer >= buffer_size) {
				sample.pointer = 0;
				sample.state = 2;
				ir_led(false, true);
				sConfig.Channel = ADCx_CHANNEL_SR;
				HAL_ADC_ConfigChannel(&AdcHandle_S, &sConfig);
				sConfig.Channel = ADCx_CHANNEL_FL;
				HAL_ADC_ConfigChannel(&AdcHandle_F, &sConfig);
			}
			break;
		case 2:
			sample.buffer[3][sample.pointer] = HAL_ADC_GetValue(&AdcHandle_S);
			sample.buffer[1][sample.pointer] = HAL_ADC_GetValue(&AdcHandle_F);
			sample.pointer++;
			if (sample.pointer >= buffer_size) {
				sample.pointer = 0;
				sample.state = 0;
				ir_led(false, false);
				samplingTicker.detach();
			}
			break;
		}
		HAL_ADC_Start_IT(&AdcHandle_S);
		HAL_ADC_Start_IT(&AdcHandle_F);
	}
	void update() {
		// read data and fft
		for (int ch = 0; ch < 4; ch++) {
			FFT fft(buffer_size);
			for (int i = 0; i < buffer_size; i++) {
				fft[i] = sample.buffer[ch][i];
			}
			fft.execute();
			int m = buffer_size * IR_RECEIVER_SAMPLING_PERIOD_US
					/ IR_LED_PERIOD_US;
			distance[ch] = norm(fft[m]);
		}
		// start next sampling
		samplingTicker.attach_us(this, &Reflector::sampling,
		IR_RECEIVER_SAMPLING_PERIOD_US);
	}
};

#endif /* REFLECTOR_H_ */
