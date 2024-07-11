/*
 * @project     MIDIController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * MIDI Router implementation
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "MIDIRouter.h"

#include <MIDI.h>
//#include <USB-MIDI.h>

#include <inttypes.h>

#include <common_config.h>


// MIDI Devices
MIDI_CREATE_INSTANCE(HardwareSerial, Serial7,    MIDI1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1,    MIDI2);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3,    MIDI3);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial4,    MIDI4);
// TXD8 is not connected
MIDI_CREATE_INSTANCE(HardwareSerial, Serial8,    MIDIPedal);
// TXD2 is on I2S audio header
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2,    MIDIKbd);
// TXD3 is shared with SPDIF_OUT, on I2S audio header
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3,    MIDITechnics);

//USBMIDI_CREATE_DEFAULT_INSTANCE();


MIDIRouter::MIDIRouter()
{
}

void MIDIRouter::begin()
{
    MIDI1.begin(1);
    MIDI2.begin(2);
    MIDI3.begin(3);
    MIDI4.begin(4);
    MIDIPedal.begin(1);
    MIDIKbd.begin(4);
    MIDITechnics.begin(2);

    MIDI1.turnThruOff();
    MIDI2.turnThruOff();
    MIDI3.turnThruOff();
    MIDI4.turnThruOff();
    MIDIPedal.turnThruOff();
    MIDIKbd.turnThruOff();
    MIDITechnics.turnThruOff();

}

void MIDIRouter::loop()
{

}