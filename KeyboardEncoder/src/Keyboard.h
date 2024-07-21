/*
 * @project     Midi KeyboardController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Input encoding and processing routines.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <inttypes.h>

#include "config.h"

static const uint8_t NUM_KEYBOARDS = 2;
static const uint8_t NUM_LINES = 8;
static const uint8_t NUM_KEYS = 61;

using KeyChangeCallback     = void(*)(uint8_t kbd, uint8_t note, uint8_t velocity);
using LearnCompleteCallback = void(*)(uint8_t kbd);

class Keyboard
{
    private:
        uint8_t mLearning;
        uint8_t mCurrentInput;
        uint8_t mLearnNextNote;

        uint8_t mKeyMap[NUM_KEYBOARDS * NUM_LINES * 8];

        // callback for key changes
        KeyChangeCallback mKeyChangeCallback;
        LearnCompleteCallback mLearnCompleteCallback;

        void loadKeyMap();

        void storeKeyMap();

        void learnNextKey(const uint8_t kbd, const uint8_t key);

        void readLine(const uint8_t kbd, const uint8_t line);
    public:
        explicit Keyboard() : mLearning(0xFF), mKeyChangeCallback(NULL), mLearnCompleteCallback(NULL) {}

        void setHandleKeyChange(KeyChangeCallback callback);

        void setLearnCompleteCallback(LearnCompleteCallback callback);

        /**
         * Initialize all pins and routines.
         **/
        void begin();

        bool isLearning() const { return mLearning != 0xFF; }

        void startLearning(uint8_t kbd);

        /** 
         * poll input ports for changes, call handler on changed notes.
         **/
        void poll();
};

#endif
