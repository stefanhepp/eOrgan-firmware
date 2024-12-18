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
#pragma once

#include <inttypes.h>

#include <CalibratedAnalogInput.h>


class Settings
{
    public:
        Settings();

        bool hasCalibrationData();

        void getCalibrationData(uint8_t pedal, AICalibrationData &data) const;

        void setCalibrationData(uint8_t pedal, const AICalibrationData &data);

        uint8_t getSensitivity();

        void setSensitivity(uint8_t sensitivity);
};
