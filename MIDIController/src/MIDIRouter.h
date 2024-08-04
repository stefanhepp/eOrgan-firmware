/**
 * @project     MidiController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * MIDI router implementatoin
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#include <MIDI.h>

#include <inttypes.h>

#include <common_config.h>

struct RouteSettings {
    bool enabled;
    // Mapping of input channel -> output channel
    uint8_t channelMap[16];
};

typedef midi::Message<midi::DefaultSettings::SysExMaxSize> MidiMessage;

class MIDIRouter
{
    private:
        bool mEchoMIDI;

        // Routing settings, for each input channel -> for each output channel.
        RouteSettings mRoutes[NUM_MIDI_PORTS][NUM_MIDI_OUTPUT_PORTS];

        void printNote(int note) const;

        void printMessage(const MidiMessage &msg) const;

        /**
         * Send a message to an output port (no filtering or routing)
         */
        void sendMessage(MIDIPort outPort, const MidiMessage &msg);

        /**
         * Forward mesage from input to output port, applying the route filters
         */
        void forwardMessage(MIDIPort inPort, MIDIPort outPort, const MidiMessage &msg, bool echo);

    public:
        explicit MIDIRouter();

        void echoMIDIMessages(bool enable);

        void resetRoutes();

        // TODO enable/disable routes

        // TODO set channel mapping per input -> output

        void routeMessage(MIDIPort inPort, const MidiMessage &msg);

        void begin();

        void loop();
};
