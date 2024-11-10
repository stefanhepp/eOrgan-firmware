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

PanelInterface::PanelInterface(OrganStateManager &stateManager, CouplerProcessor &coupler,
                               MIDIRouter &router, AudioProcessor &audio)
: mStateManager(stateManager), mCoupler(coupler), mRouter(router), mAudio(audio)
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
    //Serial.printf("RX Buf: %d, Len: %d, Com: %d < data: %hhu\n", 
    //              mRxBufferLength, mRxCommandLength, mRxCommand, data);
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
    if (mRxBufferLength >= mRxCommandLength) {
        mRxReadPos = 0;
        processCommand(mRxCommand);
        mRxBufferLength = -1;
    }
}

void PanelInterface::processCommand(uint8_t command) {
    uint8_t output, mode, channel, direction;
    uint8_t volumeLow, volumeHigh;
    int volume;

    switch (command) {
        case PANEL_CMD_STATUS:
            sendStatus();
            break;
        case PANEL_CMD_COUPLER:
            mode = read();
            switch (mode) {
                case PANEL_COUPLER_DISABLED:
                    mStateManager.setCouplerMode(CouplerMode::CM_DISABLED);
                    break;
                case PANEL_COUPLER_MIDI:
                    mStateManager.setCouplerMode(CouplerMode::CM_MIDI);
                    break;
                case PANEL_COUPLER_ENABLED:
                    mStateManager.setCouplerMode(CouplerMode::CM_ENABLED);
                    break;
                default:
                    break;
            }
            break;
        case PANEL_CMD_ROUTER:
            output = read();
            mRouter.enableUSBOutput(output & 0x01 ? true : false);
            mRouter.enableMIDIOutput(output & 0x02 ? true : false);
            break;
        case PANEL_CMD_VOLUME:
            channel = read();
            volumeHigh = read();
            volumeLow = read();
            volume = ((int)(volumeHigh) << 8)|(int)volumeLow;
            switch (channel) {
                case PANEL_CHANNEL_MASTER:
                    mAudio.setVolume((float)(volume)/1023.0);
                    break;
                case PANEL_CHANNEL_USB:
                    mAudio.setUSBInputGain((float)(volume)/1023.0);
                    break;
                case PANEL_CHANNEL_FX:
                    mAudio.setFXInputGain((float)(volume)/1023.0);
                    break;
                default:
                    break;
            }
            break;
        case PANEL_CMD_PAGE_TURN:
            direction = read();
            switch (direction) {
                case PANEL_PAGE_NEXT:
                    mCoupler.sendPageTurn(ButtonType::BT_NEXT);
                    break;
                case PANEL_PAGE_PREV:
                    mCoupler.sendPageTurn(ButtonType::BT_PREV);
                    break;
            }
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
    routerState |= mCoupler.doSendMIDICommands();

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
        uint8_t data = Serial6.read();
        processSerialData(data);
    }
}