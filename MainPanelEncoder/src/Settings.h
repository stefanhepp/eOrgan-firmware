/*
 * @project     MainPanel Encoder
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Store config values in EEPROM.
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

        void getCalibrationData(uint8_t knob, AICalibrationData &data) const;

        void setCalibrationData(uint8_t knob, const AICalibrationData &data);

        uint8_t getSensitivity();

        void setSensitivity(uint8_t sensitivity);
};
