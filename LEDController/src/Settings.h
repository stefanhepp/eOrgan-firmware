/*
 * @project     Midi Pedalboard
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

        uint8_t getLEDIntensity(int index);

        void setLEDIntensity(int index, uint8_t intensity);
};

#endif
