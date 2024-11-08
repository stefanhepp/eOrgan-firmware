/*
 * @project     PanelUI
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * UART interface to main controller implementation.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "SerialInterface.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include <iostream>

#include <wiringSerial.h>

#include <common_config.h>
#include <panel_commands.h>

#include "config.h"


SerialInterface::SerialInterface()
{
}

uint8_t SerialInterface::read()
{
    if (mRxReadPos < mRxBufferLength) {
        return mRxBuffer[mRxReadPos++];
    }
    return 0xFF;
}

void SerialInterface::write(uint8_t data)
{
    if (mTxBufferLength != -1 && mTxBufferLength < CMD_BUFFER_SIZE) {
        mTxBuffer[mTxBufferLength++] = data;
    }
}

void SerialInterface::beginTransmission(uint8_t command)
{
    mTxCommand = command;
    mTxBufferLength = 0;
}

void SerialInterface::endTransmission()
{
    serialPutchar(mFdSerial, 0x80 | (mTxBufferLength & 0xFF));
    serialPutchar(mFdSerial, mTxCommand);
    for (int i = 0; i<mTxBufferLength; i++) {
        serialPutchar(mFdSerial, mTxBuffer[i]);
    }
    serialFlush(mFdSerial);
    mTxBufferLength = -1;
}

void SerialInterface::processSerialData(int data)
{
    if (data < 0 || data > 255) {
        return;
    }

    if (mRxBufferLength == -1) {
        // Start of transmission: 0x80 | length
        if (data & 0x80) {
            mRxCommandLength = (data & ~0x80);
            mRxBufferLength = -2;
        }
    }
    else if (mRxBufferLength == -2)  {
        // Second byte: command
        mRxCommand = data;
        // Following bytes are payload
        mRxBufferLength = 0;
    } else {
        if (mRxBufferLength < CMD_BUFFER_SIZE) {
            mRxBuffer[mRxBufferLength] = data;
        }
        mRxBufferLength++;
    }
    if (mRxBufferLength == mRxCommandLength) {
        mRxReadPos = 0;
        processCommand(mRxCommand);
        mRxBufferLength = -1;
    }
}

void SerialInterface::processCommand(uint8_t command)
{
    uint8_t output, routerState;
    StatusInfo statusInfo;

    switch (command) {
        case PANEL_CMD_STATUS:
            // received status
            output = read();
            routerState = read();

            statusInfo.USBOutputEnabled  = output & 0x01 ? true : false;
            statusInfo.MIDIOutputEnabled = output & 0x02 ? true : false;
            statusInfo.CouplerEnabled    = routerState & 0x01 ? true : false;
            statusInfo.SendMIDIMessages  = routerState & 0x02 ? true : false;

            if (mStatusCallback) {
                mStatusCallback(statusInfo);
            }
            break;
        default:
            std::cerr << "Received unknown command: " << command << std::endl;
            break;
    }
}

void SerialInterface::requestStatus()
{
    beginTransmission(PANEL_CMD_STATUS);
    endTransmission();
}

void SerialInterface::enableRouter(bool USB, bool MIDI)
{
    uint8_t output = 0x00;
    output |= USB  ? 0x01 : 0;
    output |= MIDI ? 0x02 : 0;

    beginTransmission(PANEL_CMD_ROUTER);
    write(output);
    endTransmission();
}

void SerialInterface::sendCouplerMode(CouplerMode mode)
{
    beginTransmission(PANEL_CMD_COUPLER);
    write(mode);
    endTransmission();
}

void SerialInterface::sendVolume(VolumeChannel channel, int volume)
{
    if (volume < 0) {
        volume = 0;
    }
    if (volume > 1023) {
        volume = 1023;
    }

    beginTransmission(PANEL_CMD_VOLUME);
    write(volume >> 8);
    write(volume & 0xFF);
    endTransmission();
}

void SerialInterface::sendPageTurn(PageTurn direction)
{
    beginTransmission(PANEL_CMD_PAGE_TURN);
    write(direction);
    endTransmission();
}

int SerialInterface::begin()
{
    if ((mFdSerial = serialOpen(DEV_SERIAL, 115200)) < 0)
    {
        std::cerr << "Unable to open serial device: " << strerror(errno) << std::endl;
        return 1;
    }
    return 0;
}

void SerialInterface::loop()
{
    while (serialDataAvail(mFdSerial)) {
        processSerialData(serialGetchar(mFdSerial));
    }
}
