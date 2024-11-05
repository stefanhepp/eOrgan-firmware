/**
 * Author: Stefan Hepp (stefan@stefant.org)
 *
 * Panel UART protocol opcodes.
 **/
#pragma once

#include <inttypes.h>

/* ==============================================================
 * Panel UART Commands and Constants
 * ============================================================== */

static const uint32_t PANEL_UART_SPEED = 115200;

// Request:  STATUS
// Response: STATUS <status1> <status2>
//    status1:0: MIDI USB out enabled
//    status1:1: MIDI out enabled
//    status2:0: Coupler enabled
//    status2:1: Coupler MIDI output enabled
static const uint8_t PANEL_CMD_STATUS  = 0x01;

// Request:  COUPLER <mode>
// Response: none
//    mode: 0: Disabled, 1: MIDI output, 2: Coupler enabled
static const uint8_t PANEL_CMD_COUPLER = 0x02;

static const uint8_t PANEL_COUPLER_DISABLED = 0;
static const uint8_t PANEL_COUPLER_MIDI = 1;
static const uint8_t PANEL_COUPLER_ENABLED = 2;

// Request:  ROUTER <output>
// Response: none
//    output:0: MIDI USB out enabled
//    output:1: MIDI Out enabled
static const uint8_t PANEL_CMD_ROUTER  = 0x03; 

// Request:  VOLUME <channel> <volumeHigh> <volumeLow>
// Response: none
//    channel:    0: master volume, 1: USB input, 2: FX input
//    volumeHigh: high byte of volume
//    volumeLow:  low byte of volume
static const uint8_t PANEL_CMD_VOLUME  = 0x04;

static const uint8_t PANEL_CHANNEL_MASTER = 0;
static const uint8_t PANEL_CHANNEL_USB = 1;
static const uint8_t PANEL_CHANNEL_FX = 2;

// Request:  PAGE_TURN <direction>
// Response: none
//    direction: 0: next, 1: back
static const uint8_t PANEL_CMD_PAGE_TURN = 0x05;

static const uint8_t PANEL_PAGE_NEXT = 0;
static const uint8_t PANEL_PAGE_PREV = 1;
