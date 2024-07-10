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
static const uint8_t I2C_ADDR_PISTON_KEYBOARD  = 0x5A;
static const uint8_t I2C_ADDR_PISTON_TECHNICS  = 0x5B;

static const uint8_t I2C_CMD_RESET          = 0x01;
static const uint8_t I2C_CMD_SET_CHANNEL    = 0x02;
static const uint8_t I2C_CMD_CALIBRATE      = 0x03;
static const uint8_t I2C_CMD_STOP_CALIBRATE = 0x04;
static const uint8_t I2C_CMD_LED_INTENSITY  = 0x05;
static const uint8_t I2C_CMD_SET_LEDS       = 0x06;
static const uint8_t I2C_CMD_SET_MODE       = 0x07;

// Default MIDI channels
static const uint8_t MIDI_CHANNEL_KEYBOARD_1 = 5;  // Swell
static const uint8_t MIDI_CHANNEL_KEYBOARD_2 = 6;  // Solo
static const uint8_t MIDI_CHANNEL_TECHNICS   = 4;  // Choir
static const uint8_t MIDI_CHANNEL_PEDAL	     = 3;  // Pedal
static const uint8_t MIDI_CHANNEL_TOESTUDS   = 7; 

// Crescendo pedal CC
static const uint8_t MIDI_CONTROL_CRESCENDO =  11; // ExpressionPedal

// Velocity for digital keys
static const uint8_t KEY_VELOCITY = 64;

#endif