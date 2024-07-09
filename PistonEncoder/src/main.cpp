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

static const uint8_t QUEUE_SIZE = 16;

uint8_t btnQueue[QUEUE_SIZE];
uint8_t queueLength;

static const uint8_t IRQ_BUTTONS = 0;

static uint16_t IRQFlags = 0x00;

static void sendIRQ(uint8_t flag)
{
    IRQFlags |= (1<<flag);
    digitalWrite(PIN_INTERRUPT, HIGH);
}

static void clearIRQ(uint8_t flag)
{
    IRQFlags &= ~(1<<flag);
    if (IRQFlags == 0x00) {
        digitalWrite(PIN_INTERRUPT, LOW);
    }
}

static void resetEncoder(void)
{
    pistons.reset();
    queueLength = 0;
    clearIRQ(IRQ_BUTTONS);
}

void onButtonPress(uint8_t kbd, uint8_t btn, bool longPress)
{
    noInterrupts();
    if (queueLength < QUEUE_SIZE) {
        btnQueue[queueLength++] = (kbd << 7)|(btn << 1)|longPress;
    }
    sendIRQ(IRQ_BUTTONS);
    interrupts();
}

void i2cReceive(uint8_t length) {
    uint8_t cmd = Wire.read();

    switch (cmd) {
        case I2C_CMD_RESET: 
            resetEncoder();
            break;
        case I2C_CMD_SET_LEDS:
            if (Wire.available() > 3) {
                uint8_t mask1 = Wire.read();
                uint8_t mask2 = Wire.read();
                uint8_t mask3 = Wire.read();
                uint8_t mask4 = Wire.read();
                uint8_t kbd = (mask1 >> 7);
                pistons.setLEDs(kbd, mask1 & 0x7F, mask2, mask3, mask4);
            }
            break;
    }
}

void i2cRequest()
{
    Wire.write(queueLength);
    for (uint8_t i = 0; i < queueLength; i++) {
        Wire.write(btnQueue[i]);
    }
    queueLength = 0;
    clearIRQ(IRQ_BUTTONS);
}

void setup() {
    // Enable pullups for unconnected pins
    pinMode(PIN_PC6, INPUT_PULLUP);
 
    // Set output pin modes
    // Set pin value first before turing on output mode, to prevent spurious signals
    digitalWrite(PIN_INTERRUPT, LOW);
    pinMode(PIN_INTERRUPT, OUTPUT);

    Wire.onReceive(i2cReceive);
    Wire.onRequest(i2cRequest);
    pistons.setPressEvent(onButtonPress);

#ifdef PISTON_TECHNICS
    Wire.begin(I2C_ADDR_PISTON_TECHNICS);
    pistons.begin(1);
#else
    Wire.begin(I2C_ADDR_PISTON_KEYBOARD);
    pistons.begin(2);
#endif
}

void loop() {
    pistons.poll();
}
