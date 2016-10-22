#include "mbed.h"
/*
 * HAL_TIM_Encoder_MspInit()
 * Overrides the __weak function stub in stm32f1xx_hal_tim.h
 *
 * Edit the below for your preferred pin wiring & pullup/down
 * I have encoder common at 3V3, using GPIO_PULLDOWN on inputs.
 * Encoder A&B outputs connected directly to GPIOs.
 *
 * http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/CD00161566.pdf
 * Table 5 has GPIO alternate function pinout mappings.
 *
 * TIM1_CH1: default PA_8, remap PE_9
 * TIM1_CH2: default PA_9, remap PE_11
 *
 * TIM2_CH1: default PA_0 
 * TIM2_CH2: default PA_1, remap PB_3
 *
 * TIM3_CH1: default PA_6, remap PB_4, PC_6
 * TIM3_CH2: default PA_7, remap PB_5, PC_7
 *
 * TIM4_CH1: default PB_6, remap PD_12
 * TIM4_CH2: default PB_7, remap PD_13
 *
 * NB one of these timers will be the employed by mbed as systick, unavailable as encoder.
 */

#ifdef TARGET_STM32F1
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    if (htim->Instance == TIM1) { //PA8 PA9 = Nucleo D7 D8
        __TIM1_CLK_ENABLE();
        __GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else if (htim->Instance == TIM2) { //PA0 PA1 = Nucleo A0 A1
        __TIM2_CLK_ENABLE();
        __GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//        GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else if (htim->Instance == TIM3) { //PA6 PA7 = Nucleo D12 D11
        __TIM3_CLK_ENABLE();
        __GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//        GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else if (htim->Instance == TIM4) { // PB6 PB7 = Nucleo D10 MORPHO PB7
        __TIM4_CLK_ENABLE();
        __GPIOB_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//        GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}
#endif