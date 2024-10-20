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

CouplerProcessor::CouplerProcessor(MIDIRouter &router)
: mMIDIRouter(router)
{
    // Map division -> channel
    mDivisionChannels[MIDIDivision::MD_Pedal] = MIDIDivision::MD_Pedal;
    mDivisionChannels[MIDIDivision::MD_Choir] = MIDIDivision::MD_Choir;
    mDivisionChannels[MIDIDivision::MD_Swell] = MIDIDivision::MD_Swell;
    mDivisionChannels[MIDIDivision::MD_Solo ] = MIDIDivision::MD_Solo;
    mDivisionChannels[MIDIDivision::MD_Control] = MIDIDivision::MD_Control;

    // Map channel -> division
    for (int i = 0; i < NUM_MIDI_CHANNELS; i++) {
        mChannelDivisions[i] = MIDIDivision::MD_MIDI;
    }
    mChannelDivisions[MIDIDivision::MD_Pedal] = MIDIDivision::MD_Pedal;
    mChannelDivisions[MIDIDivision::MD_Choir] = MIDIDivision::MD_Choir;
    mChannelDivisions[MIDIDivision::MD_Swell] = MIDIDivision::MD_Swell;
    mChannelDivisions[MIDIDivision::MD_Solo ] = MIDIDivision::MD_Solo;
    mChannelDivisions[MIDIDivision::MD_Control] = MIDIDivision::MD_Control;

    // initialize coupler status
    for (int i = 0; i < MAX_DIVISION_CHANNEL + 1; i++) {
        mCoupler[i].enabled = true;
        mCoupler[i].crescendo = false;
        for (int j = 0; j < MAX_DIVISION_CHANNEL + 1; j++) {
            mCoupler[i].couple[j] = CM_OFF;
        }
    }

    // initialize piston command mapping
    for (int i = 0; i < MAX_DIVISION_CHANNEL + 1; i++) {
        for (int j = 0; j < MAX_PISTONS; j++) {
            mPistonCommands[i][j].type = PCT_NONE;
            mPistonCommands[i][j].division = MIDIDivision::MD_MIDI;
            mPistonCommands[i][j].param.value = 0;
        }
    }

    mPistonCommands[MD_Control][2]  = {.division = MD_Control, .type = PCT_NONE, .param = 0};        // Top row, right (right to left)
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
    // disable the old channel, make it a unmapped MIDI input
    for (int i = 0; i < NUM_MIDI_CHANNELS; i++) {
        if (mChannelDivisions[i] == division) {
            mChannelDivisions[i] = MIDIDivision::MD_MIDI;
        }
    }
    
    mDivisionChannels[division] = channel;
    mChannelDivisions[channel] = division;
}

void CouplerProcessor::begin()
{
}

void CouplerProcessor::clearCouplers(MIDIDivision division)
{
    for (int i = 0; i < MAX_DIVISION_CHANNEL + 1; i++) {
        mCoupler[division].couple[i] = CM_OFF;
    }
}

void CouplerProcessor::coupleDivision(MIDIDivision division, MIDIDivision target, CoupleMode mode)
{
    mCoupler[division].couple[target] = mode;

    // TODO update LED output, update Panel
}

void CouplerProcessor::transposeDivision(MIDIDivision division, CoupleMode mode)
{
    mCoupler[division].couple[division] = mode;

    // TODO update LED output, update Panel
}

void CouplerProcessor::enableCrescendo(MIDIDivision division, bool crescendo)
{
    mCoupler[division].crescendo = crescendo;
}

void CouplerProcessor::enableDivision(MIDIDivision division, bool output)
{
    mCoupler[division].enabled = output;
}

CoupleMode CouplerProcessor::coupled(MIDIDivision division, MIDIDivision target) const
{
    return mCoupler[division].couple[target];
}

