/**
 * @project     MidiController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Audio effects and router
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#include <inttypes.h>
#include <common_config.h>

class AudioProcessor {
    public:
        AudioProcessor();

        void begin();

        void setUSBInputGain(float gain);

        void setFXInputGain(float gain);

        void setVolume(float volume);
};
