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

uint8_t EEMEM confChannel = 0x00;

uint8_t EEMEM confIntensity = 0x0F;

Settings::Settings()
{
}

uint8_t Settings::getMIDIChannel()
{
    uint8_t v = eeprom_read_byte(&confChannel);
    return (v == 0xFF) ? MIDI_CHANNEL_PEDAL : v;
}

uint8_t Settings::getLEDIntensity()
{
    uint8_t v = eeprom_read_byte(&confIntensity);
    return (v == 0xFF) ? 0x0F : v;
}

void Settings::setMIDIChannel(uint8_t channel)
{
    eeprom_write_byte(&confChannel, channel);
}

void Settings::setLEDIntensity(uint8_t intensity)
{
    eeprom_write_byte(&confIntensity, intensity);
}
