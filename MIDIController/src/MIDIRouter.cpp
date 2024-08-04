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
    mEchoMIDI = false;
    resetRoutes();
}

void MIDIRouter::echoMIDIMessages(bool enabled) {
    mEchoMIDI = enabled;
}

void MIDIRouter::printNote(int note) const {
    Serial.printf("N%d");
}

void MIDIRouter::printMessage(const MidiMessage &msg) const {
    if (msg.type < midi::MidiType::SystemExclusive) {
        // Channel message 
        Serial.printf("C%hhu ", msg.channel);
    }
    int pitch = 0;
    switch (msg.type) {
        case midi::MidiType::InvalidType:
            Serial.println("Invalid");
            break;
        case midi::MidiType::NoteOn:
            Serial.print("NoteOn ");
            printNote(msg.data1);
            Serial.printf(" V%hhu\n", msg.data2);
            break;
        case midi::MidiType::NoteOff:
            Serial.print("NoteOn ");
            printNote(msg.data1);
            Serial.printf(" V%hhu\n", msg.data2);
            break;
        case midi::MidiType::AfterTouchPoly:
            Serial.print("Aftertouch ");
            printNote(msg.data1);
            Serial.printf(" V%hhu\n", msg.data2);
            break;
        case midi::MidiType::ControlChange:
            Serial.printf("CC %hhu=%hhu\n", msg.data1, msg.data2);
            break;
        case midi::MidiType::ProgramChange:
            Serial.printf("PC %hhu\n", msg.data1);
            break;
        case midi::MidiType::AfterTouchChannel:
            Serial.printf("Channel Aftertouch V%hhu\n", msg.data1);
            break;
        case midi::MidiType::PitchBend:
            pitch = (((int)(msg.data1)<<8)|(int)msg.data2) - 0x2000;
            Serial.printf("Bend %d\n", pitch);
            break;
        default:
            // System message
            Serial.printf("Sys T%02hhX D1:%hhu D2:%hhu", msg.type, msg.data1, msg.data2);
            break;
    }
    Serial.flush();
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

void MIDIRouter::forwardMessage(MIDIPort inPort, MIDIPort outPort, const MidiMessage &msg, bool echo)
{
    // Check for valid and enabled input
    if (!mRoutes[inPort][outPort].enabled || !msg.valid || msg.type == midi::MidiType::InvalidType) {
        return;
    }

    // TODO filter message types ?!
    
    if (msg.type >= midi::MidiType::SystemExclusive) {
        // System message, no channel
        // Forward as-is on all ports
        if (echo) {
            Serial.printf(" O%hhu", outPort); 
        }
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
                if (echo) {
                    Serial.printf(" O%hhu:C%hhu", outPort, mappedMsg.channel); 
                }
                sendMessage(outPort, mappedMsg);
            }
        } else {
            // Forward message as-is
            if (echo) {
                Serial.printf(" O%hhu", outPort); 
            }
            sendMessage(outPort, msg);
        }
    }
}

void MIDIRouter::routeMessage(MIDIPort inPort, const MidiMessage &msg)
{
    // get echo mode in the beginning, to print consistently if the mode is changed mid-sending.
    bool echo = mEchoMIDI;

    if (echo) {
        Serial.printf("MIDI In %d: ", inPort);
        printMessage(msg);
        Serial.print(" ->");
    }

    forwardMessage(inPort, MIDIPort::MP_MIDI1, msg, echo);
    forwardMessage(inPort, MIDIPort::MP_MIDI2, msg, echo);
    forwardMessage(inPort, MIDIPort::MP_MIDI3, msg, echo);
    forwardMessage(inPort, MIDIPort::MP_MIDI4, msg, echo);
    forwardMessage(inPort, MIDIPort::MP_MIDI_USB, msg, echo);

    if (echo) {
        Serial.println();
    }
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