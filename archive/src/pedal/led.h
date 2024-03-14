/*
 * @project     Midi Pedalboard
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:21:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: input.h 108 2007-08-04 21:21:53Z stefan $
 *
 * LED intensity and mode interface.
 *
 * Copyright 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __LED_H__
#define __LED_H__

#include <inttypes.h>

#define LED_MODE_NORMAL 0
#define LED_MODE_CONFIG 1

/**
 * Initialize all input ports and routines.
 **/
void ledInit(void);

void ledReset(void);

void ledSetMode(uint8_t mode);

void ledDecIntensity(void);

void ledIncIntensity(void);

#endif
