/*
 * @project     Technics Keyboard driver
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Input encoding and processing routines.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __TECHNICSKEYBOARD_H__
#define __TECHNICSKEYBOARD_H__

#include <inttypes.h>

static const int NUM_KEYS = 61;

class TechnicsKeyboard
{
    private:

        uint8_t mLastKeysState[NUM_KEYS];

        // callback for key changes
        void (*mKeyChangeHandler)(uint8_t key, uint8_t velocity);

        bool readData(uint8_t &data);
        
    public:
        explicit TechnicsKeyboard();

        void setHandleKeyChange(void (*handler)(uint8_t note, uint8_t velocity) );

        /**
         * Initialize all pins and routines.
         **/
        void begin();

        /** 
         * poll input ports for changes, call handler on changed notes.
         **/
        void poll();
};

#endif
