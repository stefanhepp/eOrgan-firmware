/**
 * @project     MidiController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Panel UART control interface handler.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#include <inttypes.h>

#include <common_config.h>

#include "MIDIRouter.h"
#include "AudioProcessor.h"
#include "CouplerProcessor.h"

class PanelInterface
{
    private:

        uint8_t readCommandOpcode();

        void receiveCommand();

    public:
        explicit PanelInterface();

        void begin();

        void loop();
};
