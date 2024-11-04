/*
 * @project     MIDIController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Panel UART interface handler implementation.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "PanelInterface.h"

#include <Arduino.h>
#include <inttypes.h>

#include <common_config.h>
#include <panel_commands.h>

PanelInterface::PanelInterface(CouplerProcessor &coupler, MIDIRouter &router, AudioProcessor &audio)
: mCoupler(coupler), mRouter(router), mAudio(audio)
{
}

uint8_t PanelInterface::read() {
    if (mRxReadPos < mRxBufferLength) {
        return mRxBuffer[mRxReadPos++];
    }
    return 0xFF;
}

void PanelInterface::write(uint8_t data) {
    if (mTxBufferLength != -1 && mTxBufferLength < CMD_BUFFER_SIZE) {
        mTxBuffer[mTxBufferLength++] = data;
    }
}

void PanelInterface::beginTransmission(uint8_t command) {
    mTxCommand = command;
    mTxBufferLength = 0;
}

void PanelInterface::endTransmission() {
    Serial6.write(0x80 | (mTxBufferLength & 0xFF));
    Serial6.write(mTxCommand);
    for (int i = 0; i<mTxBufferLength; i++) {
        Serial6.write(mTxBuffer[i]);
    }
    Serial6.flush();
    mTxBufferLength = -1;
}

void PanelInterface::processSerialData(uint8_t data) {
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

void PanelInterface::processCommand(uint8_t command) {
    uint8_t output, state;

    switch (command) {
        case PANEL_CMD_STATUS:
            sendStatus();
            break;
        case PANEL_CMD_COUPLER:
            state = read();


            break;
        case PANEL_CMD_ROUTER:
            output = read();


            break;
        default:
            break;
    }
}

void PanelInterface::sendStatus() {
    beginTransmission(PANEL_CMD_STATUS);

    uint8_t midiOutput = 0x00;
    midiOutput |= mRouter.isUSBOutEnabled()  ? 0x01 : 0;
    midiOutput |= mRouter.isMIDIOutEnabled() ? 0x02 : 0;

    uint8_t routerState = 0x00;
    routerState |= mRouter.isCouplerEnabled() ? 0x01 : 0;
    routerState |= 0; // mCoupler.doSendMIDIMessages();

    write(midiOutput);
    write(routerState);

    endTransmission();
}

void PanelInterface::begin()
{
    Serial6.begin(PANEL_UART_SPEED, SERIAL_8N1);
}

void PanelInterface::loop()
{
    while (Serial6.available()) {
        processSerialData(Serial6.read());
    }
}