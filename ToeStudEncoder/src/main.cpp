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
#include <CalibratedAnalogInput.h>

#include <common_config.h>

#include "config.h"
#include "Settings.h"
#include "Keyboard.h"

// Create device driver instances
MIDI_CREATE_DEFAULT_INSTANCE();
MegaWire Wire;

Settings settings;
Keyboard kbd;

CalibratedAnalogInput volumeChoir(PIN_VOL_CHOIR);
CalibratedAnalogInput volumeSwell(PIN_VOL_SWELL);
CalibratedAnalogInput crescendo(PIN_CRESENDO);

static uint8_t MIDIChannel;

/**
 * send notes off for all pressed notes.
 * @param force if non-zero, send a note-off controller msg on the current channel.
 **/
static void sendAllNotesOff(uint8_t force)
{
    if ( force ) {
       MIDI.sendControlChange(midi::AllSoundOff, 0, MIDIChannel;
    } else {
       MIDI.sendControlChange(midi::AllNotesOff, 0, MIDIChannel);
    }
}

static void updateMIDIChannel(uint8_t channel) {
    MIDIChannel = channel;
    settings.setMIDIChannel(channel);
}

static void resetEncoder(void)
{
    // disable all notes on current channel
    sendAllNotesOff(1);

    // Reset channel
    updateMIDIChannel(MIDI_CHANNEL_TOESTUD);
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

    pinMode(PIN_L1,   OUTPUT);
    pinMode(PIN_L2,   OUTPUT);
    pinMode(PIN_L3,   OUTPUT);
    pinMode(PIN_L4,   OUTPUT);
    pinMode(PIN_D1,   OUTPUT);
    pinMode(PIN_D2,   OUTPUT);
    pinMode(PIN_D3,   OUTPUT);
    pinMode(PIN_D4,   OUTPUT);
    pinMode(PIN_D5,   OUTPUT);

    Wire.begin(I2C_ADDR_TOESTUD);

    MIDIChannel = settings.getMIDIChannel();

    MIDI.turnThruOn(midi::Thru::Full);
    MIDI.begin(MIDIChannel);

    kbd.begin();
}

void loop() {
    kbd.poll();
}
