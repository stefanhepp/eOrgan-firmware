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
#include <usb_midi.h>

#include <inttypes.h>
#include <functional>

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


// The current instance of the MIDI router, registered at begin().
// Needed for static callback functions.
static MIDIRouter* Router;

template<MIDIPort inPort>
void processMIDIMessage(const MidiMessage &msg) {
    Router->routeMessage(inPort, msg);
}


MIDIRouter::MIDIRouter()
{
    resetRoutes();
}

void MIDIRouter::resetRoutes()
{
    for (int in = 0; in < NUM_MIDI_PORTS; in++) {
        for (int out = 0; out < NUM_MIDI_OUTPUT_PORTS; out++) {
            // enable all inputs for all output ports that are not on the same port
            mRoutes[in][out].enabled = (out != in);
            // setup default route
            for (int c = 0; c < 16; c++) {
                mRoutes[in][out].channelMap[c] = c;
            }
        }
    }
}

void MIDIRouter::sendMessage(MIDIPort outPort, const MidiMessage &msg)
{
    switch (outPort) {
        case MIDIPort::MP_MIDI1:
            MIDI1.send(msg);
            break;
        case MIDIPort::MP_MIDI2:
            MIDI2.send(msg);
            break;
        case MIDIPort::MP_MIDI3:
            MIDI3.send(msg);
            break;
        case MIDIPort::MP_MIDI4:
            MIDI4.send(msg);
            break;
        case MIDIPort::MP_Pedal:
            MIDIPedal.send(msg);
            break;
        case MIDIPort::MP_Keyboard:
            MIDIKbd.send(msg);
            break;
        case MIDIPort::MP_Technics:
            MIDITechnics.send(msg);
            break;
        case MIDIPort::MP_MIDI_USB:
            if (msg.type != midi::MidiType::SystemExclusive) {
                usbMIDI.send(msg.type, msg.data1, msg.data2, msg.channel, 0);

                // Force sending now? Otherwise may have 1ms delay for packing
                usbMIDI.send_now();
            } else {
                // TODO send SysEx messages to USB ?!            
            }
            break;
        case MIDIPort::MP_MIDI_RTP:

            break;
        case MIDIPort::MP_MIDI_Teensy:

            break;
    }
}

void MIDIRouter::forwardMessage(MIDIPort inPort, MIDIPort outPort, const MidiMessage &msg)
{
    // Check for valid and enabled input
    if (!mRoutes[inPort][outPort].enabled || !msg.valid || msg.type == midi::MidiType::InvalidType) {
        return;
    }

    // TODO filter message types ?!
    
    if (msg.type > midi::MidiType::PitchBend) {
        // System message, no channel
        // Forward as-is on all ports
        sendMessage(outPort, msg);
    } else {
        // Channel message
        if (mRoutes[inPort][outPort].channelMap[msg.channel] != msg.channel)
        {
            // Channel message, and mapped to a different channel
            MidiMessage mappedMsg = msg;
            mappedMsg.channel = mRoutes[inPort][outPort].channelMap[msg.channel];

            if (mappedMsg.channel != MIDI_CHANNEL_OFF) {
                // Forward mapped message, if the input channel is not disabled
                sendMessage(outPort, mappedMsg);
            }
        } else {
            // Forward message as-is
            sendMessage(outPort, msg);
        }
    }
}

void MIDIRouter::routeMessage(MIDIPort inPort, const MidiMessage &msg)
{
    forwardMessage(inPort, MIDIPort::MP_MIDI1, msg);
    forwardMessage(inPort, MIDIPort::MP_MIDI2, msg);
    forwardMessage(inPort, MIDIPort::MP_MIDI3, msg);
    forwardMessage(inPort, MIDIPort::MP_MIDI4, msg);
    forwardMessage(inPort, MIDIPort::MP_MIDI_USB, msg);
}

void MIDIRouter::begin()
{
    // Register this instance for the callback handlers
    Router = this;

    // MIDI forwarding is handled by this router
    MIDI1.turnThruOff();
    MIDI2.turnThruOff();
    MIDI3.turnThruOff();
    MIDI4.turnThruOff();
    MIDIPedal.turnThruOff();
    MIDIKbd.turnThruOff();
    MIDITechnics.turnThruOff();

    // register read callbacks to process received messages
    MIDI1.setHandleMessage(processMIDIMessage<MIDIPort::MP_MIDI1>);
    MIDI2.setHandleMessage(processMIDIMessage<MIDIPort::MP_MIDI2>);
    MIDI3.setHandleMessage(processMIDIMessage<MIDIPort::MP_MIDI3>);
    MIDI4.setHandleMessage(processMIDIMessage<MIDIPort::MP_MIDI4>);
    MIDIPedal.setHandleMessage(processMIDIMessage<MIDIPort::MP_Pedal>);
    MIDIKbd.setHandleMessage(processMIDIMessage<MIDIPort::MP_Keyboard>);
    MIDITechnics.setHandleMessage(processMIDIMessage<MIDIPort::MP_Technics>);

    // Listen on all channels, filtering done by this router
    MIDI1.begin(MIDI_CHANNEL_OMNI);
    MIDI2.begin(MIDI_CHANNEL_OMNI);
    MIDI3.begin(MIDI_CHANNEL_OMNI);
    MIDI4.begin(MIDI_CHANNEL_OMNI);
    MIDIPedal.begin(MIDI_CHANNEL_OMNI);
    MIDIKbd.begin(MIDI_CHANNEL_OMNI);
    MIDITechnics.begin(MIDI_CHANNEL_OMNI);

    usbMIDI.begin();
}

void MIDIRouter::loop()
{
    // Empty all MIDI input queues
    while (MIDI1.read()) {}
    while (MIDI2.read()) {}
    while (MIDI3.read()) {}
    while (MIDI4.read()) {}
    while (MIDIPedal.read()) {}
    while (MIDIKbd.read()) {}
    while (MIDITechnics.read()) {}

    while (usbMIDI.read()) {
        MidiMessage msg;
        msg.type = (midi::MidiType) usbMIDI.getType();
        msg.channel = usbMIDI.getChannel();
        msg.data1 = usbMIDI.getData1();
        msg.data2 = usbMIDI.getData2();
        
        routeMessage(MIDIPort::MP_MIDI_USB, msg);
    }
}