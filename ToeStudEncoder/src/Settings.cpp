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

uint8_t EEMEM confChannel[3];
uint8_t EEMEM confMode;
uint8_t EEMEM confSensitivity;

uint16_t EEMEM confCalibrationData[6];

static const uint8_t ChannelDefault[3] = {MIDI_CHANNEL_TOESTUD, 
                                          MIDI_CHANNEL_TOESTUD_SWELL,
                                          MIDI_CHANNEL_TOESTUD_CHOIR};

Settings::Settings()
{
}

uint8_t Settings::getMIDIChannel(SettingsChannel channel)
{
    uint8_t v = eeprom_read_byte(&confChannel[channel]);
    return (v == 0xFF) ? ChannelDefault[channel] : v;
}

void Settings::setMIDIChannel(uint8_t channel, uint8_t channelSwell, uint8_t channelChoir)
{
    eeprom_write_byte(&confChannel[0], channel);
    eeprom_write_byte(&confChannel[1], channelSwell);
    eeprom_write_byte(&confChannel[2], channelChoir);
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
    data.center = 0;
}

void Settings::setCalibrationData(uint8_t pedal, const AICalibrationData &data)
{
    eeprom_write_word(&confCalibrationData[pedal * 2    ], (uint16_t) data.min);
    eeprom_write_word(&confCalibrationData[pedal * 2 + 1], (uint16_t) data.max);
}

uint8_t Settings::getSendMode(uint8_t defaultMode)
{
    uint8_t v = eeprom_read_byte(&confMode);
    return (v == 0xFF) ? defaultMode : v;
}

void Settings::setSendMode(uint8_t mode)
{
    eeprom_write_byte(&confMode, mode);
}

uint8_t Settings::getSensitivity()
{
    uint8_t v = eeprom_read_byte(&confSensitivity);
    return (v == 0xFF) ? 6 : v;
}

void Settings::setSensitivity(uint8_t sensitvity)
{
    eeprom_write_byte(&confSensitivity, sensitvity);
}
