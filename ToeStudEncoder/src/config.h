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
#pragma once

#include <Arduino.h>

/* ==================================================== */
/* Common controller settings                           */
/* ==================================================== */

#define PORT_IN       B

static const uint8_t PIN_INTERRUPT   = PIN_PC3;

static const uint8_t PIN_L1        = PIN_PB0;
static const uint8_t PIN_L2        = PIN_PB1;
static const uint8_t PIN_L3        = PIN_PB2;
static const uint8_t PIN_L4        = PIN_PB3;
static const uint8_t PIN_D1        = PIN_PD3;
static const uint8_t PIN_D2        = PIN_PD4;
static const uint8_t PIN_D3        = PIN_PD5;
static const uint8_t PIN_D4        = PIN_PD6;
static const uint8_t PIN_D5        = PIN_PD7;

static const uint8_t PIN_PEDAL_CHOIR = PC1;
static const uint8_t PIN_PEDAL_SWELL = PC2;
static const uint8_t PIN_CRESCENDO = PC0;

