/**
 * @project     MidiController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Central MIDI routing state manager.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#include <inttypes.h>
#include <common_config.h>

#include <MIDI.h>

#include "ControllerDriver.h"
#include "MIDIRouter.h"

class OrganStateManager
{
    private:
        MIDIRouter &mMIDIRouter;
        ControllerDriver &mControl;

        midi::Channel mDivisionChannels[MAX_DIVISION_CHANNEL + 1];

    public:
        explicit OrganStateManager(MIDIRouter &router, ControllerDriver &driver);

        /**
         * Print the next status messages received via I2C
         */
        void printNextStatus();

        void setDivisionChannel(MIDIDivision division, uint8_t channel);

};