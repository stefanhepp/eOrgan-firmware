/*
 * @project     MIDI Components
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-05 03:01:18 +0200 (Son, 05 Aug 2007) $
 * @version     $Id: adc.h 113 2007-08-05 01:01:18Z stefan $
 *
 * Multiline midi input and output driver library.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __MIDI_MIDI_MIDI_H__
#define __MIDI_MIDI_MIDI_H__

#include <config/midi_settings.h>

/* ==================================================== */
/* Constants, Macros, Types                             */
/* ==================================================== */

typedef struct {
    uint8_t status,
    uint8_t data1,
    uint8_t data2
} MidiMsg_t;


#define MIDI_GET_CHANNEL(status)	    (status & 0x0F)

#define MIDI_SET_CHANNEL(status,channel)    ((status & 0xF0)|(channel & 0x0F))


/* ==================================================== */
/* Functions                                            */
/* ==================================================== */

/**
 * Initialze the midi driver.
 **/
void midiInit(void);

/**
 * Reset the Tx/Rx states on all channels.
 **/
void midiReset(void);

/**
 * Serialize a midi message (including additional data blocks) to a byte array.
 *
 * @param startpos index of first byte to write in bytearray.
 * @param maxbytes number of bytes to write at most (bytearray must have at least startpos+maxbytes bytes).
 * @return error code if not enough memory is available to copy data.
 **/
uint8_t midiSerializeMsg(uint8_t *dest, MidiMsg_t *msg, uint8_t startpos, uint8_t maxbytes);

uint8_t midiUnserializeMsg(uint8_t *src, MidiMsg_t *msg, uint8_t startpos, uint8_t maxbytes);

#if MIDI_RX_LINES > 0 

void midiSetRxCompleteFunc(void (*func)(uint8_t line, MidiMsg_t *msg));

uint8_t midiReceive(uint8_t line, uint8_t byte);

#endif

#if MIDI_TX_LINES > 0

void midiSetSendFunc(uint8_t (*func)(uint8_t line, uint8_t *data, uint8_t length));

/**
 * Send a midi message.
 * The status byte must be one of the MIDI_* status macros, the data bytes range is 0 to 127.
 **/
uint8_t midiSend(uint8_t line, MidiMsg_t *msg);

#endif

#endif
