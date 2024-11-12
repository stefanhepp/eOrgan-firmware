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

struct NoteStatus {
    // This is a bitmask of source divisions playing this note
    uint16_t sourceMask;
    // true if the note is played by the division manual
    bool     pressed;
    // Velocity of the original note
    uint8_t  velocity;
};

static const int COUPLER_LOWEST_NOTE = 16;
// 77key keyboard plus transposed octaves
static const int COUPLER_NUM_NOTES = 77 + 2*12;

class CouplerProcessor
{
    private:
        MIDIRouter &mMIDIRouter;

        PistonCommand mPistonCommands[MAX_DIVISION_CHANNEL + 1][MAX_PISTONS];

        MIDIDivision mChannelDivisions[NUM_MIDI_CHANNELS];

        midi::Channel mDivisionChannels[MAX_DIVISION_CHANNEL + 1];

        MIDIPort mInjectPorts[MAX_DIVISION_CHANNEL + 1];

        CouplerStatus mCoupler[MAX_DIVISION_CHANNEL + 1];

        NoteStatus mPressedNotes[MAX_DIVISION_CHANNEL+1][COUPLER_NUM_NOTES];

        uint16_t mPedalCrescendo = 0;
        uint16_t mPedalSwell = 0;
        uint16_t mPedalChoir = 0;

        /**
         * If true, send MIDI commands for coupler actions instead of
         * performing the coupler translations.
         */
        bool mSendMIDICommands;

        /**
         * Get the division for a MIDI message.
         * \param inPort where the message is received from.
         * \param msg the MIDI message.
         */
        MIDIDivision getDivision(MIDIPort inPort, const MidiMessage &msg);

        NoteStatus &getNoteStatus(MIDIDivision division, int note);

        void sendCouplerNoteOn(MIDIDivision division, int note, uint8_t velocity);

        void sendCouplerNoteOff(MIDIDivision division, int note);

        /**
         * Send MIDI messages for any active coupler for a received input message.
         * \param division the source division of the MIDI message.
         * \param target the target division to check the coupler and send any coupler message.
         * \param msg the source MIDI message.
         */
        void sendCouplerMessage(MIDIDivision division, MIDIDivision target, const MidiMessage &msg);

        /**
         * Play a note on a division.
         * 
         * \param sendMidi if true, send a NoteOn if this note is not yet played.
         */
        void setCoupledNote(MIDIDivision source, MIDIDivision target, int sourceNote, int note, bool sendMidi);

        /**
         * Stop playing a note on a division.
         * 
         * \param sendMidi if true, send a NoteOff if this note is not played from any division anymore.
         */
        void clearCoupledNote(MIDIDivision source, MIDIDivision target, int note, bool sendMidi);

        void updateCouplerMode(MIDIDivision source, MIDIDivision target, CoupleMode mode);

        void updateCoupledNote(MIDIDivision source, MIDIDivision target, const MidiMessage &msg, const MidiMessage &cmsg);

        /**
         * Update NoteStatus for a received input MIDI message.
         */
        void updatePlayedNote(MIDIDivision souce, const MidiMessage &msg);

        uint8_t getTransposedNote(CoupleMode mode, uint8_t note);

    public:
        explicit CouplerProcessor(MIDIRouter &router);

        void setDivisionChannel(MIDIDivision division, uint8_t channel);

        void sendMIDICommands(bool sendCommands);

        bool doSendMIDICommands() const { return mSendMIDICommands; }


        /**
         * Reset coupler states and turn all notes off.
         */
        void reset();

        /**
         * Turn all notes off on all divisions.
         */
        void allCouplerNotesOff();

        /**
         * Turn all notes off that are coupled from this division.
         */
        void allCouplerNotesOff(MIDIDivision division);

        /**
         * Turn all notes playing on a division off.
         */
        void allDivisionNotesOff(MIDIDivision division);

        /**
         * Send a page turn command.
         * 
         * \param direction BT_NEXT or BT_PREV.
         */
        void sendPageTurn(ButtonType direction);

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
