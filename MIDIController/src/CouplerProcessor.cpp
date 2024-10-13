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

    // initialize piston command mapping
    for (int i = 0; i < MAX_DIVISION_CHANNEL + 1; i++) {
        for (int j = 0; j < MAX_PISTONS; j++) {
            mPistonCommands[i][j].type = PCT_NONE;
            mPistonCommands[i][j].division = MIDIDivision::MD_MIDI;
            mPistonCommands[i][j].param = 0;
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

void CouplerProcessor::processPistonPress(MIDIDivision division, uint8_t button, bool longPress)
{
    if (button >= MAX_PISTONS) {
        return;
    }

    PistonCommand &cmd = mPistonCommands[division][button];

    switch (cmd.type) {
        case PCT_COUPLER:
            break;
        case PCT_TRANSPOSE:
            break;
        case PCT_OFF:
            break;
        case PCT_COMBINATION:
            break;
        case PCT_CLEAR:
            break;
        case PCT_CRESCENDO:
            break;
        case PCT_PAGE:
            if (cmd.param == PP_PREV) {
                //Keyboard.press(KEY_LEFT); 
                //Keyboard.release(KEY_LEFT); 
            } else {
                //Keyboard.press(KEY_RIGHT);
                //Keyboard.release(KEY_RIGHT);
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

    // TODO handle coupler, transpose, inject new messages

    // TODO only inject original message if division is not OFF (only for Note messages)
    mMIDIRouter.injectMessage(inPort, msg);
}

