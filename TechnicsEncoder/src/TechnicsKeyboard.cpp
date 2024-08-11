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

#include "config.h"

struct KeyEvent {
    uint8_t note;
    uint8_t velocity;
};

static const uint8_t BUFFER_SIZE = 64;

TechnicsKeyboard::TechnicsKeyboard() : mKeyChangeHandler(NULL) {
}

void TechnicsKeyboard::setHandleKeyChange( void(*handler)(uint8_t note, uint8_t velocity) ) {
    mKeyChangeHandler = handler;
}

uint8_t TechnicsKeyboard::readData()
{
    // Wait for clock to be high
    while (digitalRead(PIN_CLOCK) == LOW);

    // Wait for Clock falling edge
    while (digitalRead(PIN_CLOCK) == HIGH);    

    // Pin B7 is read pin, ignore its data
    return IO_PORT(PORT_IN) & 0x7F;
}

void TechnicsKeyboard::begin()
{
    // init ports (in port without pullups, out ports, all disabled)
    IO_DDR(PORT_IN) = 0x00;
    IO_PORT(PORT_IN) = 0x00;

    pinMode(PIN_ENABLE, OUTPUT);
    pinMode(PIN_LATCH, INPUT);
    pinMode(PIN_CLOCK, INPUT);

    pinMode(PIN_READ, OUTPUT);
    digitalWrite(PIN_READ, LOW);

    digitalWrite(PIN_ENABLE, HIGH);

    delayMicroseconds(1000);
}

void TechnicsKeyboard::poll()
{
    // Start read cycle
    digitalWrite(PIN_READ, HIGH);

    KeyEvent Buffer[BUFFER_SIZE];
    uint8_t BufferLength = 0;

    // Read data from Technics keyboard controller
    for (int i = 0; i < BUFFER_SIZE; i++) {
        uint8_t note = readData();
        uint8_t velocity = readData();

        // Stop message, no more data
        if (note == 0x7F && velocity == 0x7F) {
            break;
        }

        if (BufferLength < BUFFER_SIZE) {
            Buffer[BufferLength].note = note;
            Buffer[BufferLength].velocity = velocity;
            BufferLength++;
        }
    }

    // finish read cycle
    digitalWrite(PIN_READ, LOW);

    // Process all received events
    if (mKeyChangeHandler) {
        for (uint8_t i = 0; i < BufferLength; i++) {
            mKeyChangeHandler(Buffer[i].note, Buffer[i].velocity);
        }
    }
}
