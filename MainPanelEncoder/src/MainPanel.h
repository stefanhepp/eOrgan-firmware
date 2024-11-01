/*
 * @project     MainPanelEncoder
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

static const uint8_t NUM_BUTTONS = 25;
static const uint8_t BUTTON_LONG_PRESS = 200;

class MainPanel
{
    private:
        // callback for key changes
        void (*mButtonHandler)(uint8_t btn, bool longPress) = nullptr;
        void (*mEncoderHandler)(uint8_t encoder, int position) = nullptr;

        uint8_t mButtons[NUM_BUTTONS];

        uint8_t getBtnNumber(uint8_t line, uint8_t pin);

        void readLine(const uint8_t line);
    public:
        explicit MainPanel();

        void reset();

        void setHandleButton(void (*handler)(uint8_t btn, bool longPress) );

        void setHandleEncoder(void (*handler)(uint8_t encoder, int position) );

        /**
         * Initialize all pins and routines.
         **/
        void begin();

        /** 
         * poll input ports for changes, call handler on changed notes.
         **/
        void poll();
};
