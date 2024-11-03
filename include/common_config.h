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
 * 
 * Note: MIDI channel is 1..16
 */
enum MIDIDivision : uint8_t
{
    MD_MIDI    = 0,  // No division, general MIDI input
    MD_Great   = 2,  // external keyboard

    MD_Pedal   = 3,
    MD_Choir   = 4,
    MD_Swell   = 5,
    MD_Solo    = 6,
    MD_Control = 7
};

// Highest number of Division channels. Sparse index!
static const int MAX_DIVISION_CHANNEL = 7;

/**
 * I2C Controller. Enum value represents I2C address.
 */
enum Controller : uint8_t
{
    MC_Keyboard        = 0x50,
    MC_Technics        = 0x51,
    MC_ToeStud         = 0x52,
    MC_Pedal           = 0x53,
    MC_Panel           = 0x54,
    MC_LEDController   = 0x55,
    MC_Piston_Keyboard = 0x5A,
    MC_Piston_Technics = 0x5B,
    MC_MainPanel       = 0x60
};

enum MIDIPort : uint8_t
{
    // Ports for input and output, general devices. Output ports must be first in this enum.
    MP_MIDI1       = 0,
    MP_MIDI2       = 1,
    MP_MIDI3       = 2,
    MP_MIDI4       = 3,
    MP_MIDI_USB    = 4,
    MP_MIDI_RTP    = 5,
    MP_MIDI_Teensy = 6,
    // MIDI ports with fixed controller assignment,s input only
    MP_Technics    = 7,
    MP_Keyboard    = 8,
    MP_Pedal       = 9
};

// Number of MIDI output ports.
static const int NUM_MIDI_OUTPUT_PORTS = 7;

// Number of both Controller and MIDI ports
static const int NUM_MIDI_PORTS = 10;

static const int NUM_MIDI_CHANNELS = 16;

/* ==============================================================
 * Various Constants
 * ============================================================== */

// MIDI note for lowest note on organ divisions: C2 (great C)
static const uint8_t MIDI_NOTE_C2 = 36;

// Maximum number of pistons in a division
static const uint8_t MAX_PISTONS = 26;

// Mode flags for ToeStud Encoder. Used as a bitmask.
enum ToeStudMode : uint8_t
{
    TSM_MIDI = 0x01,
    TSM_I2C  = 0x02
};

/* ==============================================================
 * I2C Commands
 * ============================================================== */

// I2C Addresses
static const uint8_t I2C_CMD_RESET           = 0x01;
static const uint8_t I2C_CMD_SET_CHANNEL     = 0x02;
static const uint8_t I2C_CMD_CALIBRATE       = 0x03;
static const uint8_t I2C_CMD_STOP_CALIBRATE  = 0x04;
static const uint8_t I2C_CMD_LED_INTENSITY   = 0x05;
static const uint8_t I2C_CMD_SET_LEDS        = 0x06;
static const uint8_t I2C_CMD_SET_MODE        = 0x07;
static const uint8_t I2C_CMD_SET_SENSITIVITY = 0x08;

/* ==============================================================
 * MIDI Channels and Constants
 * ============================================================== */

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

/* ==============================================================
 * Panel UART Commands and Constants
 * ============================================================== */

static const int PANEL_UART_SPEED = 115200;

static const uint8_t PANEL_CMD_PING = 0x01;

