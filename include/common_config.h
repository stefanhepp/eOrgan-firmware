/**
 * Author: Stefan Hepp (stefan@stefant.org)
 *
 * Common settings for all organ controllers.
 **/
#pragma once

#include <inttypes.h>

/* ==============================================================
 * Common Types
 * ============================================================== */

/**
 * Organ Divisions. Enum value represents default MIDI channel.
 */
enum MIDIDivision : uint8_t 
{
    MD_Pedal   = 3,
    MD_Choir   = 4,
    MD_Great   = 1,
    MD_Swell   = 5,
    MD_Solo    = 6,
    MD_Control = 7
};

/**
 * MIDI Controller. Enum values are indexes, in same index range of MIDIPort.
 */
enum Controller
{
    MC_Technics    = 0,
    MC_Keyboard    = 1,
    MC_Pedal       = 2
};

enum MIDIPort
{
    MP_MIDI1       = 3,
    MP_MIDI2       = 4,
    MP_MIDI3       = 5,
    MP_MIDI4       = 6,
    MP_MIDI_USB    = 7,
    MP_MIDI_RTP    = 8,
    MP_MIDI_Teensy = 9
};

/* ==============================================================
 * I2C addresses and commands
 * ============================================================== */

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

/*
 * MIDI channels and messages
 */

// Default MIDI channels for controllers
static const uint8_t MIDI_CHANNEL_PEDAL         = MIDIDivision::MD_Pedal;
static const uint8_t MIDI_CHANNEL_TECHNICS      = MIDIDivision::MD_Choir;
static const uint8_t MIDI_CHANNEL_KEYBOARD_1    = MIDIDivision::MD_Swell;
static const uint8_t MIDI_CHANNEL_KEYBOARD_2    = MIDIDivision::MD_Solo;
static const uint8_t MIDI_CHANNEL_TOESTUD       = MIDIDivision::MD_Control;
static const uint8_t MIDI_CHANNEL_TOESTUD_SWELL = MIDIDivision::MD_Swell;
static const uint8_t MIDI_CHANNEL_TOESTUD_CHOIR = MIDIDivision::MD_Choir;

// Crescendo pedal CC
static const uint8_t MIDI_CONTROL_CRESCENDO =  11; // ExpressionPedal

// Velocity for digital keys
static const uint8_t KEY_VELOCITY = 64;
