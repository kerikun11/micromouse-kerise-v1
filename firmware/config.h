/*
 * config.h
 *
 *  Created on: 2016/10/23
 *      Author: kerikun11
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define BATTERY_PIN			PB_1

#define ENCODER_L_TIMER		TIM3
#define ENCODER_R_TIMER		TIM4

#define MOTOR_L_CTRL1_PIN	PA_0
#define MOTOR_L_CTRL2_PIN	PA_1
#define MOTOR_R_CTRL1_PIN	PA_2
#define MOTOR_R_CTRL2_PIN	PA_3

// Modify PeripheralPins.c PWM Pin using Timer11
#define BUZZER_PIN			PB_9

#define BUTTON_PIN			PB_0

#define LED1_PIN			PC_5
#define LED2_PIN			PC_6
#define LED3_PIN			PC_7
#define LED4_PIN			PC_8


#define MPU6500_MOSI_PIN	PA_7
#define MPU6500_MISO_PIN	PA_6
#define MPU6500_SCLK_PIN	PA_5
#define MPU6500_SSEL_PIN	PA_4

#endif /* CONFIG_H_ */
