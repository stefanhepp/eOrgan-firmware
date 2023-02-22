/*
 * @project     Midi Pedalboard
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:21:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: input.h 108 2007-08-04 21:21:53Z stefan $
 *
 * Functions to merge and buffer midi events.
 *
 * Copyright 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __MERGER_H__
#define __MERGER_H__

#include <inttypes.h>

#include <midi/midi/midi.h>

#define MERGER_SEND_MIDIOUT  1
#define MERGER_SEND_MIDIINT  2
#define MERGER_SEND_BOTH     3

void mergerInit(void);

void mergerStart(void);

/**
 * Function to send a midi message. 
 * @param mode is one of the MERGER_SEND_* macros.
 * @return an error code.
 **/
uint8_t mergerMidiSend(uint8_t mode, MidiMsg_t *msg) ;

uint8_t mergerHasData(void);

void mergerProcessData(void);

#endif
