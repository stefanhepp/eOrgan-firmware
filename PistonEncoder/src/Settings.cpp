/*
 * @project     PistonController
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

uint8_t EEMEM confChannel1 = 0x00;
uint8_t EEMEM confChannel2 = 0x01;

Settings::Settings()
{
}

uint8_t Settings::getMIDIChannel(uint8_t kbd)
{
    if (kbd == 0) {
        uint8_t v = eeprom_read_byte(&confChannel1);
        return (v == 0xFF) ? MIDI_CHANNEL_KEYBOARD_1 : v;
    } else {
        uint8_t v = eeprom_read_byte(&confChannel2);
        return (v == 0xFF) ? MIDI_CHANNEL_KEYBOARD_2 : v;
    }
}

void Settings::setMIDIChannel(uint8_t kbd, uint8_t channel)
{
    if (kbd == 0) {
        eeprom_write_byte(&confChannel1, channel);
    } else {
        eeprom_write_byte(&confChannel2, channel);
    }
}
