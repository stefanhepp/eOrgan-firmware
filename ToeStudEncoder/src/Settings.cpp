/*
 * @project     ToeStud Encoder
 * @author      Stefan Hepp, s.hepp@gentics.com
 *
 * Implementation of EEPROM access.
 *
 * Copyright 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "Settings.h"

#include <avr/eeprom.h>

#include <common_config.h>

uint8_t EEMEM confChannel;
uint16_t EEMEM confCalibrationData[6];

Settings::Settings()
{
}

uint8_t Settings::getMIDIChannel()
{
    uint8_t v = eeprom_read_byte(&confChannel);
    return (v == 0xFF) ? MIDI_CHANNEL_TOESTUDS : v;
}

void Settings::setMIDIChannel(uint8_t channel)
{
    eeprom_write_byte(&confChannel, channel);
}

bool Settings::hasCalibrationData()
{
    uint16_t v = eeprom_read_word(&confCalibrationData[0]);
    return (v != 0xFFFF);
}

void Settings::getCalibrationData(uint8_t pedal, AICalibrationData &data) const
{
    data.min = (int) eeprom_read_word(&confCalibrationData[pedal * 2    ]);
    data.max = (int) eeprom_read_word(&confCalibrationData[pedal * 2 + 1]);
}

void Settings::setCalibrationData(uint8_t pedal, const AICalibrationData &data)
{
    eeprom_write_word(&confCalibrationData[pedal * 2    ], (uint16_t) data.min);
    eeprom_write_word(&confCalibrationData[pedal * 2 + 1], (uint16_t) data.max);
}
