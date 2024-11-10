/**
 * @project     PanelUI
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Panel UART control interface.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#include <stdint.h>

#include <common_config.h>
#include <panel_commands.h>

#include "commonTypes.h"

static const uint8_t CMD_BUFFER_SIZE = 64;

struct StatusInfo {
    bool USBOutputEnabled;
    bool MIDIOutputEnabled;
    bool CouplerEnabled;
    bool SendMIDIMessages;
};

using StatusCallback = void(*)(StatusInfo &info);

class SerialInterface
{
    private:
        StatusCallback  mStatusCallback = nullptr;

        uint8_t mRxBuffer[CMD_BUFFER_SIZE];
        int     mRxBufferLength = -1;
        int     mRxReadPos = 0;
    
        uint8_t mRxCommand;
        int     mRxCommandLength = -1;

        uint8_t mTxBuffer[CMD_BUFFER_SIZE];
        uint8_t mTxCommand;
        int     mTxBufferLength = -1;

        int     mFdSerial = -1;


        int serialOpen(const char* device, uint32_t baud);

        void serialWrite(uint8_t data);

        int serialBufferSize();

        int serialRead();

        void serialFlush();

        void serialClose();


        /**
         * To be called within processCommand, read next received command data byte.
         */
        uint8_t readByte();

        /**
         * To be called between beginTransmission() and endTransmission(), add one byte to command data.
         */
        void writeByte(uint8_t data);

        void beginTransmission(uint8_t command);

        void endTransmission();

        void processSerialData(int data);

        void processCommand(uint8_t command);

    public:
        explicit SerialInterface();

        void setStatusCallback(StatusCallback callback) { mStatusCallback = callback; }

        void requestStatus();

        void enableRouter(bool USB, bool MIDI);

        void sendCouplerMode(CouplerMode mode);

        void sendVolume(VolumeChannel channel, int volume);

        void sendPageTurn(PageTurn direction);

        int begin();

        void loop();
};