CoupleMode CouplerProcessor::transposed(MIDIDivision division) const
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
                coupleDivision(cmd.division, cmd.param.division, CM_OCTAVE_UP);
            } else {
                // Toggle coupler status
                if (coupled(cmd.division, cmd.param.division) == CM_OFF) {
                    coupleDivision(cmd.division, cmd.param.division, CM_COUPLE);
                } else {
                    coupleDivision(cmd.division, cmd.param.division, CM_OFF);
                }
            }
            break;
        case PCT_TRANSPOSE:
            if (cmd.param.button == BT_TOGGLE) {
                if (longPress) {
                    // Disable transpose
                    transposeDivision(cmd.division, CM_OFF);
                } else {
                    if (transposed(cmd.division) == CM_OFF) {
                        transposeDivision(cmd.division, CM_OCTAVE_UP);
                    } else if (transposed(cmd.division) == CM_OCTAVE_UP) {
                        transposeDivision(cmd.division, CM_OCTAVE_DOWN);
                    } else if (transposed(cmd.division) == CM_OCTAVE_DOWN) {
                        transposeDivision(cmd.division, CM_OFF);
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
            clearCombination(cmd.division);
            break;
        case PCT_CRESCENDO:
            enableCrescendo(cmd.division, !crescendo(cmd.division));
            break;
        case PCT_PAGE:
            if (cmd.param.button == BT_PREV) {
                Keyboard.press(KEY_LEFT); 
                Keyboard.release(KEY_LEFT); 
            } else {
                Keyboard.press(KEY_RIGHT);
                Keyboard.release(KEY_RIGHT);
            }
            break;
        case PCT_SEQUENCE:
            break;
        case PCT_SET:
            break;
        case PCT_HOLD:
            break;

        case PCT_NONE:
            break;
    }
}

bool CouplerProcessor::processPedalMessage(MIDIDivision, const MidiMessage &msg)
{

    return false;
}

void CouplerProcessor::processPedalChange(uint16_t crescendo, uint16_t swell, uint16_t choir)
{
    if (crescendo != mPedalCrescendo) {

        mPedalCrescendo = crescendo;
    }
    if (swell != mPedalSwell) {

        mPedalSwell = swell;
    }
    if (choir != mPedalChoir) {

        mPedalChoir = choir;
    }
}

void CouplerProcessor::sendCouplerMessage(MIDIDivision division, MIDIDivision target, 
                                          MIDIPort inPort, const MidiMessage &msg)
{
    if (mCoupler[division].couple[target] == CM_OFF) {
        return;
    }

    // Create new MIDI message with target channel
    MidiMessage cmsg = msg;
    cmsg.channel = mDivisionChannels[target];

    // Transpose note messages
    if ((cmsg.type == midi::MidiType::NoteOn || cmsg.type == midi::MidiType::NoteOff)) {
        if (mCoupler[division].couple[target] == CM_OCTAVE_DOWN) {
            cmsg.data1 -= 12;
        }
        if (mCoupler[division].couple[target] == CM_OCTAVE_UP) {
            cmsg.data1 += 12;
        }
    }

    // Inject coupler midi message into the router (using the same MIDI port)
    mMIDIRouter.injectMessage(inPort, cmsg);
}

void CouplerProcessor::routeDivisionInput(MIDIPort inPort, const MidiMessage &msg)
{
    MIDIDivision division = getDivision(inPort, msg);

    // Forward any non-division inputs directly to output routing
    if (division == MIDIDivision::MD_MIDI) {
        mMIDIRouter.injectMessage(inPort, msg);
        return;
    }

    // catch MIDI messages from swell pedals, process them here
    if (processPedalMessage(division, msg)) {
        return;
    }

    // handle coupler, transpose, inject new messages
    sendCouplerMessage(division, MIDIDivision::MD_Pedal, inPort, msg);
    sendCouplerMessage(division, MIDIDivision::MD_Choir, inPort, msg);
    sendCouplerMessage(division, MIDIDivision::MD_Great, inPort, msg);
    sendCouplerMessage(division, MIDIDivision::MD_Swell, inPort, msg);
    sendCouplerMessage(division, MIDIDivision::MD_Solo,  inPort, msg);

    // only inject original message if division is not OFF (only for Note messages)
    if (mCoupler[division].enabled) {
        mMIDIRouter.injectMessage(inPort, msg);
    }
}

