/*
 * @project     Midi Pedalboard
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * common custom IO and config settings.
 *
 * Copyright (c) 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __CONFIG_PISTON_H__
#define __CONFIG_PISTON_H__

#include <Arduino.h>

/* ==================================================== */
/* Common controller settings                           */
/* ==================================================== */

#define PORT_IN       B

static const uint8_t PIN_INTERRUPT   = PIN_PC3;

static const uint8_t PIN_A1        = PIN_PC2;
static const uint8_t PIN_A2        = PIN_PC1;
static const uint8_t PIN_A3        = PIN_PC0;

#endif

