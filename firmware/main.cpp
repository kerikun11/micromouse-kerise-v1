#include "mbed.h"

BusOut led(LED1, LED2, LED3, LED4);

// main() runs in its own thread in the OS
// (note the calls to Thread::wait below for delays)
int main() {
	int i=0;
    while (true) {
        led = i++;
        printf("%d\n", i);
        wait(0.5);
    }
}

