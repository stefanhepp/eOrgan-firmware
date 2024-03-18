/**
 * Author: Stefan Hepp (stefan@stefant.org)
 *
 * Common settings for all organ controllers.
 **/
#ifndef __Common_H__
#define __Common_H__

#include <inttypes.h>

// I2C Addresses
static const uint8_t I2C_ADDR_KEYBOARD = 10;
static const uint8_t I2C_ADDR_TECHNICS = 11;
static const uint8_t I2C_ADDR_PISTON   = 12;
static const uint8_t I2C_ADDR_TOESTUD  = 13;
static const uint8_t I2C_ADDR_PEDAL	   = 14;
static const uint8_t I2C_ADDR_PANEL	   = 15;

// Default MIDI channels
static const uint8_t MIDI_CHANNEL_KEYBOARD_1 = 6;
static const uint8_t MIDI_CHANNEL_KEYBOARD_2 = 5;
static const uint8_t MIDI_CHANNEL_TECHNICS   = 4;
static const uint8_t MIDI_CHANNEL_PEDAL	     = 3;

// Velocity for digital keys
static const uint8_t KEY_VELOCITY = 64;

#endif