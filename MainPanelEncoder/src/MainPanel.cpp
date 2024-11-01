/*
 * @project     MainPanelEncoder
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Input processing routines.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "MainPanel.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avrlib.h>

#include <common_config.h>

#include "config.h"

static const uint8_t NUM_OUTPUT_PINS = 5;
static const uint8_t OUTPUT_PINS[5] = {PIN_KP_L1, PIN_KP_L2, PIN_KP_L3, PIN_BTN1, PIN_BTN2};

MainPanel::MainPanel()
{
    reset();
}

void MainPanel::setHandleButton( void(*handler)(uint8_t button, bool longPress) ) {
    mButtonHandler = handler;
}

void MainPanel::setHandleEncoder( void(*handler)(uint8_t encoder, int position) ) {
    mEncoderHandler = handler;
}

void MainPanel::reset()
{
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        mButtons[i] = 0;
    }
}

void MainPanel::begin()
{
    uint8_t i;
    for (i = 0; i < NUM_OUTPUT_PINS; i++) {
        pinMode(OUTPUT_PINS[i], OUTPUT);
        digitalWrite(OUTPUT_PINS[i], HIGH);
    }
    pinMode(PIN_KP_BTN, INPUT_PULLUP);
    pinMode(PIN_S0, OUTPUT);
    pinMode(PIN_S1, OUTPUT);
    pinMode(PIN_S2, OUTPUT);
}

uint8_t MainPanel::getBtnNumber(uint8_t line, uint8_t pin)
{
    return line * NUM_OUTPUT_PINS + pin;
}

void MainPanel::readLine(const uint8_t line) {
    for (uint8_t i = 0; i < NUM_OUTPUT_PINS; i++) {
        uint8_t btn = getBtnNumber(line, i);
        uint8_t value = digitalRead(PIN_KP_BTN);

        // reading LOW means button is pressed
        if (!value && mButtons[btn] < 0xFF) {
            mButtons[btn]++;

            // Send Long button press as soon as timeout is reached
            if (mButtons[btn] > BUTTON_LONG_PRESS) {
                mButtons[btn] = 0xFF;
                if (mButtonHandler) {
                    mButtonHandler(btn, true);
                }
            }
        }
        if (value && mButtons[btn] > 0) {
            // Button was just released

            // Send short press on release
            if (mButtons[btn] != 0xFF) {
                if (mButtonHandler) {
                    mButtonHandler(btn, false);
                }
            }
            mButtons[btn] = 0;
        }

    }
}

void MainPanel::poll()
{
    uint8_t line;

    for (line = 0; line < 3; line++) {
        digitalWrite(OUTPUT_PINS[line], LOW);

        delayMicroseconds(300);

        readLine(line);

        digitalWrite(OUTPUT_PINS[line], HIGH);
    }
}
