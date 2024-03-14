/*
 * @project     Midi Pedalboard
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:21:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: input.h 108 2007-08-04 21:21:53Z stefan $
 *
 * Implementation of EEPROM access.
 *
 * Copyright 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "setup.h"

#include <avr/eeprom.h>

uint8_t EEMEM confChannel = 0x00;

uint8_t EEMEM confIntensity = 0x0F;

uint8_t confReadChannel(void)
{
    uint8_t v = eeprom_read_byte(&confChannel);
    return (v == 0xFF) ? 0 : v;
}

uint8_t confReadIntensity(void)
{
    uint8_t v = eeprom_read_byte(&confIntensity);
    return (v == 0xFF) ? 0x0F : v;
}

void confWriteChannel(uint8_t channel)
{
    eeprom_write_byte(&confChannel, channel);
}

void confWriteIntensity(uint8_t intensity)
{
    eeprom_write_byte(&confIntensity, intensity);
}
