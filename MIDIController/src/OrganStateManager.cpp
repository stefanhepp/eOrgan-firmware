/*
 * @project     MIDIController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Organ routing state manager implementation.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "OrganStateManager.h"

#include <Arduino.h>

#include <inttypes.h>

#include <common_config.h>

OrganStateManager::OrganStateManager(MIDIRouter &router, CouplerProcessor &coupler, 
                                     ControllerDriver &driver)
: mMIDIRouter(router), mCoupler(coupler), mControl(driver)
{
    mDivisionChannels[MIDIDivision::MD_Pedal] = MIDIDivision::MD_Pedal;
    mDivisionChannels[MIDIDivision::MD_Choir] = MIDIDivision::MD_Choir;
    mDivisionChannels[MIDIDivision::MD_Swell] = MIDIDivision::MD_Swell;
    mDivisionChannels[MIDIDivision::MD_Solo ] = MIDIDivision::MD_Solo;
    mDivisionChannels[MIDIDivision::MD_Control] = MIDIDivision::MD_Control;
}

void OrganStateManager::setDivisionChannel(MIDIDivision division, uint8_t channel)
{
    mDivisionChannels[division] = channel;

    mCoupler.setDivisionChannel(division, channel);

    switch (division) {
        case MIDIDivision::MD_Pedal:
            mControl.setPedalChannel(channel);
            break;
        case MIDIDivision::MD_Choir:
            mControl.setTechnicsChannel(channel);
            break;
        case MIDIDivision::MD_Swell:
            mControl.setKeyboardChannels(mDivisionChannels[MIDIDivision::MD_Swell], 
                                         mDivisionChannels[MIDIDivision::MD_Solo]
                                         );
            break;
        case MIDIDivision::MD_Solo:
            mControl.setKeyboardChannels(mDivisionChannels[MIDIDivision::MD_Swell], 
                                         mDivisionChannels[MIDIDivision::MD_Solo]
                                         );
            break;
        case MIDIDivision::MD_Control:
        case MIDIDivision::MD_Great:
        case MIDIDivision::MD_MIDI:
            // Nothing to do, external devices
            break;
    }
}

void OrganStateManager::setCouplerMode(CouplerMode mode)
{
    switch (mode) {
        case CM_DISABLED:
            mMIDIRouter.enableCoupler(false);
            break;
        case CM_MIDI:
            mMIDIRouter.enableCoupler(false);
            mCoupler.sendMIDICommands(true);
            break;
        case CM_ENABLED:
            mMIDIRouter.enableCoupler(true);
            mCoupler.sendMIDICommands(false);
            break;
    }
}

void OrganStateManager::begin()
{
}

void OrganStateManager::loop()
{
}