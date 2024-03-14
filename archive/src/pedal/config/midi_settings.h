/*
 * @project     Midi Components
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: adc_settings.h 109 2007-08-04 21:22:53Z stefan $
 *
 * Midi driver custom IO and config settings.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __CONFIG_MIDI_SETTINGS_H__
#define __CONFIG_MIDI_SETTINGS_H__

#include <config/common.h>

/* ==================================================== */
/* Constants, IO settings                               */
/* ==================================================== */

/**
 * set to 1 to enable send capabilities in code.
 **/
#define MIDI_USE_TX	1

/**
 * set to 1 to enable receive capabilities in code.
 **/
#define MIDI_USE_RX	0

#define MIDI_MAX_SKIP_STATUS    10

#endif

