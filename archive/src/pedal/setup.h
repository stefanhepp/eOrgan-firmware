/*
 * @project     Midi Pedalboard
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:21:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: input.h 108 2007-08-04 21:21:53Z stefan $
 *
 * Store config values in EEPROM.
 * Channel and intensity are values from 0 to 15.
 *
 * Copyright 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __SETUP_H__
#define __SETUP_H__

#include <inttypes.h>

uint8_t confReadChannel(void);

uint8_t confReadIntensity(void);

void confWriteChannel(uint8_t channel);

void confWriteIntensity(uint8_t intensity);

#endif
