/**
 * @project     MidiController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Division coupler and piston controller.
 * Filters MIDI router division input messages and forwards them to division outputs.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#include <MIDI.h>

#include <inttypes.h>

#include <common_config.h>

#include "MIDIRouter.h"

enum PistonParameter : int {
    PP_NEXT = 0,
    PP_PREV = 1,
    PP_TOGGLE = 2,
    PP_OCTAVE_DOWN = 3,
    PP_OCTAVE_UP = 4
};

enum PistonCommandType {
    PCT_NONE,         // Not a valid input or no action defined
    PCT_COUPLER,      // coupler piston; Param: division to couple to
    PCT_TRANSPOSE,    // transpose piston; Param: TOGGLE, OCTAVE_DOWN, OCTAVE_UP
    PCT_OFF,          // turn division off
    PCT_COMBINATION,  // select preset; Param: number of preset
    PCT_CLEAR,        // clear combination
    PCT_CRESCENDO,    // crescendo coupler
    PCT_PAGE,         // Page turn; Param: PREV, NEXT
    PCT_SEQUENCE,     // Sequencer; Param: PREV, NEXT
    PCT_SET,          // Set combination
    PCT_HOLD          // Hold current combination
};

struct PistonCommand {
    MIDIDivision        division;
    PistonCommandType   type;
    int                 param;
};

class CouplerProcessor
{
    private:
        MIDIRouter &mMIDIRouter;

        PistonCommand mPistonCommands[MAX_DIVISION_CHANNEL + 1][MAX_PISTONS];

        MIDIDivision mChannelDivisions[NUM_MIDI_CHANNELS];

        midi::Channel mDivisionChannels[MAX_DIVISION_CHANNEL + 1];

        uint16_t mPedalCrescendo = 0;
        uint16_t mPedalSwell = 0;
        uint16_t mPedalChoir = 0;

        MIDIDivision getDivision(MIDIPort inPort, const MidiMessage &msg);

        /**
         * \return True if the message was a pedal message and was processed, else false.
         */
        bool processPedalMessage(MIDIDivision, const MidiMessage &msg);

    public:
        explicit CouplerProcessor(MIDIRouter &router);

        void setDivisionChannel(MIDIDivision division, uint8_t channel);

        void processPistonPress(MIDIDivision division, uint8_t button, bool longPress);

        void processPedalChange(uint16_t crescendo, uint16_t swell, uint16_t choir);

        void routeDivisionInput(MIDIPort inPort, const MidiMessage &msg);

        void begin();
};
