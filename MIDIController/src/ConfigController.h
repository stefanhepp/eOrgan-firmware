/**
 * @project     MidiController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * I2C configuration controller
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#include <inttypes.h>

class ConfigController
{
    private:

    public:
        explicit ConfigController();

        void resetAll();

        void readAll();


        void setPedalChannel(uint8_t channel);

        void setPedalLEDIntensity(uint8_t intensity);

        void setToestudChannels(uint8_t channel, uint8_t swellChannel, uint8_t choirChannel);

        void setToestudMode(uint8_t mode);

        void setTechnicsChannel(uint8_t channel);

        void setKeyboardChannels(uint8_t channel1, uint8_t channel2);

        void startCalibrateAnalogInputs();

        void stopCalibrateAnalogInputs();

        void trainKeyboard(uint8_t keyboard);


        void begin();

        void loop();
};
