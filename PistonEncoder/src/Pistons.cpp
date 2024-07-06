/*
 * @project     PistonController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Input processing routines.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "Pistons.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/eeprom.h>

#include <avrlib.h>

#include <common_config.h>

#include "config.h"

static uint8_t kbdStatus[NUM_KEYBOARDS * NUM_LINES];

static EEMEM uint8_t eeKeyMap = 0x04;

void Keyboard::setHandleKeyChange( void(*handler)(uint8_t kbd, uint8_t note, uint8_t velocity) ) {
    mKeyChangeHandler = handler;
}

void Keyboard::loadKeyMap() {
    uint8_t v = eeprom_read_byte(&eeKeyMap);
    if (v == 0xFF) {
        // not initialized, load default map
        for (uint8_t i = 0; i < NUM_KEYBOARDS * NUM_LINES * 8; i++) {
            mKeyMap[i] = LOWEST_NOTE + i;
        }
    } else {
        for (uint8_t i = 0; i < NUM_KEYBOARDS * NUM_LINES * 8; i++) {
            mKeyMap[i] = eeprom_read_byte(&eeKeyMap + i);
        }
    }
}

void Keyboard::storeKeyMap() {
    for (uint8_t i = 0; i < NUM_KEYBOARDS * NUM_LINES * 8; i++) {
        eeprom_write_byte(&eeKeyMap + i, mKeyMap[i]);
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
        kbdStatus[i] = 0x00;
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
    mKeyMap[key] = mLearnNextNote;

    mCurrentInput = key;
    mLearnNextNote++;
    if (mLearnNextNote >= NUM_KEYS) {
        storeKeyMap();
        mLearning = 0xFF;
    }
}

void Keyboard::readLine(const uint8_t kbd, const uint8_t line) {
    uint8_t idx = kbd * NUM_KEYBOARDS + line;
    uint8_t oldStatus = kbdStatus[idx];

    // Select line
    digitalWrite(PIN_S0, (line & 0x01) > 0 ? HIGH : LOW);
    digitalWrite(PIN_S1, (line & 0x02) > 0 ? HIGH : LOW);
    digitalWrite(PIN_S2, (line & 0x04) > 0 ? HIGH : LOW);

    // wait for response
    delayMicroseconds(20);

    // read status from current line
    char input = IO_PIN(PORT_IN);

    for (uint8_t i = 0; i < 8; i++) {
        uint8_t key = line * 8 + i;

        if ( (input & (1<<i)) && !(oldStatus & (1<<i)) ) {
            // Key was pressed
            if (mLearning == kbd) {
                learnNextKey(kbd, key);         
            } else {
                mKeyChangeHandler(kbd, mKeyMap[key], KEY_VELOCITY);
            }
        } else if ( !(input & (1<<i)) && (oldStatus & (1<<i)) ) {
            // Key was released
            if (mLearning == kbd) {
                // ignored
            } else {
                mKeyChangeHandler(kbd, mKeyMap[key], 0);
            }
        }
    } 

    kbdStatus[idx] = input;
}

void Keyboard::poll()
{
    uint8_t line;

    digitalWrite(PIN_EN1, HIGH);

    for (line = 0; line < 7; line++) {
        readLine(0, line);        
    }

    digitalWrite(PIN_EN1, LOW);
    digitalWrite(PIN_EN2, HIGH);

    for (line = 0; line < 7; line++) {
        readLine(1, line);        
    }

    digitalWrite(PIN_EN2, LOW);

}
