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

static const uint8_t PIN_INTERRUPT   = PIN_PC3;

static const uint8_t PIN_VOLUME = PC0;
static const uint8_t PIN_FX1 = PC2;
static const uint8_t PIN_FX2 = PC1;

static const uint8_t PIN_LED1        = PIN_PB1;
static const uint8_t PIN_LED2        = PIN_PB2;
static const uint8_t PIN_LED3        = PIN_PB3;
static const uint8_t PIN_LED4        = PIN_PB4;
static const uint8_t PIN_LED5        = PIN_PB5;
static const uint8_t PIN_LED6        = PIN_PB6;
static const uint8_t PIN_LED_CTRL    = PIN_PB0;

static const uint8_t PIN_KP_BTN      = PIN_PB7;
static const uint8_t PIN_KP_L1       = PIN_PD2;
static const uint8_t PIN_KP_L2       = PIN_PD1;
static const uint8_t PIN_KP_L3       = PIN_PD0;
static const uint8_t PIN_S0          = PIN_PD3;
static const uint8_t PIN_S1          = PIN_PD4;
static const uint8_t PIN_S2          = PIN_PD5;
static const uint8_t PIN_BTN1        = PIN_PD6;
static const uint8_t PIN_BTN2        = PIN_PD7;
