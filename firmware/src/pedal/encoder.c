/*
 * @project     UsbRc
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-05 03:00:41 +0200 (Son, 05 Aug 2007) $
 * @version     $Id: input.c 111 2007-08-05 01:00:41Z stefan $
 *
 * Input processing routines.
 *
 * Copyright 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "encoder.h"

#include "config/common.h"

#include <avr/io.h>

#include <avrlib/common/common.h>

#include <midi/midi/midi_const.h>
#include <midi/midi/midi.h>

#include "setup.h"
#include "led.h"

// lower nibble current active line (bit 1,0), state of input btn (bit 2)
static uint8_t state;

static uint8_t channel;

// current state buffer of pedals
static char pedals[4];

void encInit(void)
{
    uint8_t i;

    // init with line 0
    state = 0x00;

    channel = confReadChannel();

    // init ports (in port without pullups, out ports, all disabled, btn in with pullup)
    IO_DDR(PORT_IN) = 0x00;
    IO_PORT(PORT_IN) = 0x00;

    IO_DDR(PORT_LINE) |= (0x0F << PIN_LINE0);
    IO_PORT(PORT_LINE) |= (0x0F << PIN_LINE0);

    // Enable Btn input (ext. pulldown)
    IO_DDR(PORT_BTN) &= ~(1<<PIN_BTN);
    IO_PORT(PORT_BTN) &= ~(1<<PIN_BTN);

    // enable line0
    IO_PORT(PORT_LINE) &= ~(1<<PIN_LINE0);

    for ( i = 0; i < 4; i++ ) {
	pedals[i] = 0x00;
    }

}

static inline char getNote( char pedal ) 
{
    return pedal + LOWEST_NOTE;
}

/**
 * send notes off for all pressed notes.
 * @param force if non-zero, send a note-off controller msg on the current channel.
 **/
static void sendAllNotesOff(uint8_t force)
{
    uint8_t i,j = 0;

    if ( force ) {
        for (i = 0; i < 32; i++) {
	    if ( (i & 0x07) == 0 ) {
		j = pedals[i>>3];
	    } else {
		j >>= 1;
	    }
	    if ( j & 0x01 ) {
		midiSend(MIDI_NOTEOFF(channel), getNote(i), 64);
	    }
	}
    } else {
	midiSend(MIDI_CONTROL(channel), MIDI_CTL_ALL_SOUND_OFF, 0);
    }
}

/**
 * start config mode.
 **/
static void startConfig(void)
{
    sendAllNotesOff(0);

    ledSetMode(LED_MODE_CONFIG);
}

/**
 * cleanup after config button is released.
 **/
static void finalizeConfig(void)
{
    uint8_t i;    

    for (i = 0; i < 4; i++) {
	pedals[i] = 0x00;
    }

    ledSetMode(LED_MODE_NORMAL);
}

static void encReset(void)
{
    // disable all notes on current channel
    sendAllNotesOff(1);

    // Reset channel
    channel = 0;
    confWriteChannel(channel);
}

/**
 * process configuration input.
 **/
static void processConfigInput(char input, uint8_t line)
{
    uint8_t i;
    char bit = 1;

    // lower 16 keys set channel
    if ( line < 2 ) {
	for (i = 0; i < 8; i++) {
	    if ( (input & bit) && !(pedals[line] & bit) ) {

		// set new channel
		channel = ((line<<3)|i) & 0x0F;
		confWriteChannel(channel);

		break;
	    }

	    bit <<= 1;
	}

    // highest notes for config
    } else if (line == 3) {

	if ( (input & (1<<4)) && !(pedals[line] & (1<<4)) ) {
	    encReset();
	    ledReset();
	} else if ( (input & (1<<0)) && !(pedals[line] & (1<<0)) ) {
	    ledDecIntensity();
	} else if ( (input & (1<<2)) && !(pedals[line] & (1<<2)) ) {
	    ledIncIntensity();
	}

    } else {
	// TODO transpose
    }
    
}

static void processPedalInput(char input, uint8_t line)
{
    uint8_t i;
    char bit = 1;

    for (i = 0; i < 8; i++) {
	if ( (input & bit) && !(pedals[line] & bit) ) {
	    midiSend(MIDI_NOTEON(channel), getNote( (line<<3) | i ), 64);
	} else if ( !(input & bit) && (pedals[line] & bit) ) {
	    // this is a note off event (velocity is 0)
	    midiSend(MIDI_NOTEON(channel), getNote( (line<<3) | i ), 0);
	}

	bit <<= 1;
    }
}

void encPoll(void)
{
    uint8_t line = state & 0x03;
    uint8_t newline = (line + 1) % 4;

    // read status from current line
    char in = IO_PIN(PORT_IN);

    // enable next line 
    IO_PORT(PORT_LINE) |= (1<<(line + PIN_LINE0));
    IO_PORT(PORT_LINE) &= ~(1<<(newline + PIN_LINE0));

    // check config-btn state
    if ( IO_PIN(PORT_BTN) & (1<<PIN_BTN) ) {
	// btn is pressed
	if ( ! (state & 0x04) ) {
	    // btn is now pressed and was not pressed
	    startConfig();
	    state |= (1<<2);
	}

	processConfigInput(in, line);

    } else {
	if ( state & 0x04 ) {
	    // btn was pressed and is now released
	    finalizeConfig();
	    state &= ~(1<<2);
	}

	processPedalInput(in, line);
    }

    // update pedal states
    pedals[line] = in;

    state &= 0xFC;
    state |= newline;
}

