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

TechnicsKeyboard::TechnicsKeyboard() : mKeyChangeHandler(NULL) {
}

void TechnicsKeyboard::setHandleKeyChange( void(*handler)(uint8_t note, uint8_t velocity) ) {
    mKeyChangeHandler = handler;
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

    delayMicroseconds(50);
}

void TechnicsKeyboard::poll()
{
    
}
