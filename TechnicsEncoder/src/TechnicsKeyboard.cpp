/*
 * @project     TechnicsController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Input processing routines.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "TechnicsKeyboard.h"

#include <inttypes.h>
#include <avr/io.h>

#include <avrlib.h>

#include <common_config.h>

#include "config.h"

static const uint8_t KEY_NOTE_OFF = 0;

TechnicsKeyboard::TechnicsKeyboard() : mKeyChangeHandler(NULL) {
}

void TechnicsKeyboard::setHandleKeyChange( void(*handler)(uint8_t note, uint8_t velocity) ) {
    mKeyChangeHandler = handler;
}

bool TechnicsKeyboard::readData(uint8_t &data)
{
    int i;

    // Wait for clock to be high
    i = 0;
    while (digitalRead(PIN_CLOCK) == LOW) {
        delayMicroseconds(1);
        i++;
        // timeout
        if (i > 1000) { return false; }
    }

    // Latch in data
    digitalWrite(PIN_LATCH, LOW);
    delayMicroseconds(1);
    // Pin B7 is read pin, ignore its data
    data = IO_PIN(PORT_IN) & 0x7F;
    digitalWrite(PIN_LATCH, HIGH);

    // Clock is ~5us low, might be too fast to see.
    // Just wait for the next clock cycle instead.
    delayMicroseconds(4);

    return true;
}

void TechnicsKeyboard::begin()
{
    // init ports (in port without pullups, out ports, all disabled)
    IO_DDR(PORT_IN) = 0x00;
    IO_PORT(PORT_IN) = 0x00;

    pinMode(PIN_ENABLE, OUTPUT);
    pinMode(PIN_LATCH, OUTPUT);
    pinMode(PIN_READ, OUTPUT);

    pinMode(PIN_CLOCK, INPUT);

    digitalWrite(PIN_READ, LOW);
    digitalWrite(PIN_LATCH, HIGH);

    digitalWrite(PIN_ENABLE, HIGH);

    for (int i = 0; i < NUM_KEYS; i++) {
        mLastKeysState[i] = KEY_NOTE_OFF;
    }

    delayMicroseconds(1000);
}

void TechnicsKeyboard::poll()
{
    // Start read cycle
    digitalWrite(PIN_READ, HIGH);

    uint8_t pressedKeys[NUM_KEYS];

    for (int note = 0; note < NUM_KEYS; note++) {
        pressedKeys[note] = KEY_NOTE_OFF;
    }

    // Read data from Technics keyboard controller
    for (int i = 0; i < NUM_KEYS; i++) {
        uint8_t note;
        uint8_t velocity;

        if (!readData(note)) {
            // Read error
            break;
        }
        if (!readData(velocity)) {
            // Read error
            break;
        }

        // Stop message, no more data
        if (note == 126 && velocity == 126) {
            break;
        }

        // Got a pressed key
        if (note < NUM_KEYS) {
            pressedKeys[note] = velocity;
        }
    }

    // finish read cycle
    digitalWrite(PIN_READ, LOW);

    // Process all received events
    if (mKeyChangeHandler) {
        for (int note = 0; note < NUM_KEYS; note++) {
            // Check for changes, trigger handler on change.
            if (pressedKeys[note] != mLastKeysState[note]) {
                // Note: if note is no longer pressed, then its new state is 0 (NOTE_OFF).
                mKeyChangeHandler(MIDI_NOTE_C2 + note, pressedKeys[note]);
            }

            mLastKeysState[note] = pressedKeys[note];
        }
    }
}
