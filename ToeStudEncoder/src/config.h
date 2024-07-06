/*
 * @project     ToeStudController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * common custom IO and config settings.
 *
 * Copyright (c) 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __CONFIG_TOESTUD_H__
#define __CONFIG_TOESTUD_H__

#include <Arduino.h>

/* ==================================================== */
/* Common controller settings                           */
/* ==================================================== */

#define PORT_IN       B

static const uint8_t PIN_INTERRUPT   = PIN_PD3;

static const uint8_t PIN_S0        = PIN_PD2;
static const uint8_t PIN_S1        = PIN_PD3;
static const uint8_t PIN_S2        = PIN_PD4;
static const uint8_t PIN_EN1       = PIN_PD5;
static const uint8_t PIN_EN2       = PIN_PD6;

// midi number of lowest key on pedal (great C)
static const uint8_t LOWEST_NOTE    = 36;

#endif

