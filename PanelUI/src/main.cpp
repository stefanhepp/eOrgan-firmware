#include <stdio.h>
#include <stdtypes.h>

#include <wiringPi.h>
#include <wiringPiSerial.h>
#include <wiringPiI2C.h>

#define PIN_INT_LEFT  5
#define PIN_INT_RIGHT 4

int main(void) {

    wiringPiSetup();
    pinMode(PIN_INT_RIGHT, INPUT);

    for (;;) {
	int interrupt = digitalRead(PIN_INT_RIGHT);

	if (interrupt == LOW) {
	
	}
    }
    return 0;
}

