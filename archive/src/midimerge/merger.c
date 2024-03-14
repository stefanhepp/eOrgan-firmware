/*
 * @project     Midi Merger
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-05 03:00:41 +0200 (Son, 05 Aug 2007) $
 * @version     $Id: main.c 111 2007-08-05 01:00:41Z stefan $
 *
 * Implementation of the midi merger filter and buffer.
 *
 * Copyright 2008 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "merger.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrlib/common/common.h>
#include <avrlib/uart/uart.h>

#include <midi/midi/midi.h>
#include <midi/midi/midi_const.h>

#include "config/common.h"
#include "control.h"

#define MERGER_TXLINE_INT 0
#define MERGER_TXLINE_OUT 1

#define MERGER_CMD_RXDATA 0xE0
#define MERGER_CMD_TXMSG  0xF0

static Queue_t rxQueue;

/**
 * callback function for midi driver to send midi events
 **/
uint8_t mergerMidiSendData(uint8_t line, uint8_t *data, uint8_t length)
{
    if ( line == MERGER_TXLINE_INT ) {
	return uartHWBufSendBlock(UART_HW, data, length);
    } else if ( line == MERGER_TXLINE_OUT ) {
	return uartSWBufSendBlock(0, data, length);
    }
    return ERR_INVALID;
}

/**
 * Function to process and filter received midi events.
 * Callback for midi-receive-complete
 **/
void mergerMidiReceive(uint8_t line, MidiMsg_t *msg)
{
    filterMidiProcess(line, msg);
}

/**
 * callback function for uart receive data.
 * Store data into queue for further processing in main-loop.
 **/
void mergerUARTReceive(uint8_t line, uint8_t byte)
{
    uint8_t tmpMsg[2];

    // mark higher nibble as consistency check
    tmpMsg[0] = UART_GET_LINEINDEX(line) | MERGER_CMD_RXDATA;
    tmpMsg[1] = byte;

    intLock();

    queueWriteBlock(&rxQueue, &tmpMsg, 2);

    intUnlock();
}

/**
 * callback function for midi uart send complete.
 **/
void mergerUARTSendComplete(uint8_t line)
{
    // currently not needed.
}

/**
 * callback for UART error handler.
 **/
void mergerUARTErrorHandler(uint8_t line, uint8_t error)
{
    controlReportError(error);
}

void mergerInit(void)
{
    queueInit(&rxQueue, MIDI_RX_BUFSIZE);

    midiInit();

    uartHWInit(HW_UART, INT_MIDI_BAUDRATE);
    uartSWInit(MIDI_BAUDRATE);
}

void mergerStart(void)
{
    uartHWStart(HW_UART);
    uartSWStart();
}

void mergerMidiSend(uint8_t mode, MidiMsg_t *msg)
{
    // TODO get correct buffer size from midi-library
    uint8_t tmpMsg[4];

    tmpMsg[0] = mode | MERGER_CMD_TXMSG;
    midiSerializeMsg(tmpMsg, msg, 1, 3);
    
    intLock();

    queueWriteBlock(&rxQueue, &tmpMsg, 4);

    intUnlock();
}

uint8_t mergerHasData(void)
{
    return queueMemUsed(&rxQueue);
}

void mergerProcessData(void)
{
    uint8_t tmpMsg[3];
    uint8_t err;
    uint8_t line, cmd, data;
    MidiMsg_t midiMsg;

    // dequeue all data from rx buffer
    while (1) {

	intLock();

	err = queueRead(&rxQueue, &line);
	if ( err == ERR_OK ) {
	    cmd = line & 0xF0;

	    if ( cmd == MERGER_CMD_RXDATA ) {

		err = queueRead(&rxQueue, &data);

	    } else if ( cmd == MERGER_CMD_TXMSG ) {

		err = queueReadBlock(&rxQueue, &tmpMsg, 3);

	    } else {
		// invalid cmd, should never happen. Skip this byte.
		intUnlock();		
		controlReportError(ERR_INVALID);
		continue;
	    }
	}

	intUnlock();

	// send data to midi receiver, will be processed in callback function
	// as soon as a midi message is complete.
	if ( err != ERR_OK ) {
	    break;
	}

	// process queued event
	line &= 0x0F;

	if ( cmd == MERGER_CMD_RXDATA ) {

	    // send received bytes to midi RX engine.
	    midiReceive(line, data);

	} else if ( cmd == MERGER_CMD_TXMSG ) {

	    // send queued processed midi events to midi TX engine.
	    midiUnserializeMsg(&tmpMsg, &midiMsg, 0, 3);

	    if ( line & MERGER_SEND_MIDIINT ) {
		midiSend(MERGER_TXLINE_INT, &midiMsg);
	    }
	    if ( line & MERGER_SEND_MIDIOUT ) {
		midiSend(MERGER_TXLINE_OUT, &midiMsg);
	    }
	}
    }

}

