/**
 * Author: Stefan Hepp (stefan@stefant.org)
 * 
 * Implementation of the LED controller.
 **/

#include <Arduino.h>
#include <MegaWire.h>
#include <avrlib.h>

#include <common_config.h>

#include "config.h"
#include "Settings.h"
#include "LED.h"

MegaWire Wire;

Settings   settings;
LEDDriver  led;

// Last read value from button inputs
static uint8_t ButtonStatus = 0x00;

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

static void updateLEDIntensity(int index, uint8_t intensity) {
    led.setIntensity(index, intensity);
    settings.setLEDIntensity(index, intensity);
}

static void resetEncoder(void)
{
    led.reset();
    for (int i = 0; i < 7; i++) {
        updateLEDIntensity(i, 0);
    }
}

void i2cReceive(uint8_t length) {
    uint8_t cmd = Wire.read();
    uint8_t ledIndex;

    switch (cmd) {
        case I2C_CMD_RESET:
            resetEncoder();
            break;
        case I2C_CMD_LED_INTENSITY:
            if (Wire.available()) {
                ledIndex = Wire.read();
                switch (ledIndex) {
                    case 0:
                        updateLEDIntensity(0, Wire.read());
                        updateLEDIntensity(1, Wire.read());
                        updateLEDIntensity(2, Wire.read());
                        break;
                    case 1:
                        updateLEDIntensity(3, Wire.read());
                        updateLEDIntensity(4, Wire.read());
                        updateLEDIntensity(5, Wire.read());
                        break;
                    case 2:
                        updateLEDIntensity(6, Wire.read());
                        break;
                }
            }
            break;
    }
}

void i2cRequest() {
    Wire.write(ButtonStatus);

    // BTN5 is a push button, reset the status when read via I2C
    ButtonStatus &= 0x0F;

    clearIRQ(IRQ_BUTTONS);
}

void updateButton(int index, uint8_t value) {
    if (value) {
        if ((ButtonStatus & (1<<index)) == 0) {
            ButtonStatus |= (1<<index);
            sendIRQ(IRQ_BUTTONS);
        }
    } else {
        // Clear button, except button 5 (cleared on I2C read)
        if ((ButtonStatus & (1<<index)) > 0 && index < 4) {
            ButtonStatus &= ~(1<<index);
            sendIRQ(IRQ_BUTTONS);
        }
    }
}

void setup() {
    // Enable pullups for unconnected pins
    pinMode(PIN_PC0, INPUT_PULLUP);
    pinMode(PIN_PC1, INPUT_PULLUP);
    pinMode(PIN_PC2, INPUT_PULLUP);
    pinMode(PIN_PC3, INPUT_PULLUP);
    pinMode(PIN_PC6, INPUT_PULLUP);

    // Button inputs with pullup
    IO_PORT(PORT_BTN) = 0xFF;
    IO_DDR(PORT_BTN) = 0x00;
  
    // Set output pin modes
    // Set pin value first before turing on output mode, to prevent spurious signals
    IO_PORT(PORT_LED) = 0x00;
    IO_DDR(PORT_LED) = 0xFF;

    led.begin();
    for (int i = 0; i < NUM_LEDS; i++) {
        led.setIntensity(i, settings.getLEDIntensity(i));
    }

    Wire.onReceive(i2cReceive);
    Wire.onRequest(i2cRequest);
    Wire.begin(Controller::MC_LEDController);
}

void loop() {
    uint8_t btns = IO_PIN(PORT_BTN);

    updateButton(0, btns & (1<<BTN1));
    updateButton(1, btns & (1<<BTN2));
    updateButton(2, btns & (1<<BTN3));
    updateButton(3, btns & (1<<BTN4));
    updateButton(4, btns & (1<<BTN5));

    led.updateLEDs();

    digitalWrite(PIN_LED1_R, led.getLEDStatus(0));
    digitalWrite(PIN_LED1_G, led.getLEDStatus(1));
    digitalWrite(PIN_LED1_B, led.getLEDStatus(2));
    digitalWrite(PIN_LED2_R, led.getLEDStatus(3));
    digitalWrite(PIN_LED2_G, led.getLEDStatus(4));
    digitalWrite(PIN_LED2_B, led.getLEDStatus(5));
    digitalWrite(PIN_LED3,   led.getLEDStatus(6));
}
