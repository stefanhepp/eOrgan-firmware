/*
 * @project     TechnicsController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Store config values in EEPROM.
 * Channel and intensity are values from 0 to 15.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <inttypes.h>

class Settings
{
    public:
        Settings();

        uint8_t getMIDIChannel();

        void setMIDIChannel(uint8_t channel);

        uint8_t getWheelMin();

        uint8_t getWheelMax();

        void setWheelMinMax(uint8_t min, uint8_t max);
};

#endif
