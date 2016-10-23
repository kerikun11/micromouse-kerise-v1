#include "mbed.h"
#include "config.h"
#include "Battery.h"

BusOut led(LED1, LED2, LED3, LED4);
Battery battery(BATTERY_PIN);

int main() {
	printf("Battery Voltage: %f\n", battery.voltage());
	if (!battery.check()) {
		printf("Battery Low!");
		while (1) {
			wait(1);
		}
	}

	int i = 0;
	while (true) {
		led = i++;
		printf("%d\n", i);
		wait(0.5);
	}
}

