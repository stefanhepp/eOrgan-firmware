/*
 * @project     PanelUI
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Panel I2C driver.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "PanelDriver.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include <iostream>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <common_config.h>

#include "config.h"

PanelDriver::PanelDriver()
{
}

void PanelDriver::startCalibrateAnalogInputs()
{
    uint8_t data[1];
    data[0] = I2C_CMD_CALIBRATE;
    
    wiringPiI2CRawWrite(mFdI2C, data, 1);
}

void PanelDriver::stopCalibrateAnalogInputs()
{
    uint8_t data[1];
    data[0] = I2C_CMD_STOP_CALIBRATE;
    
    wiringPiI2CRawWrite(mFdI2C, data, 1);
}

void PanelDriver::setSensitivity(int sensitivity)
{
    uint8_t data[2];
    data[0] = I2C_CMD_SET_SENSITIVITY;
    data[1] = (uint8_t)sensitivity;
    
    wiringPiI2CRawWrite(mFdI2C, data, 2);
}

void PanelDriver::setMainPanelLEDs(uint8_t leds)
{
    uint8_t data[2];
    data[0] = I2C_CMD_SET_LEDS;
    data[1] = leds;
    
    wiringPiI2CRawWrite(mFdI2C, data, 2);

    mLastMainPanelLEDs = leds;
}

void PanelDriver::readButtons(Panel panel, uint8_t *data, uint8_t size)
{
    int length = data[0];

    for (int i = 1; i < length && i < size; i++) {
        uint8_t button = data[i] >> 1;
        bool longPress = data[i] & 0x01 ? true : false;
        if (mButtonPressCallback) {
            mButtonPressCallback(panel, button, longPress);
        }
    }
}

void PanelDriver::readMainPanelStatus()
{
    uint8_t data[16];
    wiringPiI2CRawRead(mFdI2C, data, 16);

    int volumeMaster = data[0] << 8 | data[1];
    int volumeFX1    = data[2] << 8 | data[3];
    int volumeFX2    = data[4] << 8 | data[5];

    int encoderDelta1 = (int8_t)data[6];
    int encoderDelta2 = (int8_t)data[7];

    if (encoderDelta1 != 0 && mEncoderChangeCallback) {
        mEncoderChangeCallback(0, encoderDelta1);
    }
    if (encoderDelta2 != 0 && mEncoderChangeCallback) {
        mEncoderChangeCallback(1, encoderDelta2);
    }

    readButtons(Panel::PANEL_MAIN, &(data[8]), 8);
}

int PanelDriver::begin()
{
    pinMode(PIN_INT_RIGHT, INPUT);

    if ((mFdI2C = wiringPiI2CSetupInterface(DEV_I2C, Controller::MC_MainPanel)) < 0) {
        std::cerr << "Unable to open I2C-0 device: " << strerror(errno) << std::endl;
        return 1;
    }

    return 0;
}

void PanelDriver::loop()
{
    int interrupt = digitalRead(PIN_INT_RIGHT);

    if (interrupt == LOW) {
        readMainPanelStatus();
    }
}
