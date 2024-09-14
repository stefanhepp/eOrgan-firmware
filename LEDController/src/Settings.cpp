/*
 * @project     Midi Pedalboard
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

uint8_t EEMEM confIntensity[8];

Settings::Settings()
{
}

uint8_t Settings::getLEDIntensity(int index)
{
    return eeprom_read_byte(&confIntensity[index]);
}

void Settings::setLEDIntensity(int index, uint8_t intensity)
{
    eeprom_write_byte(&confIntensity[index], intensity);
}
