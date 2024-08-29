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

uint8_t EEMEM confChannel;
uint16_t EEMEM confWheelMin;
uint16_t EEMEM confWheelMax;
uint16_t EEMEM confWheelCenter;

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

bool Settings::hasSettings()
{
    uint16_t v = eeprom_read_word(&confWheelMin);
    return (v != 0xFFFF);
}

void Settings::getWheelSettings(AICalibrationData &data)
{
    data.min = (int) eeprom_read_word(&confWheelMin);
    data.max = (int) eeprom_read_word(&confWheelMax);
    data.center = (int) eeprom_read_word(&confWheelCenter);
}

void Settings::setWheelSettings(const AICalibrationData &data)
{
    eeprom_write_word(&confWheelMin, (uint16_t) data.min);
    eeprom_write_word(&confWheelMax, (uint16_t) data.max);
    eeprom_write_word(&confWheelCenter, (uint16_t) data.center);
}
