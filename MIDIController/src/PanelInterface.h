/**
 * @project     MidiController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Panel UART control interface handler.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#include <inttypes.h>

#include <common_config.h>

#include "OrganStateManager.h"
#include "MIDIRouter.h"
#include "AudioProcessor.h"
#include "CouplerProcessor.h"

static const uint8_t CMD_BUFFER_SIZE = 64;

class PanelInterface
{
    private:
        OrganStateManager &mStateManager;
        CouplerProcessor  &mCoupler;
        MIDIRouter        &mRouter;
        AudioProcessor    &mAudio;

        uint8_t mRxBuffer[CMD_BUFFER_SIZE];
        int     mRxBufferLength = -1;
        int     mRxReadPos = 0;
    
        uint8_t mRxCommand;
        int     mRxCommandLength = -1;

        uint8_t mTxBuffer[CMD_BUFFER_SIZE];
        uint8_t mTxCommand;
        int     mTxBufferLength = -1;

        /**
         * To be called within processCommand, read next received command data byte.
         */
        uint8_t read();

        /**
         * To be called between beginTransmission() and endTransmission(), add one byte to command data.
         */
        void write(uint8_t data);

        void beginTransmission(uint8_t command);

        void endTransmission();

        void processSerialData(uint8_t data);

        void processCommand(uint8_t command);

    public:
        explicit PanelInterface(OrganStateManager &stateManager, CouplerProcessor &coupler,
                                MIDIRouter &router, AudioProcessor &audio);

        void sendStatus();

        void begin();

        void loop();
};
