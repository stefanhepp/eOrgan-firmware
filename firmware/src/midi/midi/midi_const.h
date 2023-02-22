/*
 * @project     MIDI Components
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-05 03:01:18 +0200 (Son, 05 Aug 2007) $
 * @version     $Id: adc.h 113 2007-08-05 01:01:18Z stefan $
 *
 * Midi message byte macros and other constants
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __MIDI_MIDI_MIDI_CONST_H__
#define __MIDI_MIDI_MIDI_CONST_H__

/* ==================================================== */
/* Constants                                            */
/* ==================================================== */

// Standard Midi baud rate, in baud/s
#define MIDI_BAUD_RATE  31250


/////////////////// Midi Status messages //////////////////////////
// For messages with channel, the first channel is always '0'.

/**
 * Midi note on status message. 
 * Uses 2 bytes: note nr and note velocity. Default velocity is 64.
 * If velocity is 0, this is interpreted as note-off. Use this if possible to
 * speed up transmission.
 **/
#define MIDI_NOTEON(channel) (0x90 | (channel & 0x0F))

/**
 * Midi note off status message.
 * Uses 2 bytes: note nr and note velocity. Default velocity is 64.
 **/
#define MIDI_NOTEOFF(channel) (0x80 | (channel & 0x0F))

/**
 * Midi Controller message.
 * Uses two bytes: first byte is controller nr, second byte depends on controller.
 **/
#define MIDI_CONTROL(channel) (0xB0 | (channel & 0x0F))

////////////////// Midi Controller Numbers ////////////////////////
// Status Message must be MIDI_CONTROL(), second byte depends on controller.

/**
 * Midi All sound off controller message nr.
 * Turns off all sound played with note-on, regardless of hold pedal state.
 * Second byte is not used.
 **/
#define MIDI_CTL_ALL_SOUND_OFF	    120

/**
 * Midi All Notes Off controller message nr.
 * Turns off all notes played with note-on. Notes aren't actually released until 
 * Hold Pedal is turned off.
 * Second byte is not used.
 **/
#define MIDI_CTL_ALL_NOTES_OFF	    123

#define MIDI_CTL_OMNI_OFF	    124

#define MIDI_CTL_OMNI_ON	    125


#endif
