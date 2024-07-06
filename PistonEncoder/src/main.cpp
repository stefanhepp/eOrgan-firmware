/**
 * Author: Stefan Hepp (stefan@stefant.org)
 * 
 * Implementation of the pedalboard controller.
 * Reads pedal switches and generates MIDI messages.
 * Configurable either via config switch or I2C interface.
 * 
 * Pins:
 * - PB0-7: IN;  Keyboard data bytes
 * - PC3:   OUT; Interrupt to signal I2C data ready
 * - PC4,5: I2C
 * - PC6:   In;  Reset (via ISP)
 * - PD0,1  UART
 * - PD2-4: OUT; Select line
 * - PD5-6: OUT; Select keyboard
 **/

#include <Arduino.h>
#include <MegaWire.h>

#include <common_config.h>

#include "config.h"
#include "Pistons.h"

// Create device driver instances
MegaWire Wire;

Pistons pistons;

void setup() {
    // Enable pullups for unconnected pins
    pinMode(PIN_PC6, INPUT_PULLUP);
 
    // Set output pin modes
    // Set pin value first before turing on output mode, to prevent spurious signals
    digitalWrite(PIN_INTERRUPT, LOW);
    pinMode(PIN_INTERRUPT, OUTPUT);

    pinMode(PIN_A0,   OUTPUT);
    pinMode(PIN_A1,   OUTPUT);
    pinMode(PIN_A2,   OUTPUT);

    Wire.begin(I2C_ADDR_PISTON);

    pistons.setPressEvent(onPistonPress);
    pistons.begin();
}

void loop() {
    pistons.poll();
}
