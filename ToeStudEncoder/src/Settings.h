/*
 * @project     ToeStud Encoder
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Store config values in EEPROM.
 * Channel and intensity are values from 0 to 15.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __SETUP_H__
#define __SETUP_H__

#include <inttypes.h>

#include <CalibratedAnalogInput.h>

class Settings
{
    public:
        Settings();

        uint8_t getMIDIChannel();

        void setMIDIChannel(uint8_t channel);

        bool hasCalibrationData();

        void getCalibrationData(uint8_t pedal, AICalibrationData &data) const;

        void setCalibrationData(uint8_t pedal, const AICalibrationData &data);
};

#endif
