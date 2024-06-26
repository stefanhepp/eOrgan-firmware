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
#include <MIDI.h>
#include <MegaWire.h>

#include <common_config.h>

#include "config.h"
#include "Settings.h"
#include "Keyboard.h"

// Create device driver instances
MIDI_CREATE_DEFAULT_INSTANCE();
MegaWire Wire;

Settings settings;
Keyboard kbd;

static uint8_t MIDIChannel[2];

/**
 * send notes off for all pressed notes.
 * @param force if non-zero, send a note-off controller msg on the current channel.
 **/
static void sendAllNotesOff(uint8_t force)
{
    if ( force ) {
       MIDI.sendControlChange(midi::AllSoundOff, 0, MIDIChannel[0]);
       MIDI.sendControlChange(midi::AllSoundOff, 0, MIDIChannel[1]);
    } else {
       MIDI.sendControlChange(midi::AllNotesOff, 0, MIDIChannel[0]);
       MIDI.sendControlChange(midi::AllNotesOff, 0, MIDIChannel[1]);
    }
}

static void updateMIDIChannel(uint8_t kbd, uint8_t channel) {
    MIDIChannel[kbd] = channel;
    if (kbd == 0) {
        MIDI.setInputChannel(channel);
    }
    settings.setMIDIChannel(kbd, channel);

    // TODO pull ready pin to indicate MIDI channel change
}

static void encoderReset(void)
{
    // disable all notes on current channel
    sendAllNotesOff(1);

    // Reset channel
    updateMIDIChannel(0, MIDI_CHANNEL_KEYBOARD_1);
    updateMIDIChannel(1, MIDI_CHANNEL_KEYBOARD_2);
}

void onKeyChange(uint8_t kbd, uint8_t note, uint8_t velocity) {
    if (velocity > 0) {
        MIDI.sendNoteOn(  note, velocity, MIDIChannel[kbd] );
    } else {
	    // this is a note off event (velocity is 0)
        MIDI.sendNoteOff( note, velocity, MIDIChannel[kbd] );
    }
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

    pinMode(PIN_S0,   OUTPUT);
    pinMode(PIN_S1,   OUTPUT);
    pinMode(PIN_S2,   OUTPUT);
    pinMode(PIN_EN1,  OUTPUT);
    pinMode(PIN_EN2,  OUTPUT);

    Wire.begin(I2C_ADDR_KEYBOARD);

    MIDIChannel[0] = settings.getMIDIChannel(0);
    MIDIChannel[1] = settings.getMIDIChannel(1);

    MIDI.turnThruOff();
    MIDI.begin(MIDIChannel[0]);

    kbd.setHandleKeyChange(onKeyChange);
    kbd.begin();
}

void loop() {
    kbd.poll();
}
