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

#define PORT_BTN B
#define PORT_LED D

// Index of pin in Port B
static const uint8_t BTN1        = 0;
static const uint8_t BTN2        = 3;
static const uint8_t BTN3        = 2;
static const uint8_t BTN4        = 1;
static const uint8_t BTN5        = 4;

static const uint8_t PIN_INTERRUPT   = 0;
static const uint8_t PIN_LED1_R      = 3;
static const uint8_t PIN_LED1_G      = 2;
static const uint8_t PIN_LED1_B      = 1;
static const uint8_t PIN_LED2_R      = 7;
static const uint8_t PIN_LED2_G      = 6;
static const uint8_t PIN_LED2_B      = 5;
static const uint8_t PIN_LED3        = 4;


#endif

