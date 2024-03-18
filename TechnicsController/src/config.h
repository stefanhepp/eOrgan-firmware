/*
 * @project     Midi Pedalboard
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:21:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: common.h 108 2007-08-04 21:21:53Z stefan $
 *
 * common custom IO and config settings.
 *
 * Copyright (c) 2007 Stefan Hepp
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

static const uint8_t PIN_INTERRUPT   = PIN_PD3;

static const uint8_t PIN_BENDER      = PIN_PC0;

static const uint8_t PIN_READ        = PIN_PB7;
static const uint8_t PIN_ENABLE      = PIN_PD5;
static const uint8_t PIN_LATCH       = PIN_PD6;
static const uint8_t PIN_CLOCK       = PIN_PD7;

// midi number of lowest key on pedal (great C)
static const uint8_t LOWEST_NOTE    = 36;

#endif

