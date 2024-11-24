/*
 * @project     MIDIController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Division coupler implementation
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "CouplerProcessor.h"

#include <MIDI.h>

#include <inttypes.h>

#include <common_config.h>

static const int COUPLER_NUM_SOUND_DIVISIONS = 5;
static const int COUPLER_NUM_DIVISIONS = 6;

static const MIDIDivision COUPLER_DIVISIONS[COUPLER_NUM_DIVISIONS] = {
    MIDIDivision::MD_Pedal, 
    MIDIDivision::MD_Choir,
    MIDIDivision::MD_Great,
    MIDIDivision::MD_Swell,
    MIDIDivision::MD_Solo,
    MIDIDivision::MD_Control
};

CouplerProcessor::CouplerProcessor(MIDIRouter &router)
: mMIDIRouter(router), mCouplerMode(CouplerMode::CM_ENABLED)
{
    // Map division -> channel
    mDivisionChannels[MIDIDivision::MD_Pedal] = MIDIDivision::MD_Pedal;
    mDivisionChannels[MIDIDivision::MD_Choir] = MIDIDivision::MD_Choir;
    mDivisionChannels[MIDIDivision::MD_Great] = MIDIDivision::MD_Great;
    mDivisionChannels[MIDIDivision::MD_Swell] = MIDIDivision::MD_Swell;
    mDivisionChannels[MIDIDivision::MD_Solo ] = MIDIDivision::MD_Solo;
    mDivisionChannels[MIDIDivision::MD_Control] = MIDIDivision::MD_Control;

    // Map channel -> division
    for (int i = 0; i < NUM_MIDI_CHANNELS; i++) {
        mChannelDivisions[i] = MIDIDivision::MD_MIDI;
    }
    mChannelDivisions[MIDIDivision::MD_Pedal] = MIDIDivision::MD_Pedal;
    mChannelDivisions[MIDIDivision::MD_Choir] = MIDIDivision::MD_Choir;
    mChannelDivisions[MIDIDivision::MD_Great] = MIDIDivision::MD_Great;
    mChannelDivisions[MIDIDivision::MD_Swell] = MIDIDivision::MD_Swell;
    mChannelDivisions[MIDIDivision::MD_Solo ] = MIDIDivision::MD_Solo;
    mChannelDivisions[MIDIDivision::MD_Control] = MIDIDivision::MD_Control;

    // Map divisions to MIDI ports
    for (int i = 0; i < MAX_DIVISION_CHANNEL + 1; i++) {
        mInjectPorts[i] = MIDIPort::MP_MIDI1;
    }
    mInjectPorts[MIDIDivision::MD_Pedal] = MIDIPort::MP_Pedal;
    mInjectPorts[MIDIDivision::MD_Choir] = MIDIPort::MP_Technics;
    mInjectPorts[MIDIDivision::MD_Great] = MIDIPort::MP_MIDI1;
    mInjectPorts[MIDIDivision::MD_Swell] = MIDIPort::MP_Keyboard;
    mInjectPorts[MIDIDivision::MD_Solo]  = MIDIPort::MP_Keyboard;

    // initialize piston command mapping
    for (int i = 0; i < MAX_DIVISION_CHANNEL + 1; i++) {
        for (int j = 0; j < MAX_PISTONS; j++) {
            mPistonCommands[i][j].type = PCT_NONE;
            mPistonCommands[i][j].division = MIDIDivision::MD_MIDI;
            mPistonCommands[i][j].param.value = 0;
        }
    }

    mPistonCommands[MD_Control][2]  = {.division = MD_Control, .type = PCT_SOUNDOFF, .param = BT_ALL};        // Top row, right (right to left)
    mPistonCommands[MD_Control][3]  = {.division = MD_Control, .type = PCT_SEQUENCE, .param = BT_NEXT};
    mPistonCommands[MD_Control][4]  = {.division = MD_Control, .type = PCT_SEQUENCE, .param = BT_PREV};
    mPistonCommands[MD_Control][5]  = {.division = MD_Pedal,   .type = PCT_COMBINATION, .param = 4}; // Bottom row, right (right to left)
    mPistonCommands[MD_Control][6]  = {.division = MD_Pedal,   .type = PCT_COMBINATION, .param = 3};
    mPistonCommands[MD_Control][7]  = {.division = MD_Pedal,   .type = PCT_COMBINATION, .param = 2};
    mPistonCommands[MD_Control][8]  = {.division = MD_Pedal,   .type = PCT_COMBINATION, .param = 1};
    mPistonCommands[MD_Control][9]  = {.division = MD_Control, .type = PCT_PAGE, .param = BT_NEXT};
    mPistonCommands[MD_Control][11] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 5}; // Top row, left
    mPistonCommands[MD_Control][12] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 6};
    mPistonCommands[MD_Control][13] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 7};
    mPistonCommands[MD_Control][14] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 8};
    mPistonCommands[MD_Control][15] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 1}; // Bottom row, left
    mPistonCommands[MD_Control][16] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 2};
    mPistonCommands[MD_Control][17] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 3};
    mPistonCommands[MD_Control][18] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 4};
    mPistonCommands[MD_Control][19] = {.division = MD_Control, .type = PCT_PAGE, .param = BT_PREV};

    mPistonCommands[MD_Pedal][0]  = {.division = MD_Pedal, .type = PCT_CRESCENDO, .param = 0};
    mPistonCommands[MD_Pedal][1]  = {.division = MD_Pedal, .type = PCT_CLEAR,     .param = 0};
    mPistonCommands[MD_Pedal][2]  = {.division = MD_Pedal, .type = PCT_COMBINATION, .param = 5};
    mPistonCommands[MD_Pedal][3]  = {.division = MD_Pedal, .type = PCT_COMBINATION, .param = 4};
    mPistonCommands[MD_Pedal][4]  = {.division = MD_Pedal, .type = PCT_COMBINATION, .param = 3};
    mPistonCommands[MD_Pedal][5]  = {.division = MD_Pedal, .type = PCT_COMBINATION, .param = 2};
    mPistonCommands[MD_Pedal][6]  = {.division = MD_Pedal, .type = PCT_COMBINATION, .param = 1};
    mPistonCommands[MD_Pedal][7]  = {.division = MD_Pedal, .type = PCT_OFF,       .param = 0};
    mPistonCommands[MD_Pedal][8]  = {.division = MD_Pedal, .type = PCT_TRANSPOSE, .param = BT_TOGGLE};
    mPistonCommands[MD_Pedal][9]  = {.division = MD_Pedal, .type = PCT_COUPLER, .param = MIDIDivision::MD_Choir};
    mPistonCommands[MD_Pedal][10] = {.division = MD_Pedal, .type = PCT_COUPLER, .param = MIDIDivision::MD_Great};
    mPistonCommands[MD_Pedal][11] = {.division = MD_Pedal, .type = PCT_COUPLER, .param = MIDIDivision::MD_Swell};
    mPistonCommands[MD_Pedal][17] = {.division = MD_Pedal, .type = PCT_COUPLER, .param = MIDIDivision::MD_Solo};

    mPistonCommands[MD_Choir][0]  = {.division = MD_Choir, .type = PCT_CRESCENDO, .param = 0};
    mPistonCommands[MD_Choir][1]  = {.division = MD_Choir, .type = PCT_CLEAR,     .param = 0};
    mPistonCommands[MD_Choir][2]  = {.division = MD_Choir, .type = PCT_COMBINATION, .param = 5};
    mPistonCommands[MD_Choir][3]  = {.division = MD_Choir, .type = PCT_COMBINATION, .param = 4};
    mPistonCommands[MD_Choir][4]  = {.division = MD_Choir, .type = PCT_COMBINATION, .param = 3};
    mPistonCommands[MD_Choir][5]  = {.division = MD_Choir, .type = PCT_COMBINATION, .param = 2};
    mPistonCommands[MD_Choir][6]  = {.division = MD_Choir, .type = PCT_COMBINATION, .param = 1};
    mPistonCommands[MD_Choir][7]  = {.division = MD_Choir, .type = PCT_OFF,       .param = 0};
    mPistonCommands[MD_Choir][8]  = {.division = MD_Choir, .type = PCT_TRANSPOSE, .param = BT_TOGGLE};
    mPistonCommands[MD_Choir][9]  = {.division = MD_Choir, .type = PCT_COUPLER, .param = MIDIDivision::MD_Pedal};
    mPistonCommands[MD_Choir][10] = {.division = MD_Choir, .type = PCT_COUPLER, .param = MIDIDivision::MD_Swell};
    mPistonCommands[MD_Choir][11] = {.division = MD_Choir, .type = PCT_COUPLER, .param = MIDIDivision::MD_Solo};

    mPistonCommands[MD_Swell][0]  = {.division = MD_Swell, .type = PCT_CRESCENDO, .param = 0};
    mPistonCommands[MD_Swell][1]  = {.division = MD_Swell, .type = PCT_CLEAR,     .param = 0};
    mPistonCommands[MD_Swell][2]  = {.division = MD_Swell, .type = PCT_COMBINATION, .param = 5};
    mPistonCommands[MD_Swell][3]  = {.division = MD_Swell, .type = PCT_COMBINATION, .param = 4};
    mPistonCommands[MD_Swell][4]  = {.division = MD_Swell, .type = PCT_COMBINATION, .param = 3};
    mPistonCommands[MD_Swell][5]  = {.division = MD_Swell, .type = PCT_COMBINATION, .param = 2};
    mPistonCommands[MD_Swell][6]  = {.division = MD_Swell, .type = PCT_COMBINATION, .param = 1};
    mPistonCommands[MD_Swell][7]  = {.division = MD_Swell, .type = PCT_OFF,       .param = 0};
    mPistonCommands[MD_Swell][8]  = {.division = MD_Swell, .type = PCT_TRANSPOSE, .param = BT_TOGGLE};
    mPistonCommands[MD_Swell][9]  = {.division = MD_Swell, .type = PCT_COUPLER, .param = MIDIDivision::MD_Pedal};
    mPistonCommands[MD_Swell][10] = {.division = MD_Swell, .type = PCT_COUPLER, .param = MIDIDivision::MD_Great};
    mPistonCommands[MD_Swell][11] = {.division = MD_Swell, .type = PCT_COUPLER, .param = MIDIDivision::MD_Solo};

    mPistonCommands[MD_Swell][12] = {.division = MD_Control, .type = PCT_PAGE, .param = BT_NEXT};
    mPistonCommands[MD_Swell][13] = {.division = MD_Control, .type = PCT_PAGE, .param = BT_PREV};
    mPistonCommands[MD_Swell][14] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 12};
    mPistonCommands[MD_Swell][15] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 11};
    mPistonCommands[MD_Swell][16] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 10};
    mPistonCommands[MD_Swell][17] = {.division = MD_Control, .type = PCT_COMBINATION, .param =  9};
    mPistonCommands[MD_Swell][18] = {.division = MD_Control, .type = PCT_CLEAR, .param = 0};
    mPistonCommands[MD_Swell][19] = {.division = MD_Control, .type = PCT_HOLD,  .param = 0};
    mPistonCommands[MD_Swell][20] = {.division = MD_Control, .type = PCT_SET,   .param = 0};

    mPistonCommands[MD_Solo][0]  = {.division = MD_Solo, .type = PCT_CRESCENDO, .param = 0};
    mPistonCommands[MD_Solo][1]  = {.division = MD_Solo, .type = PCT_CLEAR,     .param = 0};
    mPistonCommands[MD_Solo][2]  = {.division = MD_Solo, .type = PCT_COMBINATION, .param = 5};
    mPistonCommands[MD_Solo][3]  = {.division = MD_Solo, .type = PCT_COMBINATION, .param = 4};
    mPistonCommands[MD_Solo][4]  = {.division = MD_Solo, .type = PCT_COMBINATION, .param = 3};
    mPistonCommands[MD_Solo][5]  = {.division = MD_Solo, .type = PCT_COMBINATION, .param = 2};
    mPistonCommands[MD_Solo][6]  = {.division = MD_Solo, .type = PCT_COMBINATION, .param = 1};
    mPistonCommands[MD_Solo][7]  = {.division = MD_Solo, .type = PCT_OFF,       .param = 0};
    mPistonCommands[MD_Solo][8]  = {.division = MD_Solo, .type = PCT_TRANSPOSE, .param = BT_TOGGLE};
    mPistonCommands[MD_Solo][9]  = {.division = MD_Solo, .type = PCT_COUPLER, .param = MIDIDivision::MD_Choir};
    mPistonCommands[MD_Solo][10] = {.division = MD_Solo, .type = PCT_COUPLER, .param = MIDIDivision::MD_Great};
    mPistonCommands[MD_Solo][11] = {.division = MD_Solo, .type = PCT_COUPLER, .param = MIDIDivision::MD_Swell};

    mPistonCommands[MD_Solo][12] = {.division = MD_Control, .type = PCT_SEQUENCE, .param = BT_NEXT};
    mPistonCommands[MD_Solo][13] = {.division = MD_Control, .type = PCT_SEQUENCE, .param = BT_PREV};
    mPistonCommands[MD_Solo][14] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 8};
    mPistonCommands[MD_Solo][15] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 7};
    mPistonCommands[MD_Solo][16] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 6};
    mPistonCommands[MD_Solo][17] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 5};
    mPistonCommands[MD_Solo][18] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 4};
    mPistonCommands[MD_Solo][19] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 3};
    mPistonCommands[MD_Solo][20] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 2};
    mPistonCommands[MD_Solo][21] = {.division = MD_Control, .type = PCT_COMBINATION, .param = 1};

    // initialize coupler and note status
    for (int i = 0; i < MAX_DIVISION_CHANNEL + 1; i++) {
        mCoupler[i].enabled = true;
        mCoupler[i].crescendo = false;
        for (int j = 0; j < MAX_DIVISION_CHANNEL + 1; j++) {
            mCoupler[i].couple[j] = CS_OFF;
        }

        for (int j = 0; j < COUPLER_NUM_NOTES; j++) {
            mPressedNotes[i][j].sourceMask = 0;
            mPressedNotes[i][j].pressed = false;
            mPressedNotes[i][j].velocity = 0;
        }
    }
}

MIDIDivision CouplerProcessor::getDivision(MIDIPort inPort, const MidiMessage &msg)
{
    // We ignore the input port and rely only on the input channel,
    // since most input ports share multiple divisions.

    // .. except for Great input channel (MIDI2), as this is the external keyboard.
    // Accept all channels from there as Great input
    if (inPort == MIDIPort::MP_MIDI2) {
        return MIDIDivision::MD_Great;
    }

    return mChannelDivisions[msg.channel];
}

void CouplerProcessor::setDivisionChannel(MIDIDivision division, uint8_t channel)
{
    // disable playing notes first
    allDivisionNotesOff(division);

    // disable the old channel, make it a unmapped MIDI input
    for (int i = 0; i < NUM_MIDI_CHANNELS; i++) {
        if (mChannelDivisions[i] == division) {
            mChannelDivisions[i] = MIDIDivision::MD_MIDI;
        }
    }
    
    mDivisionChannels[division] = channel;
    mChannelDivisions[channel] = division;
}

void CouplerProcessor::setCouplerMode(CouplerMode mode)
{
    if (mCouplerMode == CouplerMode::CM_ENABLED && mode != mCouplerMode) {
        allCouplerNotesOff();
    }

    mCouplerMode = mode;
}

NoteStatus &CouplerProcessor::getNoteStatus(MIDIDivision division, int note)
{
    return mPressedNotes[division][note - COUPLER_LOWEST_NOTE];
}

void CouplerProcessor::sendCouplerNoteOn(MIDIDivision division, int note, uint8_t velocity)
{
    MidiMessage msg;
    msg.channel = mDivisionChannels[division];
    msg.type = midi::MidiType::NoteOn;
    msg.data1 = note;
    msg.data2 = velocity;
    msg.length = 3;
    msg.valid = true;

    mMIDIRouter.injectMessage(mInjectPorts[division], msg);
}

void CouplerProcessor::sendCouplerNoteOff(MIDIDivision division, int note)
{
    MidiMessage msg;
    msg.channel = mDivisionChannels[division];
    msg.type = midi::MidiType::NoteOff;
    msg.data1 = note;
    msg.data2 = 0;
    msg.length = 3;
    msg.valid = false;

    mMIDIRouter.injectMessage(mInjectPorts[division], msg);
}

void CouplerProcessor::setCoupledNote(MIDIDivision source, MIDIDivision target, int sourceNote, int note, bool sendMidi)
{
    if (note < COUPLER_LOWEST_NOTE || note >= COUPLER_LOWEST_NOTE + COUPLER_NUM_NOTES) {
        return;
    }

    NoteStatus &status = getNoteStatus(target, note);

    if (status.sourceMask == 0 && !status.pressed && sendMidi) {
        sendCouplerNoteOn(target, note, getNoteStatus(source, sourceNote).velocity);
    }
    status.sourceMask |= (1<<source);
}

void CouplerProcessor::clearCoupledNote(MIDIDivision source, MIDIDivision target, int note, bool sendMidi)
{
    if (note < COUPLER_LOWEST_NOTE || note >= COUPLER_LOWEST_NOTE + COUPLER_NUM_NOTES) {
        return;
    }

    NoteStatus &status = getNoteStatus(target, note);

    status.sourceMask &= ~(1<<source);

    if (status.sourceMask == 0 && !status.pressed && sendMidi) {
        sendCouplerNoteOff(target, note);
    }
}

void CouplerProcessor::updateCouplerMode(MIDIDivision source, MIDIDivision target, CouplerState mode)
{
    CouplerState oldMode = mCoupler[source].couple[target];

    if (oldMode == mode) {
        return;
    }

    for (uint8_t note = COUPLER_LOWEST_NOTE; note < COUPLER_LOWEST_NOTE + COUPLER_NUM_NOTES; note++) {
        NoteStatus &status = getNoteStatus(source, note);

        // disable old mode 
        if (status.pressed && oldMode != CouplerState::CS_OFF) {
            clearCoupledNote(source, target, getTransposedNote(oldMode, note), true);
        }

        //enable new mode
        if (status.pressed && mode != CouplerState::CS_OFF) {
            setCoupledNote(source, target, note, getTransposedNote(mode, note), true);
        }
    }

    mCoupler[source].couple[target] = mode;
}

void CouplerProcessor::updateCoupledNote(MIDIDivision source, MIDIDivision target, 
                                         const MidiMessage &msg, const MidiMessage &cmsg)
{
    if (cmsg.type == midi::MidiType::NoteOn) {
        setCoupledNote(source, target, msg.data1, cmsg.data1, false);
    }
    if (cmsg.type == midi::MidiType::NoteOff) {
        clearCoupledNote(source, target, cmsg.data1, false);
    }
}

void CouplerProcessor::updatePlayedNote(MIDIDivision source, const MidiMessage &msg)
{
    int note = msg.data2;

    if (note < COUPLER_LOWEST_NOTE || note >= COUPLER_LOWEST_NOTE + COUPLER_NUM_NOTES) {
        return;
    }

    NoteStatus &status = getNoteStatus(source, note);

    if (msg.type == midi::MidiType::NoteOn) {
        status.pressed = mCoupler[source].enabled;
        status.velocity = msg.data2;
    }
    if (msg.type == midi::MidiType::NoteOff) {
        status.pressed = false;
    }
}

uint8_t CouplerProcessor::getTransposedNote(CouplerState mode, uint8_t note)
{
    if (mode == CouplerState::CS_OCTAVE_DOWN) {
        return note - 12;
    }
    else if (mode == CouplerState::CS_OCTAVE_UP) {
        return note + 12;
    }
    else {
        return note;
    }
}

void CouplerProcessor::begin()
{
}

void CouplerProcessor::reset()
{
    allCouplerNotesOff();

    // initialize coupler status
    for (int i = 0; i < MAX_DIVISION_CHANNEL + 1; i++) {
        mCoupler[i].enabled = true;
        mCoupler[i].crescendo = false;
        for (int j = 0; j < MAX_DIVISION_CHANNEL + 1; j++) {
            mCoupler[i].couple[j] = CS_OFF;
        }
    }
}

void CouplerProcessor::allCouplerNotesOff()
{
    for (int i = 0; i < COUPLER_NUM_SOUND_DIVISIONS; i++) {
        allCouplerNotesOff(COUPLER_DIVISIONS[i]);
    }
}

void CouplerProcessor::allCouplerNotesOff(MIDIDivision division)
{
    for (int i = 0; i < COUPLER_NUM_SOUND_DIVISIONS; i++) {
        MIDIDivision target = COUPLER_DIVISIONS[i];
        for (int note = COUPLER_LOWEST_NOTE; note < COUPLER_LOWEST_NOTE+COUPLER_NUM_NOTES; note++) {
            clearCoupledNote(division, target, note, true);
        }
    }
}

void CouplerProcessor::allDivisionsNotesOff(bool soundOff)
{
    for (int i = 0; i < COUPLER_NUM_SOUND_DIVISIONS; i++) {
        allDivisionNotesOff(COUPLER_DIVISIONS[i], soundOff);
    }
}

void CouplerProcessor::allDivisionNotesOff(MIDIDivision division, bool soundOff)
{
    for (int note = COUPLER_LOWEST_NOTE; note < COUPLER_LOWEST_NOTE+COUPLER_NUM_NOTES; note++) {
        NoteStatus &status = getNoteStatus(division, note);

        status.sourceMask = 0;
        status.pressed = false;
    }

    // send AllNotesOff message
    MidiMessage msg;
    msg.channel = mDivisionChannels[division];
    msg.type = midi::MidiType::ControlChange;
    msg.data1 = soundOff ? midi::MidiControlChangeNumber::AllSoundOff 
                         : midi::MidiControlChangeNumber::AllNotesOff;
    msg.data2 = 0;
    msg.length = 3;
    msg.valid = false;

    mMIDIRouter.injectMessage(mInjectPorts[division], msg);

}

void CouplerProcessor::sendPageTurn(ButtonType direction) {
    if (direction == BT_PREV) {
        Keyboard.press(KEY_LEFT);
        Keyboard.release(KEY_LEFT);
    } else {
        Keyboard.press(KEY_RIGHT);
        Keyboard.release(KEY_RIGHT);
    }
}

void CouplerProcessor::clearCouplers(MIDIDivision division)
{
    allCouplerNotesOff(division);

    for (int i = 0; i < MAX_DIVISION_CHANNEL + 1; i++) {
        mCoupler[division].couple[i] = CS_OFF;
    }
}

void CouplerProcessor::coupleDivision(MIDIDivision division, MIDIDivision target, CouplerState mode)
{
    updateCouplerMode(division, target, mode);

    // TODO update LED output, update Panel
}

void CouplerProcessor::transposeDivision(MIDIDivision division, CouplerState mode)
{
    updateCouplerMode(division, division, mode);

    // TODO update LED output, update Panel
}

void CouplerProcessor::enableCrescendo(MIDIDivision division, bool crescendo)
{
    mCoupler[division].crescendo = crescendo;
}

void CouplerProcessor::enableDivision(MIDIDivision division, bool output)
{
    mCoupler[division].enabled = output;

    // send note off for curently playing notes
    if (!output) {
        for (int note = COUPLER_LOWEST_NOTE; note < COUPLER_LOWEST_NOTE+COUPLER_NUM_NOTES; note++) {

            NoteStatus &status = getNoteStatus(division, note);

            if (status.pressed && status.sourceMask == 0) {
                sendCouplerNoteOff(division, note);
            }
            status.pressed = false;
        }
    }
}

CouplerState CouplerProcessor::coupled(MIDIDivision division, MIDIDivision target) const
{
    return mCoupler[division].couple[target];
}

CouplerState CouplerProcessor::transposed(MIDIDivision division) const
{
    return mCoupler[division].couple[division];
}

bool CouplerProcessor::crescendo(MIDIDivision division) const
{
    return mCoupler[division].crescendo;
}

bool CouplerProcessor::enabled(MIDIDivision division) const
{
    return mCoupler[division].enabled;
}

void CouplerProcessor::selectCombination(MIDIDivision division, int combination)
{

}

void CouplerProcessor::clearCombination(MIDIDivision division)
{

}


void CouplerProcessor::processPistonPress(MIDIDivision division, uint8_t button, bool longPress)
{
    if (button >= MAX_PISTONS) {
        return;
    }

    PistonCommand &cmd = mPistonCommands[division][button];

    switch (cmd.type) {
        case PCT_COUPLER:
            if (longPress) {
                // couple transposed
                coupleDivision(cmd.division, cmd.param.division, CS_OCTAVE_UP);
            } else {
                // Toggle coupler status
                if (coupled(cmd.division, cmd.param.division) == CS_OFF) {
                    coupleDivision(cmd.division, cmd.param.division, CS_COUPLE);
                } else {
                    coupleDivision(cmd.division, cmd.param.division, CS_OFF);
                }
            }
            break;
        case PCT_TRANSPOSE:
            if (cmd.param.button == BT_TOGGLE) {
                if (longPress) {
                    // Disable transpose
                    transposeDivision(cmd.division, CS_OFF);
                } else {
                    if (transposed(cmd.division) == CS_OFF) {
                        transposeDivision(cmd.division, CS_OCTAVE_UP);
                    } else if (transposed(cmd.division) == CS_OCTAVE_UP) {
                        transposeDivision(cmd.division, CS_OCTAVE_DOWN);
                    } else if (transposed(cmd.division) == CS_OCTAVE_DOWN) {
                        transposeDivision(cmd.division, CS_OFF);
                    }                    
                }
            }
            break;
        case PCT_OFF:
            if (longPress) {
                clearCouplers(cmd.division);
            } else {
                enableDivision(cmd.division, !enabled(cmd.division));
            }
            break;
        case PCT_COMBINATION:
            selectCombination(cmd.division, cmd.param.value);
            break;
        case PCT_CLEAR:
            if (longPress) {
                allDivisionNotesOff(cmd.division);
            }
            clearCombination(cmd.division);
            break;
        case PCT_CRESCENDO:
            enableCrescendo(cmd.division, !crescendo(cmd.division));
            break;
        case PCT_PAGE:
            sendPageTurn(cmd.param.button);
            break;
        case PCT_SEQUENCE:
            break;
        case PCT_SET:
            break;
        case PCT_HOLD:
            break;
        case PCT_SOUNDOFF:
            if (longPress || cmd.param.button == ButtonType::BT_ALL) {
                allDivisionsNotesOff(true);
            } else {
                allDivisionNotesOff(cmd.division, true);
            }
            break;

        case PCT_NONE:
            break;
    }
}

void CouplerProcessor::processCrescendoChange(uint16_t crescendo)
{
    if (crescendo != mPedalCrescendo) {

        mPedalCrescendo = crescendo;
    }
}

void CouplerProcessor::processPedalChange(MIDIDivision division, uint16_t value)
{
    switch (division) {
        case MIDIDivision::MD_Choir:
            if (value != mPedalChoir) {

                mPedalChoir = value;
            }
            break;
        case MIDIDivision::MD_Swell:
            if (value != mPedalSwell) {

                mPedalChoir = value;
            }
            break;
        default:
            break;
    }
}

void CouplerProcessor::sendCouplerMessage(MIDIDivision division, MIDIDivision target, const MidiMessage &msg)
{
    if (mCoupler[division].couple[target] == CS_OFF) {
        return;
    }

    // Create new MIDI message with target channel
    MidiMessage cmsg = msg;
    cmsg.channel = mDivisionChannels[target];

    // Transpose note messages
    if ((cmsg.type == midi::MidiType::NoteOn || cmsg.type == midi::MidiType::NoteOff)) {
        cmsg.data1 = getTransposedNote(mCoupler[division].couple[target], cmsg.data1);
    }

    updateCoupledNote(division, target, msg, cmsg);

    // Inject coupler midi message into the router (using the same MIDI port)    
    mMIDIRouter.injectMessage(mInjectPorts[target], cmsg);
}

void CouplerProcessor::routeDivisionInput(MIDIPort inPort, const MidiMessage &msg)
{
    MIDIDivision division = getDivision(inPort, msg);

    // Forward any non-division inputs directly to output routing
    if (mCouplerMode != CouplerMode::CM_ENABLED || division == MIDIDivision::MD_MIDI) {
        mMIDIRouter.injectMessage(inPort, msg);
        return;
    }

    updatePlayedNote(division, msg);

    // handle coupler, transpose, inject new messages
    for (int i = 0; i < COUPLER_NUM_SOUND_DIVISIONS; i++) {
        MIDIDivision target = COUPLER_DIVISIONS[i];
        sendCouplerMessage(division, target, msg);    
    }

    // only inject original message if division is not OFF (only for Note messages)
    if (mCoupler[division].enabled) {
        mMIDIRouter.injectMessage(inPort, msg);
    }
}

