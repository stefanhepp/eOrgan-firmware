/*
 * @project     ToeStudController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Input encoding and processing routines.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#include <inttypes.h>

static const uint8_t NUM_TOESTUDS = 25;
static const uint8_t TOESTUD_LONG_PRESS = 200;

class ToeStuds
{
    private:
        // callback for key changes
        void (*mButtonHandler)(uint8_t btn, bool longPress) = nullptr;

        uint8_t mToeStuds[NUM_TOESTUDS];

        uint8_t getBtnNumber(uint8_t line, uint8_t pin);

        void readLine(const uint8_t line);
    public:
        explicit ToeStuds();

        void setHandleButton(void (*handler)(uint8_t btn, bool longPress) );

        void reset();

        /**
         * Initialize all pins and routines.
         **/
        void begin();

        /** 
         * poll input ports for changes, call handler on changed notes.
         **/
        void poll();
};
