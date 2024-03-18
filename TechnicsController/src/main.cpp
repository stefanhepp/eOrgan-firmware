/**
 * Author: Stefan Hepp (stefan@stefant.org)
 * 
 * Implementation of the pedalboard controller.
 * Reads pedal switches and generates MIDI messages.
 * Configurable either via config switch or I2C interface.
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
#include <MIDI.h>
#include <MegaWire.h>

#include <common_config.h>

#include "config.h"
#include "Settings.h"
#include "TechnicsKeyboard.h"

// Create device driver instances
MIDI_CREATE_DEFAULT_INSTANCE();
MegaWire Wire;

Settings         settings;
TechnicsKeyboard kbd;

static uint8_t MIDIChannel;

static inline char getNote( char key ) 
{
    return key + LOWEST_NOTE;
}

/**
 * send notes off for all pressed notes.
 * @param force if non-zero, send a note-off controller msg on the current channel.
 **/
static void sendAllNotesOff(uint8_t force)
{
    if ( force ) {
       MIDI.sendControlChange(midi::AllSoundOff, 0, MIDIChannel);
    } else {
       MIDI.sendControlChange(midi::AllNotesOff, 0, MIDIChannel);
    }
}

static void updateMIDIChannel(uint8_t channel) {
    MIDIChannel = channel;
    MIDI.setInputChannel(channel);
    settings.setMIDIChannel(channel);

    // TODO pull ready pin to indicate MIDI channel change
}

static void encoderReset(void)
{
    // disable all notes on current channel
    sendAllNotesOff(1);

    // Reset channel
    updateMIDIChannel(MIDI_CHANNEL_TECHNICS);
}

void onKeyChange(uint8_t key, uint8_t velocity) {
    if (velocity > 0) {
        MIDI.sendNoteOn(  getNote(key), velocity, MIDIChannel );
    } else {
	    // this is a note off event (velocity is 0)
        MIDI.sendNoteOff( getNote(key), velocity, MIDIChannel );
    }
}

void setup() {
    // Enable pullups for unconnected pins
    pinMode(PIN_PC1, INPUT_PULLUP);
    pinMode(PIN_PC2, INPUT_PULLUP);
    pinMode(PIN_PC3, INPUT_PULLUP);
    pinMode(PIN_PC6, INPUT_PULLUP);
    pinMode(PIN_PD2, INPUT_PULLUP);

    // Set output pin modes
    // Set pin value first before turing on output mode, to prevent spurious signals
    digitalWrite(PIN_INTERRUPT, HIGH);
    pinMode(PIN_INTERRUPT, OUTPUT);

    pinMode(PIN_READ,   INPUT);
    pinMode(PIN_ENABLE, OUTPUT);
    pinMode(PIN_LATCH,  OUTPUT);
    pinMode(PIN_CLOCK,  OUTPUT);

    Wire.begin(I2C_ADDR_TECHNICS);

    MIDIChannel = settings.getMIDIChannel();
    MIDI.turnThruOff();
    MIDI.begin(MIDIChannel);

    kbd.setHandleKeyChange(onKeyChange);
    kbd.begin();
}

void loop() {
    kbd.poll();
}
