#include "mbed.h"
/*
 * HAL_TIM_Encoder_MspInit()
 * Overrides the __weak function stub in stm32f3xx_hal_tim.h
 *
 * Edit the below for your preferred pin wiring & pullup/down
 * I have encoder common at 3V3, using GPIO_PULLDOWN on inputs.
 * Encoder A&B outputs connected directly to GPIOs.
 *
 * http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00093333.pdf
 * http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00097745.pdf
 * http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00133117.pdf
 * Table 14 has GPIO alternate function pinout mappings.
 *
 * TIM1_CH1: AF2 @ PC_0, PE_9; AF6 @ PA_8
 * TIM1_CH2: AF2 @ PC_1, PE_11; AF6 @ PA_9
 *
 * (TIM2_CH1: AF1 @ PA_0, PA_5, PA_15; AF2 @ PD_3)  TIM2 is the mbed system ticker, so unavailable as encoder.
 * (TIM2_CH2: AF1 @ PA_1, PB_3; AF2 @ PD_4)
 *
 * TIM3_CH1: AF2 @ PA_6, PB_4, PC_6, PE_2           not for F302R8, OK @ F334R8
 * TIM3_CH2: AF2 @ PA_4, PA_7, PB_5, PC_7, PE_3
 *
 * TIM4_CH1: AF2 @ PB_6, PD_12; AF10 @ PA_11        not for both F302R8 & F334R8
 * TIM4_CH2: AF2 @ PB_7, PD_13; AF10 @ PA_12
 *
 */

#ifdef TARGET_STM32F3

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
        GPIO_InitStruct.Alternate = GPIO_AF6_TIM1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
#if 0 //TIM2 is the mbed system ticker
    else if (htim->Instance == TIM2) { //PA0 PA1 = Nucleo A0 A1
        __TIM2_CLK_ENABLE();
        __GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
#endif
#if defined TARGET_STM32F334R8
    else if (htim->Instance == TIM3) { //PB4 PB5 = Nucleo D5 D4
        __TIM3_CLK_ENABLE();
        __GPIOB_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
#endif
#if 0
    else if (htim->Instance == TIM4) { // PB6 PB7 = Nucleo D10 MORPHO_PB7
        __TIM4_CLK_ENABLE();
        __GPIOB_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
#endif
}
#endif