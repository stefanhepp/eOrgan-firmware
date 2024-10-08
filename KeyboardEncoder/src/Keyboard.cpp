/*
 * @project     KeyboardController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Input processing routines.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "Keyboard.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/eeprom.h>

#include <avrlib.h>

#include <common_config.h>

#include "config.h"

static uint8_t EEMEM eeKeyMap[NUM_KEYBOARDS * NUM_LINES * 8];

static int mapIndex(uint8_t kbd, uint8_t key) {
    return kbd * NUM_LINES * 8 + key;
}

static const uint8_t NOT_LEARNING = 0xFF;

 Keyboard::Keyboard() 
 : mLearning(NOT_LEARNING), 
   mLearnNextNote(0),
   mKeyChangeCallback(NULL), 
   mLearnCompleteCallback(NULL)
 {
 }

void Keyboard::setHandleKeyChange( KeyChangeCallback callback ) {
    mKeyChangeCallback = callback;
}

void Keyboard::setLearnCompleteCallback( LearnCompleteCallback callback ) {
    mLearnCompleteCallback = callback;
}

void Keyboard::loadKeyMap() {
    uint8_t v = eeprom_read_byte(eeKeyMap);
    if (v == 0xFF) {
        // not initialized, load default map
        for (uint8_t i = 0; i < NUM_KEYBOARDS * NUM_LINES * 8; i++) {
            mKeyMap[i] = LOWEST_NOTE + i;
        }
    } else {
        for (uint8_t i = 0; i < NUM_KEYBOARDS * NUM_LINES * 8; i++) {
            mKeyMap[i] = eeprom_read_byte(eeKeyMap + i);
        }
    }
}

void Keyboard::storeKeyMap() {
    for (uint8_t i = 0; i < NUM_KEYBOARDS * NUM_LINES * 8; i++) {
        eeprom_write_byte(eeKeyMap + i, mKeyMap[i]);
    }
}

void Keyboard::begin()
{
    uint8_t i;

    // init ports (in port without pullups, out ports, all disabled)
    IO_DDR(PORT_IN) = 0x00;
    IO_PORT(PORT_IN) = 0x00;

    pinMode(PIN_S0, OUTPUT);
    pinMode(PIN_S1, OUTPUT);
    pinMode(PIN_S2, OUTPUT);
    pinMode(PIN_EN1, OUTPUT);
    pinMode(PIN_EN2, OUTPUT);

    digitalWrite(PIN_S0, LOW);
    digitalWrite(PIN_S1, LOW);
    digitalWrite(PIN_S2, LOW);

    digitalWrite(PIN_EN1, LOW);
    digitalWrite(PIN_EN2, LOW);

    for (i = 0; i < NUM_KEYBOARDS * NUM_LINES; i++) {
        mKbdStatus[i] = 0x00;
    }

    loadKeyMap();
}

void Keyboard::startLearning(const uint8_t kbd) {
    mLearning = kbd;
    mLearnNextNote = LOWEST_NOTE;
    mCurrentInput = 0xFF;
}

void Keyboard::learnNextKey(const uint8_t kbd, const uint8_t key) {
    if (key == mCurrentInput) {
        // repeated press of the same key, ignore
        return;
    }

    // The input key for note 'mLearnNextKey' is 'key'..
    mKeyMap[mapIndex(kbd, key)] = mLearnNextNote;

    mCurrentInput = key;
    if (mLearnCompleteCallback) {
        mLearnCompleteCallback(mLearning, mLearnNextNote);
    }

    mLearnNextNote++;
    if (mLearnNextNote >= LOWEST_NOTE + NUM_KEYS) {
        storeKeyMap();
        mLearning = NOT_LEARNING;
    }
}

bool Keyboard::isLearning() const {
    return mLearning != NOT_LEARNING;
}

uint8_t Keyboard::lastLearnedKey() const {
    if (mLearnNextNote > LOWEST_NOTE) {
        return mLearnNextNote - 1;
    } else {
        return 0;
    }
}

void Keyboard::readLine(const uint8_t kbd, const uint8_t line) {
    uint8_t idx = kbd * NUM_LINES + line;
    uint8_t oldStatus = mKbdStatus[idx];

    // Select line; Sn pins are inverted
    digitalWrite(PIN_S0, (line & 0x01) > 0 ? LOW : HIGH);
    digitalWrite(PIN_S1, (line & 0x02) > 0 ? LOW : HIGH);
    digitalWrite(PIN_S2, (line & 0x04) > 0 ? LOW : HIGH);

    // wait for response
    delayMicroseconds(25);

    // read status from current line
    char input = IO_PIN(PORT_IN);

    for (uint8_t i = 0; i < 8; i++) {
        uint8_t key = line * 8 + i;

        if ( ( !(input & (1<<i))) && (oldStatus & (1<<i)) ) {
            // Key was pressed (input low)
            if (mLearning == kbd) {
                learnNextKey(kbd, key);         
            } else {
                mKeyChangeCallback(kbd, mKeyMap[mapIndex(kbd, key)], KEY_VELOCITY);
            }
        } else if ( (input & (1<<i)) && !(oldStatus & (1<<i)) ) {
            // Key was released
            if (mLearning == kbd) {
                // ignored
            } else {
                mKeyChangeCallback(kbd, mKeyMap[mapIndex(kbd, key)], 0);
            }
        }
    }

    mKbdStatus[idx] = input;
}

void Keyboard::poll()
{
    uint8_t line;

    digitalWrite(PIN_EN1, HIGH);

    for (line = 0; line < 8; line++) {
        readLine(0, line);        
    }

    digitalWrite(PIN_EN1, LOW);
    digitalWrite(PIN_EN2, HIGH);

    for (line = 0; line < 8; line++) {
        readLine(1, line);        
    }

    digitalWrite(PIN_EN2, LOW);

}
