/*
 * @project     MIDI Component
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-05 03:01:18 +0200 (Son, 05 Aug 2007) $
 * @version     $Id: adc.c 113 2007-08-05 01:01:18Z stefan $
 *
 * Midi driver implementation.
 *
 * TODO use timer (configured) to reset lastStatus from time to time.
 * TODO use avrlib uart driver, use config
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "midi.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrlib/common/common.h>

#include <config/common.h>
#include <config/midi_settings.h>

#if (!defined(MIDI_MAX_SKIP_STATUS) || (MIDI_MAX_SKIP_STATUS > 0)) && MIDI_TX_LINES > 0
#define __MIDI_HAS_LAST_STATUS
#if defined(MIDI_MAX_SKIP_STATUS)
#define __MIDI_HAS_STATUS_CNT
#endif
#endif

static uint8_t buffer[3];

#ifdef __MIDI_HAS_LAST_STATUS
static uint8_t lastStatus[MIDI_TX_CHANNELS];
#endif

#ifdef __MIDI_HAS_STATUS_CNT
static uint8_t lastStatusCnt[MIDI_TX_CHANNELS];
#endif

#if !defined(MIDI_RX_COMPLETE_CALLBACK_FUNC) && MIDI_RX_LINES > 0
static uint8_t (*midiRxCompleteFunc)(uint8_t line, MidiMsg_t *msg) = NULL;
#endif
#if !defined(MIDI_TX_SEND_CALLBACK_FUNC) && MIDI_TX_LINES > 0 
static uint8_t (*midiTxSendFunc)(uint8_t line, uint8_t *data, uint8_t size) = NULL;
#endif

#if MIDI_RX_LINES > 0
typedef struct {
    uint8_t lastStatus;
    uint8_t byte;
} MidiRxState_t;

static MidiRxState_t midiRxStates[MIDI_RX_LINES];
static MidiMsg_t midiRxMsgs[MIDI_RX_LINES];
#endif

void midiInit(void)
{
    midiReset();
}

void midiReset(void)
{
    uint8_t i;

#if MIDI_RX_LINES > 0
    for (i = 0; i < MIDI_RX_LINES; i++) {
	midiRxStates[i].lastStatus = 0;
	midiRxStates[i].byte = 0;
    }
#endif

#ifdef __MIDI_HAS_LAST_STATUS
    for (i = 0; i < MIDI_TX_LINES; i++) {
        lastStatus[i] = 0;
    }
#endif
#ifdef __MIDI_HAS_STATUS_CNT
    for (i = 0; i < MIDI_TX_LINES; i++) {
	lastStatusCnt[i] = 0;
    }
#endif
}

uint8_t midiSerializeMsg(uint8_t *dest, MidiMsg_t *msg, uint8_t startpos, uint8_t maxbytes)
{
    if ( maxbytes < 3 ) {
	return ERR_OUT_OF_MEM;
    }
    
    dest[startpos++] = msg->status;
    dest[startpos++] = msg->data1;
    dest[startpos]   = msg->data2;
}

uint8_t midiUnserializeMsg(uint8_t *src, MidiMsg_t *msg, uint8_t startpos, uint8_t maxbytes)
{
    if ( maxbytes < 3 ) {
	return ERR_OUT_OF_MEM;
    }

    msg->status = src[startpos++];
    msg->data1  = src[startpos++];
    msg->data2  = src[startpos];
}

#if MIDI_RX_LINES > 0
#ifndef MIDI_RX_COMPLETE_CALLBACK_FUNC
void midiSetRxCompleteFunc(uint8_t (*func)(uint8_t line, MidiMsg_t *msg))
{
    midiRxCompleteFunc = func;
}
#endif

static inline uint8_t midiRxComplete(uint8_t line, MidiMsg_t *msg)
{
#ifndef MIDI_RX_COMPLETE_CALLBACK_FUNC
    if ( midiRxCompleteFunc == NULL ) {
	return ERR_NO_CALLBACK;
    }
    return midiRxCompleteFunc(line, msg);
#elif MIDI_RX_COMPLETE_CALLBACK_FUNC != 0
    return MIDI_RX_COMPLETE_CALLBACK_FUNC(line, msg);
#else
    return ERR_NO_CALLBACK;
#endif
}

uint8_t midiReceive(uint8_t line, uint8_t byte)
{
    uint8_t c;

    if ( line > MIDI_RX_LINES ) {
	return ERR_RANGE;
    }

    c = midiRxStates[line].byte;
    if ( c == 0 ) {
	// check if status byte is omitted
	if ( byte & 0x80 ) {
	    midiRxMsgs[line].status = byte;
	    midiRxStates[line].byte = 1;  
	} else {
	    midiRxMsgs[line].data1 = byte;
	    midiRxStates[line].byte = 2;
	}
    } else if ( c == 1 ) {
	midiRxMsgs[line].data1 = byte;
	midiRxStates[line].byte = 2;
    } else if ( c == 2 ) {
	midiRxMsgs[line].data2 = byte;
	midiRxStates[line].byte = 0;

	return midiRxComplete(line, &midiRxMsgs[line]);
    }

    return ERR_OK;
}
#endif // MIDI_RX_LINES


#if MIDI_TX_LINES > 0
#ifndef MIDI_TX_SEND_CALLBACK_FUNC
void midiSetSendFunc(uint8_t (*func)(uint8_t line, uint8_t *data, uint8_t length))
{
    midiTxSendFunc = func;
}
#endif

static inline uint8_t midiTxSend(uint8_t line, uint8_t *data, uint8_t length) 
{
#ifdef MIDI_TX_SEND_CALLBACK_FUNC
    return MIDI_TX_SEND_CALLBACK_FUNC (line, data, length);
#else
    return midiTxSendFunc(line, data, length);
#endif
}

uint8_t midiSend(uint8_t line, MidiMsg_t *msg)
{
    uint8_t err, length, status;

#ifndef MIDI_TX_SEND_CALLBACK_FUNC
    if ( midiTxSendFunc == NULL ) {
	return ERR_NO_CALLBACK;
    }
#endif

    // send status byte
#if defined __MIDI_HAS_STATUS_CNT
    status = msg->status;
    if ( status != lastStatus[line] || lastStatusCnt[line] > MIDI_MAX_SKIP_STATUS ) {
	buffer[0] = status;
	length = 1;
	lastStatus[line] = status;
	lastStatusCnt[line] = 0;
    } else {
	lastStatusCnt[line]++;
    }
#elif defined __MIDI_HAS_LAST_STATUS
    if ( status != lastStatus[line] ) {
	buffer[0] = status;
	length = 1;
	lastStatus[line] = status;
    }
#else
    buffer[0] = status;
    length = 1;
#endif

    // send data
    // TODO support different msg lengths
    buffer[length++] = msg->data1;
    buffer[length++] = msg->data2;

    err = midiTxSend(line, &buffer, length);

    // need to clear last status to reset transfer if error has occured
#ifdef __MIDI_HAS_LAST_STATUS
    if ( err != ERR_OK ) {
	lastStatus[line] = 0;	
    }
#endif

    return err;
}
#endif // MIDI_TX_LINES

