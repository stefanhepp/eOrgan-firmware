/*
 * @project     TechnicsController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Common custom IO and config settings.
 *
 * Copyright (c) 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#include <Arduino.h>

/* ==================================================== */
/* Common controller settings                           */
/* ==================================================== */

#define PORT_IN       B

static const uint8_t PIN_INTERRUPT   = PIN_PD3;

static const uint8_t PIN_BENDER      = PIN_PC0;

static const uint8_t PIN_READ        = PIN_PB7;
static const uint8_t PIN_ENABLE      = PIN_PD5;
static const uint8_t PIN_LATCH       = PIN_PD6;
static const uint8_t PIN_CLOCK       = PIN_PD7;

// midi number of lowest key on pedal (great C)
static const uint8_t LOWEST_NOTE    = 36;

