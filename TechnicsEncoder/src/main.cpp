/**
 * Author: Stefan Hepp (stefan@stefant.org)
 * 
 * Implementation of the Technics keyboard MIDI encoder.
 * Reads Technics proprietary keyboard messages and generates MIDI messages.
 * Configurable via I2C interface.
 * 
 * Pins:
 * - PB0-6: IN;  Keyboard data bytes
 * - PB7:   IN;  Read
 * - PC0:   IN;  Bend wheel
 * - PC4,5: I2C
 * - PC6:   In;  Reset (via ISP)
 * - PD0,1  UART
 * - PD3:   OUT; Interrupt to signal I2C data ready
 * - PD5:   OUT; Enable Technics controller
 * - PD6:   OUT; Latch in new data
 * - PD7:   OUT: Data clock
 **/

#include <Arduino.h>
#include <MiniMIDI.h>
#include <CalibratedAnalogInput.h>
#include <MegaWire.h>

#include <common_config.h>

#include "config.h"
#include "Settings.h"
#include "TechnicsKeyboard.h"

// Create device driver instances
MiniMIDI MIDI;
MegaWire Wire;

Settings         settings;
TechnicsKeyboard kbd;
CalibratedAnalogInput  wheel;

static uint8_t MIDIChannel;

/**
 * send notes off for all pressed notes.
 * @param force if non-zero, send a note-off controller msg on the current channel.
 **/
static void sendAllNotesOff()
{
    MIDI.sendAllNotesOff(MIDIChannel);
}

static void updateMIDIChannel(uint8_t channel) {
    MIDIChannel = channel;
    MIDI.setInputChannel(channel);
    settings.setMIDIChannel(channel);
}

static void resetEncoder(void)
{
    // disable all notes on current channel
    sendAllNotesOff();

    // Reset channel
    updateMIDIChannel(MIDI_CHANNEL_TECHNICS);
}

void onKeyChange(uint8_t note, uint8_t velocity) {
    if (velocity > 0) {
        MIDI.sendNoteOn(  note, velocity, MIDIChannel );
    } else {
	    // this is a note off event (velocity is 0)
        MIDI.sendNoteOff( note, velocity, MIDIChannel );
    }
}

void onWheelCalibrate(void* payload) {
    AICalibrationData data;
    wheel.getCalibrationData(data);
    settings.setWheelSettings(data);
}

void onWheelChange(int value, void* payload) {
    MIDI.sendPitchBend(value * 16, MIDIChannel);
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
            wheel.resetCalibration();
            break;
        case I2C_CMD_STOP_CALIBRATE:
            wheel.stopCalibration();
            break;
    }
}

void i2cRequest() {
    Wire.write(MIDIChannel);
    Wire.write((uint8_t)(wheel.value() >> 8));
    Wire.write((uint8_t)(wheel.value() & 0xFF));
}

void setup() {
    // Enable pullups for unconnected pins
    pinMode(PIN_PC1, INPUT_PULLUP);
    pinMode(PIN_PC2, INPUT_PULLUP);
    pinMode(PIN_PC3, INPUT_PULLUP);
    pinMode(PIN_PC6, INPUT_PULLUP);
    pinMode(PIN_PD2, INPUT_PULLUP);
    pinMode(PIN_PD4, INPUT_PULLUP);

    // Set output pin modes
    // Set pin value first before turing on output mode, to prevent spurious signals
    digitalWrite(PIN_INTERRUPT, HIGH);
    pinMode(PIN_INTERRUPT, OUTPUT);

    MIDIChannel = settings.getMIDIChannel();
    MIDI.turnThruOn(ThruMode::Full);
    MIDI.begin(MIDIChannel);

    wheel.onCalibrating(onWheelCalibrate);
    wheel.onChange(onWheelChange);
    wheel.setRange(1023, 512);
    wheel.setSensitivy(1, 2);
    if (settings.hasSettings()) {
        AICalibrationData data;
        settings.getWheelSettings(data);
        wheel.setCalibrationData(data);
    }
    wheel.begin(PIN_BENDER);

    Wire.onReceive(i2cReceive);
    Wire.onRequest(i2cRequest);
    Wire.begin(Controller::MC_Technics);

    kbd.setHandleKeyChange(onKeyChange);
    kbd.begin();
}

void loop() {
    kbd.poll();
    wheel.poll();

    // wait for any MIDI messages being sent
    while (MIDI.sending()) {
        delayMicroseconds(20);
    }
}
