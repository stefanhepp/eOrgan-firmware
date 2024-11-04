/**
 * @project     MidiController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * High-level state manager, drives other controllers for some higher-level functions.
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
#include "CouplerProcessor.h"

enum CouplerMode {
    CM_DISABLED = 0x00,
    CM_MIDI = 0x01,
    CM_ENABLED = 0x02
};

class OrganStateManager
{
    private:
        MIDIRouter &mMIDIRouter;
        CouplerProcessor &mCoupler;
        ControllerDriver &mControl;

        midi::Channel mDivisionChannels[MAX_DIVISION_CHANNEL + 1];

    public:
        explicit OrganStateManager(MIDIRouter &router, CouplerProcessor &coupler, ControllerDriver &driver);

        void setDivisionChannel(MIDIDivision division, uint8_t channel);

        void setCouplerMode(CouplerMode mode);

        void begin();

        void loop();

};
