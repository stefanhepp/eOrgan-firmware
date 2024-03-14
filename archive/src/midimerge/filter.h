/*
 * @project     Midi Pedalboard
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:21:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: input.h 108 2007-08-04 21:21:53Z stefan $
 *
 * Functions to control midi filter and to handle control IO.
 *
 * Copyright 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __FILTER_H__
#define __FILTER_H__

#include <inttypes.h>

#include <midi/midi/midi.h>

#define FILTER_LINE_ENABLED	0x01
#define FILTER_LINE_NOTESONLY	0x02

void filterInit(void);

/**
 * Reset the setup to the default configuration.
 **/
void filterReset();

void filterSetupLine(uint8_t line, uint8_t flags);

// TODO map lines/channels to channels, transpose, coupler


/**
 * Filter and modify a midi message using the current setup.
 * Send the message with mergerMidiSend().
 **/
void filterMidiProcess(uint8_t line, MidiMsg_t *msg);

#endif
