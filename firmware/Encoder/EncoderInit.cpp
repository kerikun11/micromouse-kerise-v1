#include "mbed.h"

void EncoderInit(TIM_Encoder_InitTypeDef * encoder, TIM_HandleTypeDef * timer, TIM_TypeDef * TIMx, uint32_t maxcount, uint32_t encmode)
{

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
        while (1) {}
    }

    if(HAL_TIM_Encoder_Start(timer,TIM_CHANNEL_1)!=HAL_OK) {
        printf("Couldn't Start Encoder\r\n");
        while (1) {}
    }
}

