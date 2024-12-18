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

enum CouplerMode {
    CM_DISABLED = 0x00,
    CM_MIDI = 0x01,
    CM_ENABLED = 0x02
};

enum CouplerState : int {
    CS_OFF = 0,
    CS_COUPLE = 1,
    CS_OCTAVE_UP = 2,
    CS_OCTAVE_DOWN = 3
};

enum ButtonType : int {
    BT_NONE = 0,
    BT_NEXT = 4,
    BT_PREV = 5,
    BT_TOGGLE = 6,
    BT_ALL = 7
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
    PCT_HOLD,         // Hold current combination
    PCT_SOUNDOFF      // Send AllSoundOff; Param.value: ALL = all divisions, else param.division
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
    CouplerState couple[MAX_DIVISION_CHANNEL+1];
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


// NRPN numbers for specific coupler events
enum NRPNEvent : int {
    NRPN_Set             = 101,
    NRPN_Clear           = 102,
    NRPN_Hold            = 103,
    NRPN_EnableCrescendo = 104,
    NRPN_Off             = 105,
    NRPN_ClearCouplers   = 106,
    NRPN_SequencerPrev   = 107,
    NRPN_SequencerNext   = 108
};

static const int NRPN_COUPLER_OFFSET = 200;
static const int NRPN_STOPS_OFFSET = 1000;

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

        bool mDebug = false;

        /**
         * Mode of the coupler processor:
         * - CM_DISABLED: all output off
         * - CM_MIDI: Send MIDI commands for stops and couplers
         * - CM_ENABLED: Transform MIDI notes with couplers, send stops as MIDI commands.
         */
        CouplerMode mCouplerMode;

        /**
         * If enabled, setting of general or divisional combinations is enabled.
         */
        bool mSettingCombination = false;

        /**
         * If enabled, changing stops only has effect when holding is disabled.
         */
        bool mHoldingCombination = false;

        /**
         * Get the division for a MIDI message.
         * \param inPort where the message is received from.
         * \param msg the MIDI message.
         */
        MIDIDivision getDivision(MIDIPort inPort, const MidiMessage &msg);

        NoteStatus &getNoteStatus(MIDIDivision division, int note);

        /**
         * Send MIDI note on message on a given division.
         * 
         * MIDI message is sent regardless of CouplerMode.
         */
        void sendCouplerNoteOn(MIDIDivision division, int note, uint8_t velocity);

        /**
         * Send MIDI note off message on a given division.
         * 
         * MIDI message is sent regardless of CouplerMode.
         */
        void sendCouplerNoteOff(MIDIDivision division, int note);

        /**
         * Send MIDI CC message on a given division.
         * 
         * MIDI message is sent regardless of CouplerMode.
         */
        void sendControlChange(MIDIDivision division, midi::MidiControlChangeNumber ccNumber, uint8_t value);

        /**
         * Send MIDI Non-Registered Parameter Number message on a given division.
         * 
         * MIDI message is sent regardless of CouplerMode.
         */
        void sendNRPN(MIDIDivision division, int parameterNumber, uint8_t value);

        /**
         * Play a note on a division.
         * 
         * \param sendMidi if true, send a NoteOn if this note is not yet played.
         */
        void playCoupledNote(MIDIDivision source, MIDIDivision target, int sourceNote, int note, bool sendMidi);

        /**
         * Stop playing a note on a division.
         * 
         * \param sendMidi if true, send a NoteOff if this note is not played from any division anymore.
         */
        void clearCoupledNote(MIDIDivision source, MIDIDivision target, int note, bool sendMidi);

        /** 
         * Change the state of a coupler.
         * 
         * If CouplerMode == Enabled, then send MIDI Note On/Off messages for affected pressed keys.
         */
        void updateCouplerMode(MIDIDivision source, MIDIDivision target, CouplerState mode);

        /**
         * Record the state of a coupled Note On/off message.
         */
        void recordCoupledNote(MIDIDivision source, MIDIDivision target, const MidiMessage &msg, const MidiMessage &cmsg);

        /**
         * Update NoteStatus for a received input MIDI message.
         */
        void recordPlayedNote(MIDIDivision source, const MidiMessage &msg);

        uint8_t getTransposedNote(CouplerState mode, uint8_t note);

        int getCouplerNRPN(MIDIDivision target, CouplerState mode);

        /**
         * Create MIDI messages for coupled notes for a target division for a received division manual message.
         * 
         * \param division the source division of the MIDI message.
         * \param target the target division to check the coupler and send any coupled message for.
         * \param msg the source MIDI message on <division>.
         */
        void sendCouplerMessage(MIDIDivision division, MIDIDivision target, const MidiMessage &msg);

    public:
        explicit CouplerProcessor(MIDIRouter &router);

        void setDivisionChannel(MIDIDivision division, uint8_t channel);


        void setCouplerMode(CouplerMode mode);

        CouplerMode couplerMode() const { return mCouplerMode; }


        void setDebug(bool debug) { mDebug = debug; }

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

        void allDivisionsNotesOff(bool soundOff = false);

        /**
         * Turn all notes playing on a division off.
         * 
         * \param soundOff send AllSoundOff instead of AllNotesOff
         */
        void allDivisionNotesOff(MIDIDivision division, bool soundOff = false);

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
        void coupleDivision(MIDIDivision division, MIDIDivision target, CouplerState mode);

        /**
         * \param param Can be one of PP_NONE (off), PP_OCTAVE_UP, PP_OCTAVE_DOWN.
         */
        void transposeDivision(MIDIDivision division, CouplerState mode);

        void enableCrescendo(MIDIDivision division, bool crescendo);

        /**
         * \param output Enable or disable normal output of division (not including transposed or couplers).
         */
        void enableDivision(MIDIDivision, bool output);


        CouplerState coupled(MIDIDivision division, MIDIDivision target) const;

        CouplerState transposed(MIDIDivision division) const;

        bool crescendo(MIDIDivision division) const;

        bool enabled(MIDIDivision division) const;
        
        void selectCombination(MIDIDivision division, int combination);

        void clearCombination(MIDIDivision division);


        bool settingCombination() const { return mSettingCombination; }

        bool holdingCombination() const { return mHoldingCombination; }

        void setCombination(bool enable);

        void holdCombination(bool enable);


        void selectSequencer(ButtonType button);


        void processPistonPress(MIDIDivision division, uint8_t button, bool longPress);

        void processCrescendoChange(uint16_t crescendo);

        void processPedalChange(MIDIDivision division, uint16_t value);

        void routeDivisionInput(MIDIPort inPort, const MidiMessage &msg);

        void begin();
};
