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

OrganStateManager::OrganStateManager(MIDIRouter &router, ControllerDriver &driver)
: mMIDIRouter(router), mControl(driver)
{
    mDivisionChannels[MIDIDivision::MD_Pedal] = MIDIDivision::MD_Pedal;
    mDivisionChannels[MIDIDivision::MD_Choir] = MIDIDivision::MD_Choir;
    mDivisionChannels[MIDIDivision::MD_Swell] = MIDIDivision::MD_Swell;
    mDivisionChannels[MIDIDivision::MD_Solo ] = MIDIDivision::MD_Solo;
    mDivisionChannels[MIDIDivision::MD_Control] = MIDIDivision::MD_Control;
}

void OrganStateManager::printNextStatus()
{
    
}

void OrganStateManager::setDivisionChannel(MIDIDivision division, uint8_t channel)
{
    mDivisionChannels[division] = channel;

    switch (division) {
        case MIDIDivision::MD_Pedal:
            mControl.setPedalChannel(channel);
            break;
        case MIDIDivision::MD_Choir:
            mControl.setTechnicsChannel(channel);
            mControl.setToestudChannels(mDivisionChannels[MIDIDivision::MD_Control], 
                                        mDivisionChannels[MIDIDivision::MD_Swell], 
                                        mDivisionChannels[MIDIDivision::MD_Choir]
                                        );
            break;
        case MIDIDivision::MD_Swell:
            mControl.setKeyboardChannels(mDivisionChannels[MIDIDivision::MD_Swell], 
                                         mDivisionChannels[MIDIDivision::MD_Solo]
                                         );
            mControl.setToestudChannels(mDivisionChannels[MIDIDivision::MD_Control], 
                                        mDivisionChannels[MIDIDivision::MD_Swell], 
                                        mDivisionChannels[MIDIDivision::MD_Choir]
                                        );
            break;
        case MIDIDivision::MD_Solo:
            mControl.setKeyboardChannels(mDivisionChannels[MIDIDivision::MD_Swell], 
                                         mDivisionChannels[MIDIDivision::MD_Solo]
                                         );
            break;
        case MIDIDivision::MD_Control:
            mControl.setToestudChannels(mDivisionChannels[MIDIDivision::MD_Control], 
                                        mDivisionChannels[MIDIDivision::MD_Swell], 
                                        mDivisionChannels[MIDIDivision::MD_Choir]
                                        );
            break;
    }
}

void OrganStateManager::begin()
{

}

void OrganStateManager::loop()
{

}
