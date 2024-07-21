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
uint8_t EEMEM confChannelSwell;
uint8_t EEMEM confChannelChoir;
uint8_t EEMEM confMode;

uint16_t EEMEM confCalibrationData[6];

Settings::Settings()
{
}

uint8_t Settings::getMIDIChannel()
{
    uint8_t v = eeprom_read_byte(&confChannel);
    return (v == 0xFF) ? MIDI_CHANNEL_TOESTUD : v;
}

uint8_t Settings::getMIDIChannelSwell()
{
    uint8_t v = eeprom_read_byte(&confChannel);
    return (v == 0xFF) ? MIDI_CHANNEL_TOESTUD_SWELL : v;
}

uint8_t Settings::getMIDIChannelChoir()
{
    uint8_t v = eeprom_read_byte(&confChannel);
    return (v == 0xFF) ? MIDI_CHANNEL_TOESTUD_CHOIR : v;
}

void Settings::setMIDIChannel(uint8_t channel, uint8_t channelSwell, uint8_t channelChoir)
{
    eeprom_write_byte(&confChannel, channel);
    eeprom_write_byte(&confChannelSwell, channelSwell);
    eeprom_write_byte(&confChannelChoir, channelChoir);
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

uint8_t Settings::getSendMode(uint8_t defaultMode)
{
    uint8_t v = eeprom_read_byte(&confChannel);
    return (v == 0xFF) ? defaultMode : v;
}

void Settings::setSendMode(uint8_t mode)
{
    eeprom_write_byte(&confMode, mode);
}
