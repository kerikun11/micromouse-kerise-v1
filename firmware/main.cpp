#include "mbed.h"
#include "config.h"
#include "Battery.h"
#include "Buzzer.h"
#include "Button.h"
#include "Encoder.h"
#include "Motor.h"
#include "MPU6500.h"
#include "Reflector.h"

BusOut leds(LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN);
Battery battery(BATTERY_PIN);
Encoders encoder(ENCODER_L_TIMER, ENCODER_R_TIMER);
Motor motor;
Buzzer buzzer(BUZZER_PIN);
Button button(BUTTON_PIN);
MPU6500 mpu(MPU6500_MOSI_PIN, MPU6500_MISO_PIN, MPU6500_SCLK_PIN,
MPU6500_SSEL_PIN);
Reflector reflector(IR_LED_SL_FR_PIN, IR_LED_SR_FL_PIN);

void print_info() {
//	printf("Gz: %.2f\tAngle: %.2f\tInt: %.2f\n", mpu.gz, mpu.angle,
//			mpu.int_angle);
	printf("ADC: %d\n", reflector.read());
}

void ctrl_arts() {
	float turn = (0 - mpu.gz) * 1 + (0 - mpu.angle) * 10
			+ (0 - mpu.int_angle) * 1;
//				turn = 0;
	float straight = (300000 - encoder.left() - encoder.right()) * 0.01;
	if (straight > 100)
		straight = 100;
	straight = 0;
	int32_t valueL = straight - turn / 2;
	int32_t valueR = straight + turn / 2;
	motor.drive(valueL, valueR);
}

int main() {
	printf("\nHello World!\n");
	float voltage = battery.voltage();
	printf("Battery Voltage: %.3f [V]\n", voltage);
	if (!battery.check()) {
		buzzer.playLowBattery();
		printf("Battery Low!\n");
		while (1) {
			wait(1);
		}
	}
	buzzer.playBoot();
	leds = 0x6;
	wait(0.2);
	leds = 0x9;

	/* initializations */
	button.init();
	mpu.init();
	motor.init();
//	reflector.init();

	Ticker t;
	t.attach(print_info, 0.2);

	Ticker ctrl;

	while (true) {
		static int32_t prev_enc_L, prev_enc_R;
		int32_t enc_L = encoder.left();
		int32_t enc_R = encoder.right();
		if (enc_L != prev_enc_L || enc_R != prev_enc_R) {
			prev_enc_L = enc_L;
			prev_enc_R = enc_R;
			printf("L: %ld\tR: %ld\n", enc_L, enc_R);
		}

		if (button.pressed) {
			button.flags = 0;
			buzzer.playSelect();
			ctrl.detach();
			motor.free();
		}
		if (button.long_pressed_1) {
			button.flags = 0;
			buzzer.playConfirm();
			ctrl.attach_us(ctrl_arts, 100);
		}
	}
}

void ADCx_DMA_IRQHandler(void) {
	HAL_DMA_IRQHandler(reflector.AdcHandle.DMA_Handle);
}

