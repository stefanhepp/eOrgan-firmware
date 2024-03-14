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
 * set to 0 to disable send capabilities in code.
 **/
#define MIDI_TX_LINES	2

/**
 * set to 0 to disable receive capabilities in code.
 **/
#define MIDI_RX_LINES	5

/**
 * Name of the function to call when a message has been received.
 **/
#define MIDI_RX_COMPLETE_CALLBACK_FUNC mergerMidiReceive

/**
 * Name of the function to call when a message byte has to be sent.
 **/
#define MIDI_TX_SEND_CALLBACK_FUNC mergerMidiSendData

/**
 * Maximum number of status bytes to skip if no status change occurs.
 * Infinite, if not defined.
 **/
#define MIDI_MAX_SKIP_STATUS	10

#endif

