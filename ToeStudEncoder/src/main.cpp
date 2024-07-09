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
#include <MiniMIDI.h>
#include <MegaWire.h>
#include <CalibratedAnalogInput.h>

#include <common_config.h>

#include "config.h"
#include "Settings.h"
#include "ToeStuds.h"

// Create device driver instances
MiniMIDI MIDI;
MegaWire Wire;

Settings settings;
ToeStuds toestuds;

static uint8_t PEDAL_CHOIR = 0;
static uint8_t PEDAL_SWELL = 1;
static uint8_t PEDAL_CRESCENDO = 2;

CalibratedAnalogInput pedals[3];

static uint8_t MIDIChannel;

static void updateMIDIChannel(uint8_t channel) {
    MIDIChannel = channel;
    settings.setMIDIChannel(channel);
}

static void resetEncoder(void)
{
    // Reset channel
    updateMIDIChannel(MIDI_CHANNEL_TOESTUDS);

    toestuds.reset();
}

void onPedalCalibrate(void* payload) {
    uint8_t pedal = *((uint8_t *)payload);
    AICalibrationData data;
    pedals[pedal].getCalibrationData(data);
    settings.setCalibrationData(pedal, data);
}

void onPedalChange(int value, void* payload) {
    uint8_t pedal = *((uint8_t *)payload);

}

void onToeStudPress(uint8_t button, bool longPress) {

}

void i2cReceive(uint8_t length) {
    uint8_t cmd = Wire.read();
    uint8_t value;

    switch (cmd) {
        case I2C_CMD_RESET: 
            resetEncoder();
            break;
        case I2C_CMD_SET_CHANNEL:
            if (Wire.available()) {
                value = Wire.read();
                updateMIDIChannel(value);
            }
            break;
        case I2C_CMD_CALIBRATE:
            for (uint8_t i = 0; i < 3; i++) {
                pedals[i].resetCalibration();
            }
            break;
        case I2C_CMD_STOP_CALIBRATE:
            for (uint8_t i = 0; i < 3; i++) {
                pedals[i].stopCalibration();
            }
            break;
    }
}

void i2cRequest() {
    Wire.write(MIDIChannel);
    for (uint8_t i = 0; i < 3; i++) {
        Wire.write((uint8_t)(pedals[i].value() >> 8));
        Wire.write((uint8_t)(pedals[i].value() & 0xFF));
    }
}

void setupPedal(uint8_t *pedal, uint8_t pin)
{
    pedals[*pedal].onCalibrating(onPedalCalibrate, pedal);
    pedals[*pedal].onChange(onPedalChange, pedal);
    if (settings.hasCalibrationData()) {
        AICalibrationData data;
        settings.getCalibrationData(*pedal, data);
        pedals[*pedal].setCalibrationData(data);
    }
    pedals[*pedal].begin(pin);
}

void setup() {
    // Enable pullups for unconnected pins
    pinMode(PIN_PB4, INPUT_PULLUP);
    pinMode(PIN_PB5, INPUT_PULLUP);
    pinMode(PIN_PB6, INPUT_PULLUP);
    pinMode(PIN_PB7, INPUT_PULLUP);
    pinMode(PIN_PD2, INPUT_PULLUP);

    // Set output pin modes
    // Set pin value first before turing on output mode, to prevent spurious signals
    digitalWrite(PIN_INTERRUPT, LOW);
    pinMode(PIN_INTERRUPT, OUTPUT);

    MIDIChannel = settings.getMIDIChannel();

    MIDI.begin(MIDIChannel);

    setupPedal(&PEDAL_CHOIR, PIN_PEDAL_CHOIR);
    setupPedal(&PEDAL_SWELL, PIN_PEDAL_SWELL);
    setupPedal(&PEDAL_CRESCENDO, PIN_CRESCENDO);

    Wire.onReceive(i2cReceive);
    Wire.onRequest(i2cRequest);
    Wire.begin(I2C_ADDR_TOESTUD);

    toestuds.setHandleButton(onToeStudPress);
    toestuds.begin();
}

void loop() {
    toestuds.poll();
    for (uint8_t i = 0; i < 2; i++) {
        pedals[i].poll();
    }
}
