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

static const uint8_t NUM_OUTPUT_PINS = 3;
static const uint8_t OUTPUT_PINS[5] = {PIN_KP_L1, PIN_KP_L2, PIN_KP_L3};

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
    for (uint8_t i = 0; i  < NUM_ENCODERS; i++) {
        mEncoder[i] = 127;
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
    pinMode(PIN_BTN1,   INPUT_PULLUP);
    pinMode(PIN_BTN2,   INPUT_PULLUP);
    pinMode(PIN_S0, OUTPUT);
    pinMode(PIN_S1, OUTPUT);
    pinMode(PIN_S2, OUTPUT);
}

uint8_t MainPanel::getBtnNumber(uint8_t line, uint8_t pin)
{
    if (pin < 8) {
        if (line == 2) {
            if (pin < 2) {
                // Buttons 6..7 are encoder buttons
                return 6 + pin;
            } else {
                // Buttons 8..9 are OK, Back buttons
                return 8 + (pin-2);
            }
        } else {
            // Buttons 10..17 are menu buttons
            return 10 + line * 4 + pin;
        }
    } else {
        // Buttons 0..5 are LED buttons
        return line * 2 + (pin-8);
    }
}

void MainPanel::readEncoder(uint8_t encoder, uint8_t a, uint8_t b)
{
    int lastPos = mEncoder[encoder];
    // map A/B to quadrant number
    int pos = (b) ? (int) 1-a : (int) a;

    if (lastPos < 127) {
        int delta = pos - lastPos;
        // Wrap-around from quadrant 3 to/back to 0
        if (delta > 2) {
            delta -= 4;
        }
        if (delta < -2) {
            delta += 4;
        }
        if (delta != 0 && mEncoderHandler) {
            mEncoderHandler(encoder, delta);
        }
    }

    mEncoder[encoder] = pos;
}

void MainPanel::readButton(uint8_t button, uint8_t value)
{
    if (!value && mButtons[button] < 0xFF) {
        mButtons[button]++;

        // Send Long button press as soon as timeout is reached
        if (mButtons[button] > BUTTON_LONG_PRESS) {
            mButtons[button] = 0xFF;
            if (mButtonHandler) {
                mButtonHandler(button, true);
            }
        }
    }
    if (value && mButtons[button] > 0) {
        // Button was just released

        // Send short press on release
        if (mButtons[button] != 0xFF) {
            if (mButtonHandler) {
                mButtonHandler(button, false);
            }
        }
        mButtons[button] = 0;
    }
}

void MainPanel::readLine(uint8_t line)
{
    uint8_t btn, value;
    uint8_t encoder[4];

    for (uint8_t i = 0; i < 8; i++) {
        digitalWrite(PIN_S0, (i>>0)&0x01);
        digitalWrite(PIN_S1, (i>>1)&0x01);
        digitalWrite(PIN_S2, (i>>2)&0x01);

        delayMicroseconds(50);

        value = digitalRead(PIN_KP_BTN);

        // reading LOW means button is pressed
        if (i < 4) {
            btn = getBtnNumber(line, i);
            readButton(btn, value);
        } else {
            encoder[i-4] = value;
        }
    }

    readEncoder(0, encoder[2], encoder[3]);
    readEncoder(1, encoder[1], encoder[0]);

    btn = getBtnNumber(line, 8);
    value = digitalRead(PIN_BTN1);
    readButton(btn, value);

    btn = getBtnNumber(line, 9);
    value = digitalRead(PIN_BTN2);
    readButton(btn, value);
}

void MainPanel::poll()
{
    uint8_t line;

    for (line = 0; line < NUM_OUTPUT_PINS; line++) {
        digitalWrite(OUTPUT_PINS[line], LOW);

        readLine(line);

        digitalWrite(OUTPUT_PINS[line], HIGH);
    }
}
