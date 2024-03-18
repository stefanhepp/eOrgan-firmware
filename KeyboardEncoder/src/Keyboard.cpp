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
#include "Keyboard.h"

#include <inttypes.h>
#include <avr/io.h>

#include <avrlib.h>

#include "config.h"

Keyboard::Keyboard() : mKeyChangeHandler(NULL) {
}

void Keyboard::setHandleKeyChange( void(*handler)(uint8_t kbd, uint8_t key, uint8_t velocity) ) {
    mKeyChangeHandler = handler;
}

void Keyboard::begin()
{
    // init ports (in port without pullups, out ports, all disabled)
    IO_DDR(PORT_IN) = 0x00;
    IO_PORT(PORT_IN) = 0x00;

    pinMode(PIN_S0, OUTPUT);
    pinMode(PIN_S1, OUTPUT);
    pinMode(PIN_S2, OUTPUT);
    pinMode(PIN_EN1, OUTPUT);
    pinMode(PIN_EN2, OUTPUT);

}

void Keyboard::poll()
{
    // read status from current line
    char input = IO_PIN(PORT_IN);


}
