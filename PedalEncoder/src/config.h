/*
 * @project     Midi Pedalboard
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Common custom IO and config settings.
 *
 * Copyright (c) 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __CONFIG_PEDAL_H__
#define __CONFIG_PEDAL_H__

#include <Arduino.h>

/* ==================================================== */
/* Common controller settings                           */
/* ==================================================== */

#define PORT_IN       B
#define PORT_LINE     D
#define PIN_LINE0     4

static const uint8_t PIN_CONFIG      = PIN_PC0;
static const uint8_t PIN_INTERRUPT   = PIN_PC3;
static const uint8_t PIN_LED         = PIN_PD3;

// midi number of lowest key on pedal (great C)
static const uint8_t LOWEST_NOTE    = 36;

#endif

