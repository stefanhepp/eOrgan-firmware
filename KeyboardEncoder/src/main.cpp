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

#include <common_config.h>

#include "config.h"
#include "Settings.h"
#include "Keyboard.h"

// Create device driver instances
//MIDI_CREATE_DEFAULT_INSTANCE();

MiniMIDI MIDI;
MegaWire Wire;

Settings settings;
Keyboard kbd;

static uint8_t MIDIChannel[2];

static const uint8_t IRQ_LEARN = 0;

static uint16_t IRQFlags = 0x00;

static void sendIRQ(uint8_t flag)
{
    IRQFlags |= (1<<flag);
    digitalWrite(PIN_INTERRUPT, LOW);
}

static void clearIRQ(uint8_t flag)
{
    IRQFlags &= ~(1<<flag);
    if (IRQFlags == 0x00) {
        digitalWrite(PIN_INTERRUPT, HIGH);
    }
}


/**
 * send notes off for all pressed notes.
 * @param force if non-zero, send a note-off controller msg on the current channel.
 **/
static void sendAllNotesOff()
{
    MIDI.sendControlChange(MidiControlChangeNumber::AllNotesOff, 0, MIDIChannel[0]);
    MIDI.sendControlChange(MidiControlChangeNumber::AllNotesOff, 0, MIDIChannel[1]);
}

static void updateMIDIChannel(uint8_t kbd, uint8_t channel) {
    MIDIChannel[kbd] = channel;
    if (kbd == 0) {
        MIDI.setInputChannel(channel);
    }
    settings.setMIDIChannel(kbd, channel);
}

static void resetEncoder(void)
{
    // disable all notes on current channel
    sendAllNotesOff();

    // Reset channel
    updateMIDIChannel(0, MIDI_CHANNEL_KEYBOARD_1);
    updateMIDIChannel(1, MIDI_CHANNEL_KEYBOARD_2);

    clearIRQ(IRQ_LEARN);
}

void onKeyChange(uint8_t kbd, uint8_t note, uint8_t velocity) {
    if (velocity > 0) {
        MIDI.sendNoteOn(  note, velocity, MIDIChannel[kbd] );
    } else {
	    // this is a note off event (velocity is 0)
        MIDI.sendNoteOff( note, velocity, MIDIChannel[kbd] );
    }
}

void onLearnComplete(uint8_t kbd) {
    sendIRQ(IRQ_LEARN);
}

void i2cReceive(uint8_t length) {
    uint8_t cmd = Wire.read();
    uint8_t value;

    switch (cmd) {
        case I2C_CMD_RESET: 
            resetEncoder();
            break;
        case I2C_CMD_SET_CHANNEL:
            if (Wire.available() > 1) {
                value = Wire.read();
                updateMIDIChannel(0, value);
                value = Wire.read();
                updateMIDIChannel(1, value);
            }
            break;
        case I2C_CMD_CALIBRATE:
            if (Wire.available()) {
                value = Wire.read();
                kbd.startLearning(value);
                sendIRQ(IRQ_LEARN);
            }
            break;
    }
}

void i2cRequest() {
    Wire.write(MIDIChannel[0]);
    Wire.write(MIDIChannel[1]);
    Wire.write(kbd.isLearning());
    clearIRQ(IRQ_LEARN);
}

void setup() {
    // Enable pullups for unconnected pins
    pinMode(PIN_PC0, INPUT_PULLUP);
    pinMode(PIN_PC1, INPUT_PULLUP);
    pinMode(PIN_PC2, INPUT_PULLUP);
    pinMode(PIN_PC6, INPUT_PULLUP);
    pinMode(PIN_PD7, INPUT_PULLUP);

    // Set output pin modes
    // Set pin value first before turing on output mode, to prevent spurious signals
    digitalWrite(PIN_INTERRUPT, HIGH);
    pinMode(PIN_INTERRUPT, OUTPUT);

    MIDIChannel[0] = settings.getMIDIChannel(0);
    MIDIChannel[1] = settings.getMIDIChannel(1);

    MIDI.turnThruOn(ThruMode::Full);
    MIDI.begin(MIDIChannel[0]);

    Wire.onReceive(i2cReceive);
    Wire.onRequest(i2cRequest);
    Wire.begin(Controller::MC_Keyboard);

    kbd.setHandleKeyChange(onKeyChange);
    kbd.setLearnCompleteCallback(onLearnComplete);
    kbd.begin();
}

void loop() {
    kbd.poll();
}
