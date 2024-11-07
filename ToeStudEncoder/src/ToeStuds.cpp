/*
 * @project     TueStudController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Input processing routines.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "ToeStuds.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avrlib.h>

#include <common_config.h>

#include "config.h"

static const uint8_t INPUT_PINS[5] = {PIN_D1, PIN_D2, PIN_D3, PIN_D4, PIN_D5};
static const uint8_t OUTPUT_PINS[4] = {PIN_L1, PIN_L2, PIN_L3, PIN_L4};

ToeStuds::ToeStuds()
{
    reset();
}

void ToeStuds::setHandleButton( void(*handler)(uint8_t button, bool longPress) ) {
    mButtonHandler = handler;
}

void ToeStuds::reset()
{
    for (uint8_t i = 0; i < NUM_TOESTUDS; i++) {
        mToeStuds[i] = 0;
    }
}

void ToeStuds::begin()
{
    for (int i = 0; i < 4; i++) {
        pinMode(OUTPUT_PINS[i], OUTPUT);
        digitalWrite(OUTPUT_PINS[i], HIGH);
    }
    for (int i = 0; i < 5; i++) {
        pinMode(INPUT_PINS[i], INPUT_PULLUP);
    }
}

uint8_t ToeStuds::getBtnNumber(uint8_t line, uint8_t pin)
{
    return line * 5 + pin;
}

void ToeStuds::readLine(const uint8_t line) {
    for (uint8_t i = 0; i < 5; i++) {
        uint8_t btn = getBtnNumber(line, i);
        uint8_t value = digitalRead(INPUT_PINS[i]);

        // reading LOW means button is pressed
        if (!value && mToeStuds[btn] < 0xFF) {
            mToeStuds[btn]++;

            // Send Long button press as soon as timeout is reached
            if (mToeStuds[btn] > TOESTUD_LONG_PRESS) {
                mToeStuds[btn] = 0xFF;
                if (mButtonHandler) {
                    mButtonHandler(btn, true);
                }
            }
        }
        if (value && mToeStuds[btn] > 0) {
            // Button was just released

            // Send short press on release
            if (mToeStuds[btn] != 0xFF) {
                if (mButtonHandler) {
                    mButtonHandler(btn, false);
                }
            }
            mToeStuds[btn] = 0;
        }
    }
}

void ToeStuds::poll()
{
    uint8_t line;

    for (line = 0; line < 4; line++) {
        digitalWrite(OUTPUT_PINS[line], LOW);

        delayMicroseconds(300);

        readLine(line);

        digitalWrite(OUTPUT_PINS[line], HIGH);
    }
}
