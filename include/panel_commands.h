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

// Request:  COUPLER <state>
// Response: none
//    state:0: Coupler enabled
//    state:1: Coupler MIDI out enabled
static const uint8_t PANEL_CMD_COUPLER = 0x02;

// Request:  ROUTER <output>
// Response: none
//    output:0: MIDI USB out enabled
//    output:1: MIDI Out enabled
static const uint8_t PANEL_CMD_ROUTER  = 0x03; 

