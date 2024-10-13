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

class CouplerProcessor;

/**
 * Print the name of the note into the output string.
 * 
 * @param string Output null-terminated name of the note, must be at least 4 characters long.
 */
void noteToString(int note, char* string);

class MIDIRouter
{
    private:
        bool mEchoMIDI = false;

        CouplerProcessor *mCoupler = NULL;

        bool mEnableCoupler = false;
        bool mEnableUSB = true;
        bool mEnableMIDIOut = true;

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

        void setCoupler(CouplerProcessor &coupler);

        void echoMIDIMessages(bool enable);

        /**
         * Helper function to set message length
         */
        void setMessageLength(MidiMessage &msg) const;

        void resetRoutes();

        void enableCoupler(bool enable);

        void enableUSBOutput(bool enable);

        void enableMIDIOutput(bool enable);

        // TODO enable/disable routes

        // TODO set channel mapping per input -> output

        /**
         * Process a new incoming MIDI message.
         * 
         * This sends the message through the coupler and the MIDI router.
         */
        void routeMessage(MIDIPort inPort, const MidiMessage &msg);

        /**
         * Inject a message after the coupler into the router.
         * 
         * This is intended to be called by the coupler.
         */
        void injectMessage(MIDIPort inPort, const MidiMessage &msg);

        void begin();

        void loop();
};
