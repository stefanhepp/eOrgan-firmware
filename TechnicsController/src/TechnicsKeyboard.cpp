/*
 * @project     UsbRc
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-05 03:00:41 +0200 (Son, 05 Aug 2007) $
 * @version     $Id: input.c 111 2007-08-05 01:00:41Z stefan $
 *
 * Input processing routines.
 *
 * Copyright 2007 Stefan Hepp
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
    pinMode(PIN_LATCH, OUTPUT);
    pinMode(PIN_CLOCK, OUTPUT);

    digitalWrite(PIN_LATCH, HIGH);
    digitalWrite(PIN_CLOCK, HIGH);

    digitalWrite(PIN_ENABLE, HIGH);

    delayMicroseconds(50);
}

void TechnicsKeyboard::poll()
{
    
}
