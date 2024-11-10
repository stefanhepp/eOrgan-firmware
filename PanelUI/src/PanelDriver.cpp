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
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <iostream>

#include <wiringPi.h>

#include <common_config.h>

#include "config.h"

#define I2C_SLAVE	0x0703
#define I2C_SMBUS	0x0720	/* SMBus-level access */

PanelDriver::PanelDriver()
{
}

int PanelDriver::i2cOpen(const char* device)
{
    if ((mFdI2C = open (device, O_RDWR)) < 0) {
        printf("Unable to open I2C device: %s\n", strerror(errno));
    }
    return mFdI2C;
}

int PanelDriver::i2cSelect(uint8_t slaveAddress)
{
    if (ioctl (mFdI2C, I2C_SLAVE, slaveAddress) < 0) {
        printf("Unable to select I2C device: %s\n", strerror(errno));
        return -1;
    }
    return mFdI2C;
}

int PanelDriver::i2cWriteData(const uint8_t *data, uint8_t size)
{
    int ret;
    if ((ret = write(mFdI2C, data, size)) < 0) {
        printf("Unable to write to I2C device: %s\n", strerror(errno));
    }
    // add some delay to give panel time to process
    delay(20);
    return ret;
}

int PanelDriver::i2cReadData(uint8_t *data, uint8_t size)
{
    int ret;
    if ((ret = read(mFdI2C, data, size)) < 0) {
        printf("Unable to read from I2C device: %s\n", strerror(errno));
    }
    // add some delay to give panel time to process
    delay(20);
    return ret;
}

void PanelDriver::startCalibrateAnalogInputs()
{
    uint8_t data[1];
    data[0] = I2C_CMD_CALIBRATE;
    
    i2cWriteData(data, 1);
}

void PanelDriver::stopCalibrateAnalogInputs()
{
    uint8_t data[1];
    data[0] = I2C_CMD_STOP_CALIBRATE;
    
    i2cWriteData(data, 1);
}

void PanelDriver::setSensitivity(int sensitivity)
{
    uint8_t data[2];
    data[0] = I2C_CMD_SET_SENSITIVITY;
    data[1] = (uint8_t)sensitivity;
    
    i2cWriteData(data, 2);
}

void PanelDriver::setMainPanelLEDs(uint8_t leds)
{
    uint8_t data[2];
    data[0] = I2C_CMD_SET_LEDS;
    data[1] = leds;
    
    i2cWriteData(data, 2);

    mLastMainPanelLEDs = leds;
}

void PanelDriver::readButtons(Panel panel, uint8_t *data, uint8_t size)
{
    int length = data[0];

    for (int i = 1; i < length+1 && i < size; i++) {
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

    int ret = i2cReadData(data, 16);
    if (ret < 8) {
        perror("readMainPanelStatus: Error reading I2C data");
        return;
    }
    if (data[0] == 0xFF && data[2] == 0xFF) {
        std::cerr << "No data received from I2C device." << std::endl;
        return;
    }

    int volumeMaster = data[0] << 8 | data[1];
    int volumeFX1    = data[2] << 8 | data[3];
    int volumeFX2    = data[4] << 8 | data[5];

    int encoderDelta1 = (int8_t)data[6];
    int encoderDelta2 = (int8_t)data[7];

    if (volumeMaster != mLastVolumeMaster && mVolumeChangeCallback) {
        mVolumeChangeCallback(VolumeChannel::VC_MASTER, volumeMaster);
    }
    if (volumeFX1 != mLastVolumeFX1 && mVolumeChangeCallback) {
        mVolumeChangeCallback(VolumeChannel::VC_USB, volumeFX1);
    }
    if (volumeFX2 != mLastVolumeFX2 && mVolumeChangeCallback) {
        mVolumeChangeCallback(VolumeChannel::VC_FX, volumeFX2);
    }

    mLastVolumeMaster = volumeMaster;
    mLastVolumeFX1 = volumeFX1;
    mLastVolumeFX2 = volumeFX2;

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

    if (i2cOpen(DEV_I2C) < 0) {
        return -1;
    }
    if (i2cSelect(Controller::MC_MainPanel) < 0) {
        return -1;
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
