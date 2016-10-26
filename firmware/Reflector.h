/*
 * Reflector.h
 *
 *  Created on: 2016/10/25
 *      Author: kerikun11
 */

#ifndef REFLECTOR_H_
#define REFLECTOR_H_

#include "mbed.h"

/* Definition for ADCx clock resources */
#define ADCx                            ADC1
#define ADCx_CLK_ENABLE()               __HAL_RCC_ADC1_CLK_ENABLE()
#define DMAx_CLK_ENABLE()               __HAL_RCC_DMA2_CLK_ENABLE()
#define ADCx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()

#define ADCx_FORCE_RESET()              __HAL_RCC_ADC_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __HAL_RCC_ADC_RELEASE_RESET()

/* Definition for ADCx Channel Pin */
#define ADCx_CHANNEL_PIN                GPIO_PIN_0
#define ADCx_CHANNEL_GPIO_PORT          GPIOC

/* Definition for ADCx's Channel */
#define ADCx_CHANNEL                    ADC_CHANNEL_10

/* Definition for ADCx's DMA */
#define ADCx_DMA_CHANNEL                DMA_CHANNEL_0
#define ADCx_DMA_STREAM                 DMA2_Stream0

/* Definition for ADCx's NVIC */
#define ADCx_DMA_IRQn                   DMA2_Stream0_IRQn
#define ADCx_DMA_IRQHandler             DMA2_Stream0_IRQHandler

#define IR_LED_PERIOD_US				100
#define IR_LED_DUTY_US					10

class Reflector {
public:
	Reflector(PinName led_sl_fr_pin, PinName led_sr_fl_pin) :
			led_sl_fr(led_sl_fr_pin), led_sr_fl(led_sr_fl_pin) {
		led_sl_fr.period_us(IR_LED_PERIOD_US);
		led_sr_fl.period_us(IR_LED_PERIOD_US);
	}
	ADC_HandleTypeDef AdcHandle;
	void init() {
		ADC_ChannelConfTypeDef sConfig;
		AdcHandle.Instance = ADCx;
		AdcHandle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
		AdcHandle.Init.Resolution = ADC_RESOLUTION12b;
		AdcHandle.Init.ScanConvMode = DISABLE; /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
		AdcHandle.Init.ContinuousConvMode = ENABLE; /* Continuous mode disabled to have only 1 conversion at each conversion trig */
		AdcHandle.Init.DiscontinuousConvMode = DISABLE; /* Parameter discarded because sequencer is disabled */
		AdcHandle.Init.NbrOfDiscConversion = 0;
		AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Conversion start trigged at each external event */
		AdcHandle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
		AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
		AdcHandle.Init.NbrOfConversion = 1;
		AdcHandle.Init.DMAContinuousRequests = ENABLE;
		AdcHandle.Init.EOCSelection = DISABLE;
		HAL_ADC_Init(&AdcHandle);
		/*##-2- Configure ADC regular channel ######################################*/
		sConfig.Channel = ADCx_CHANNEL;
		sConfig.Rank = 1;
		sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
		sConfig.Offset = 0;
		HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
		/*##-3- Start the conversion process #######################################*/
		/* Note: Considering IT occurring after each number of ADC conversions      */
		/*       (IT by DMA end of transfer), select sampling time and ADC clock    */
		/*       with sufficient duration to not create an overhead situation in    */
		/*        IRQHandler. */
		HAL_ADC_Start_DMA(&AdcHandle, (uint32_t*) &uhADCxConvertedValue, 1);
	}
	uint16_t read() {
		return uhADCxConvertedValue;
	}
private:
	PwmOut led_sl_fr, led_sr_fl;
	/* ADC handler declaration */
	uint16_t uhADCxConvertedValue = 0;
	void ir_enable() {
		led_sl_fr.pulsewidth_us(IR_LED_DUTY_US);
		led_sr_fl.pulsewidth_us(IR_LED_DUTY_US);
	}
	void ir_disable() {
		led_sl_fr.pulsewidth_us(0);
		led_sr_fl.pulsewidth_us(0);
	}
};

#endif /* REFLECTOR_H_ */
