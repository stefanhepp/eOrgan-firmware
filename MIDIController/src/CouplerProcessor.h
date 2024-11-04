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

enum CoupleMode : int {
    CM_OFF = 0,
    CM_COUPLE = 1,
    CM_OCTAVE_UP = 2,
    CM_OCTAVE_DOWN = 3
};

enum ButtonType : int {
    BT_NONE = 0,
    BT_NEXT = 4,
    BT_PREV = 5,
    BT_TOGGLE = 6
};

union PistonParameter {
    int          value;
    MIDIDivision division;
    ButtonType   button;
};

enum PistonCommandType {
    PCT_NONE,         // Not a valid input or no action defined
    PCT_COUPLER,      // coupler piston; Param.division: division to couple to
    PCT_TRANSPOSE,    // transpose piston; Param.type: TOGGLE
    PCT_OFF,          // turn division off
    PCT_COMBINATION,  // select preset; Param.value: number of preset
    PCT_CLEAR,        // clear combination
    PCT_CRESCENDO,    // crescendo coupler
    PCT_PAGE,         // Page turn; Param.type: PREV, NEXT
    PCT_SEQUENCE,     // Sequencer; Param.type: PREV, NEXT
    PCT_SET,          // Set combination
    PCT_HOLD          // Hold current combination
};

struct PistonCommand {
    MIDIDivision        division;
    PistonCommandType   type;
    PistonParameter     param;
};

struct CouplerStatus {
    bool enabled;
    bool crescendo;
    // Sets the coupler mode: PP_NONE (off), PP_COUPLE (coupled), PP_OCTAVE_UP, PP_OCTAVE_DOWN
    CoupleMode couple[MAX_DIVISION_CHANNEL+1];
};

class CouplerProcessor
{
    private:
        MIDIRouter &mMIDIRouter;

        PistonCommand mPistonCommands[MAX_DIVISION_CHANNEL + 1][MAX_PISTONS];

        MIDIDivision mChannelDivisions[NUM_MIDI_CHANNELS];

        midi::Channel mDivisionChannels[MAX_DIVISION_CHANNEL + 1];

        CouplerStatus mCoupler[MAX_DIVISION_CHANNEL + 1];

        uint16_t mPedalCrescendo = 0;
        uint16_t mPedalSwell = 0;
        uint16_t mPedalChoir = 0;

        /**
         * If true, send MIDI commands for coupler actions instead of
         * performing the coupler translations.
         */
        bool mSendMIDICommands;

        MIDIDivision getDivision(MIDIPort inPort, const MidiMessage &msg);

        /**
         * \return True if the message was a pedal message and was processed, else false.
         */
        bool processPedalMessage(MIDIDivision division, const MidiMessage &msg);

        void sendCouplerMessage(MIDIDivision division, MIDIDivision target, 
                                MIDIPort inPort, const MidiMessage &msg);

    public:
        explicit CouplerProcessor(MIDIRouter &router);

        void setDivisionChannel(MIDIDivision division, uint8_t channel);

        void sendMIDICommands(bool sendCommands);

        bool doSendMIDICommands() const { return mSendMIDICommands; }


        /**
         * Clear all couplers for a division.
         */
        void clearCouplers(MIDIDivision division);

        /**
         * \param mode Can be one of PP_NONE (off), PP_COUPLE, PP_OCTAVE_UP, PP_OCTAVE_DOWN.
         */
        void coupleDivision(MIDIDivision division, MIDIDivision target, CoupleMode mode);

        /**
         * \param param Can be one of PP_NONE (off), PP_OCTAVE_UP, PP_OCTAVE_DOWN.
         */
        void transposeDivision(MIDIDivision division, CoupleMode mode);

        void enableCrescendo(MIDIDivision division, bool crescendo);

        /**
         * \param output Enable or disable normal output of division (not including transposed or couplers).
         */
        void enableDivision(MIDIDivision, bool output);


        CoupleMode coupled(MIDIDivision division, MIDIDivision target) const;

        CoupleMode transposed(MIDIDivision division) const;

        bool crescendo(MIDIDivision division) const;

        bool enabled(MIDIDivision division) const;

        
        void selectCombination(MIDIDivision division, int combination);

        void clearCombination(MIDIDivision division);


        void processPistonPress(MIDIDivision division, uint8_t button, bool longPress);

        void processCrescendoChange(uint16_t crescendo);

        void processPedalChange(MIDIDivision division, uint16_t value);

        void routeDivisionInput(MIDIPort inPort, const MidiMessage &msg);

        void begin();
};
