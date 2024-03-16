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
#include "Pedal.h"

#include <inttypes.h>
#include <avr/io.h>

#include <avrlib.h>

#include "config.h"

Pedal::Pedal() : mKeyChangeHandler(NULL) {
}

void Pedal::setHandleKeyChange( void(*handler)(uint8_t key, uint8_t velocity) ) {
    mKeyChangeHandler = handler;
}

void Pedal::begin()
{
    uint8_t i;

    // init with line 0
    mPedalLine = 0;

    // init ports (in port without pullups, out ports, all disabled)
    IO_DDR(PORT_IN) = 0x00;
    IO_PORT(PORT_IN) = 0x00;

    IO_DDR(PORT_LINE) |= (0x0F << PIN_LINE0);
    IO_PORT(PORT_LINE) |= (0x0F << PIN_LINE0);

    // enable line0
    IO_PORT(PORT_LINE) &= ~(1<<PIN_LINE0);

    for ( i = 0; i < 4; i++ ) {
	    mPedalState[i] = 0x00;
    }
}

void Pedal::poll()
{
    uint8_t line = mPedalLine;
    uint8_t newline = (line + 1) % 4;

    uint8_t i;
    char bit = 1;

    // read status from current line
    char input = IO_PIN(PORT_IN);

    // enable next line 
    IO_PORT(PORT_LINE) |= (1<<(line + PIN_LINE0));
    IO_PORT(PORT_LINE) &= ~(1<<(newline + PIN_LINE0));

    for (i = 0; i < 8; i++) {
	    if ( (input & bit) && !(mPedalState[line] & bit) ) {
            // Key is being pressed
            mKeyChangeHandler( (line<<3) | i, PEDAL_VELOCITY);
	    } else if ( !(input & bit) && (mPedalState[line] & bit) ) {
            // Key is being released
            mKeyChangeHandler( (line<<3) | i, 0);
	    }

	    bit <<= 1;
    }

    // update pedal states
    mPedalState[line] = input;
    mPedalLine = newline;
}
