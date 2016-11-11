/*
 * Reflector.h
 *
 *  Created on: 2016/10/25
 *      Author: kerikun11
 */

#ifndef REFLECTOR_H_
#define REFLECTOR_H_

#include "mbed.h"
#include "config.h"
//#include "QuadratureDemodulator.h"

/* Definition for ADCx clock resources */
#define ADCx_S							ADC2
#define ADCx_F							ADC3
#define ADCx_S_CLK_ENABLE()				__HAL_RCC_ADC2_CLK_ENABLE()
#define ADCx_F_CLK_ENABLE()				__HAL_RCC_ADC3_CLK_ENABLE()
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

#define IR_LED_PERIOD_US				500
#define IR_LED_DUTY_US					100

#define IR_RECEIVER_SAMPLE_SIZE			25
#define IR_RECEIVER_SAMPLING_PERIOD_US	20
#define IR_RECEIVER_UPDATE_PERIOD_US	1000

#define REFLECTOR_GAIN_SIDE				1.34f
#define REFLECTOR_GAIN_FLONT			1.24f

class Reflector {
public:
	Reflector(PinName led_sl_fr_pin, PinName led_sr_fl_pin) :
			led_sl_fr(led_sl_fr_pin), led_sr_fl(led_sr_fl_pin),
					updateThread(PRIORITY_IR_RECEIVER_UPDATE) {
		led_sl_fr.period_us(IR_LED_PERIOD_US);
		led_sr_fl.period_us(IR_LED_PERIOD_US);
		adcInitialize();
		buffer_pointer = IR_RECEIVER_SAMPLE_SIZE - 1;
	}
	void enable() {
		buffer_pointer = IR_RECEIVER_SAMPLE_SIZE - 1;

		samplingTicker.attach_us(this, &Reflector::samplingIsr,
		IR_RECEIVER_SAMPLING_PERIOD_US);

		updateThread.start(this, &Reflector::updateTask);
		updateTicker.attach_us(this, &Reflector::updateIsr,
		IR_RECEIVER_UPDATE_PERIOD_US);
	}
	void disable() {
		updateTicker.detach();
		updateThread.terminate();
		samplingTicker.detach();
		ir_led(false, false);
	}
	int16_t side(uint8_t left_or_right) {
		if (left_or_right == 0) return sl();
		else return sr();
	}
	int16_t flont(uint8_t left_or_right) {
		if (left_or_right == 0) return fl();
		else return fr();
	}
	int16_t sl() {
		return distance[1] * REFLECTOR_GAIN_SIDE;
	}
	int16_t fl() {
		return distance[2] * REFLECTOR_GAIN_FLONT;
	}
	int16_t fr() {
		return distance[3] * REFLECTOR_GAIN_FLONT;
	}
	int16_t sr() {
		return distance[0] * REFLECTOR_GAIN_SIDE;
	}
private:
	PwmOut led_sl_fr, led_sr_fl;
	Thread updateThread;
	ADC_HandleTypeDef AdcHandle_S;
	ADC_HandleTypeDef AdcHandle_F;
	ADC_ChannelConfTypeDef sConfig;
	Ticker samplingTicker;
	Ticker updateTicker;
	volatile int16_t distance[4];
	volatile int16_t prev_distance[4];
	volatile uint16_t buffer[4][IR_RECEIVER_SAMPLE_SIZE];
	volatile int buffer_pointer;

	void ir_led(bool sl_fr, bool sr_fl) {
		if (sl_fr) {
			led_sl_fr.pulsewidth_us(IR_LED_DUTY_US);
		} else {
			led_sl_fr.pulsewidth_us(0);
		}
		if (sr_fl) {
			led_sr_fl.pulsewidth_us(IR_LED_DUTY_US);
		} else {
			led_sr_fl.pulsewidth_us(0);
		}
	}
	void updateIsr() {
		updateThread.signal_set(0x01);
	}
	void updateTask() {
		while (1) {
			Thread::signal_wait(0x01);
			for (int ch = 0; ch < 4; ch++) {
				int16_t max = 0;
				for (int i = 0; i < IR_RECEIVER_SAMPLE_SIZE; i++) {
					int16_t value = 2048 - buffer[ch][i];
					if (value > max) {
						max = value;
					}
				}
//				distance[ch] = (max + prev_distance[ch]) / 2;
				distance[ch] = max;
				prev_distance[ch] = distance[ch];
			}
		}
	}
	void set_adc_sl_fr() {
		ir_led(true, false);
		sConfig.Channel = ADCx_CHANNEL_SL;
		HAL_ADC_ConfigChannel(&AdcHandle_S, &sConfig);
		sConfig.Channel = ADCx_CHANNEL_FR;
		HAL_ADC_ConfigChannel(&AdcHandle_F, &sConfig);
	}
	void set_adc_sr_fl() {
		ir_led(false, true);
		sConfig.Channel = ADCx_CHANNEL_SR;
		HAL_ADC_ConfigChannel(&AdcHandle_S, &sConfig);
		sConfig.Channel = ADCx_CHANNEL_FL;
		HAL_ADC_ConfigChannel(&AdcHandle_F, &sConfig);
	}
	void samplingIsr() {
		buffer[buffer_pointer / IR_RECEIVER_SAMPLE_SIZE + 0][buffer_pointer
				% IR_RECEIVER_SAMPLE_SIZE] = HAL_ADC_GetValue(&AdcHandle_S);
		buffer[buffer_pointer / IR_RECEIVER_SAMPLE_SIZE + 2][buffer_pointer
				% IR_RECEIVER_SAMPLE_SIZE] = HAL_ADC_GetValue(&AdcHandle_F);
		buffer_pointer++;
		if (buffer_pointer == IR_RECEIVER_SAMPLE_SIZE) {
			set_adc_sl_fr();
		} else if (buffer_pointer == IR_RECEIVER_SAMPLE_SIZE * 2) {
			set_adc_sr_fl();
			buffer_pointer = 0;
		}
		HAL_ADC_Start_IT(&AdcHandle_S);
		HAL_ADC_Start_IT(&AdcHandle_F);
	}
	void adcInitialize() {
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
		AdcHandle_S.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
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
		if (HAL_ADC_Init(&AdcHandle_S) != HAL_OK) {
			printf("Couldn't Init ADC S\r\n");
//			while (1) {
//			}
		}
		/*##-1- Configure the ADC peripheral #######################################*/
		AdcHandle_F.Instance = ADCx_F;
		AdcHandle_F.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
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
		if (HAL_ADC_Init(&AdcHandle_F) != HAL_OK) {
			printf("Couldn't Init ADC F\r\n");
//			while (1) {
//			}
		}

		/*##-2- Configure ADC regular channel ######################################*/
		sConfig.Rank = 1;
		sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
		sConfig.Offset = 0;
	}
};

#endif /* REFLECTOR_H_ */
