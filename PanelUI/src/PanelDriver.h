/**
 * @project     PanelUI
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * I2C panel controller
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#include <stdint.h>

#include <common_config.h>

#include "commonTypes.h"

using ButtonPressCallback = void(*)(Panel panel, int button, bool longPress);
using VolumeChangeCallback = void(*)(VolumeChannel channel, int volume);
using EncoderChangeCallback = void(*)(int encoder, int delta);

class PanelDriver
{
    private:
        ButtonPressCallback    mButtonPressCallback = nullptr;
        VolumeChangeCallback   mVolumeChangeCallback = nullptr;
        EncoderChangeCallback  mEncoderChangeCallback = nullptr;

        int mFdI2C = -1;

        uint8_t mLastMainPanelLEDs = 0x00;

	int mLastVolumeMaster = -1;
	int mLastVolumeFX1 = -1;
	int mLastVolumeFX2 = -1;

        int i2cOpen(const char* device);

        int i2cSelect(uint8_t slaveAddress);

        int i2cWriteData(const uint8_t *data, uint8_t size); 

        int i2cReadData(uint8_t *data, uint8_t size);

        void readButtons(Panel panel, uint8_t *data, uint8_t size);

    public:
        explicit PanelDriver();

        void setButtonPressCallback(ButtonPressCallback callback) { mButtonPressCallback = callback; }

        void setVolumeChangeCallback(VolumeChangeCallback callback) { mVolumeChangeCallback = callback; }

        void setEncoderChangeCallback(EncoderChangeCallback callback) { mEncoderChangeCallback = callback; }


        void startCalibrateAnalogInputs();

        void stopCalibrateAnalogInputs();

        void setSensitivity(int sensitivity);

        void setMainPanelLEDs(uint8_t leds);


        void readMainPanelStatus();


        int begin();

        void loop();
};
