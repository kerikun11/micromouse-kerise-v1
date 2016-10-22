#include "mbed.h"
/*
 * HAL_TIM_Encoder_MspInit()
 * Overrides the __weak function stub in stm32f0xx_hal_tim.h
 *
 * Edit the below for your preferred pin wiring & pullup/down
 * I have encoder common at 3V3, using GPIO_PULLDOWN on inputs.
 * Encoder A&B outputs connected directly to GPIOs.
 *
 * http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00088500.pdf
 * Table 11 has GPIO alternate function pinout mappings.
 *
 * TIM1_CH1: AF2 @ PA_8 - TIM1 used as system ticker under mbed, so unavailable
 * TIM1_CH2: AF2 @ PA_9
 *
 * TIM3_CH1: AF1 @ PA_6, PB_4; AF0 @ PC_6*  *only for F030xC devices
 * TIM3_CH2: AF1 @ PA_7, PB_5; AF0 @ PC_7*
 *
 */

#ifdef TARGET_STM32F0
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    if (htim->Instance == TIM3) { //PB_4 PB_5 = Nucleo D5 D4
        __TIM3_CLK_ENABLE();
        __GPIOB_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}
#endif