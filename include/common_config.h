/**
 * Author: Stefan Hepp (stefan@stefant.org)
 *
 * Common settings for all organ controllers.
 **/
#ifndef __Common_H__
#define __Common_H__

#include <inttypes.h>

// I2C Addresses
static const uint8_t I2C_ADDR_KEYBOARD = 0x50;
static const uint8_t I2C_ADDR_TECHNICS = 0x51;
static const uint8_t I2C_ADDR_TOESTUD  = 0x52;
static const uint8_t I2C_ADDR_PEDAL    = 0x53;
static const uint8_t I2C_ADDR_PANEL    = 0x54;
static const uint8_t I2C_ADDR_PISTON1  = 0x5A;
static const uint8_t I2C_ADDR_PISTON2  = 0x5B;

static const uint8_t I2C_CMD_RESET          = 0x01;
static const uint8_t I2C_CMD_SET_CHANNEL    = 0x02;
static const uint8_t I2C_CMD_CALIBRATE      = 0x03;
static const uint8_t I2C_CMD_STOP_CALIBRATE = 0x04;
static const uint8_t I2C_CMD_LED_INTENSITY  = 0x05;

// Default MIDI channels
static const uint8_t MIDI_CHANNEL_KEYBOARD_1 = 6;
static const uint8_t MIDI_CHANNEL_KEYBOARD_2 = 5;
static const uint8_t MIDI_CHANNEL_TECHNICS   = 4;
static const uint8_t MIDI_CHANNEL_PEDAL	     = 3;

// Velocity for digital keys
static const uint8_t KEY_VELOCITY = 64;

#endif