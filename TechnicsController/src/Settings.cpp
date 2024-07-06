/*
 * @project     TechnicsController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Implementation of EEPROM access.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "Settings.h"

#include <avr/eeprom.h>

#include <common_config.h>

uint8_t EEMEM confChannel  = 0x00;
uint8_t EEMEM confWheelMin = 0x01;
uint8_t EEMEM confWheelMax = 0x02;

Settings::Settings()
{
}

uint8_t Settings::getMIDIChannel()
{
    uint8_t v = eeprom_read_byte(&confChannel);
    return (v == 0xFF) ? MIDI_CHANNEL_TECHNICS : v;
}

void Settings::setMIDIChannel(uint8_t channel)
{
    eeprom_write_byte(&confChannel, channel);
}

uint8_t Settings::getWheelMin()
{
    uint8_t v = eeprom_read_byte(&confChannel);
    return (v == 0xFF) ? 0x00 : v;
}

uint8_t Settings::getWheelMax()
{
    uint8_t v = eeprom_read_byte(&confChannel);
    return (v == 0xFF) ? 0xFF : v;
}

void Settings::setWheelMinMax(uint8_t min, uint8_t max)
{
    eeprom_write_byte(&confWheelMin, min);
    eeprom_write_byte(&confWheelMax, max);
}
