/*
 * @project     Midi Pedalboard
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-05 03:00:41 +0200 (Son, 05 Aug 2007) $
 * @version     $Id: main.c 111 2007-08-05 01:00:41Z stefan $
 *
 * This is the main program for the Midi Pedalboard firmware.
 *
 * Copyright 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#include <midi/midi/midi.h>

#include "encoder.h"
#include "led.h"

int main(void)
{
    midiInit();
    ledInit();
    encInit();
    sei();

    MCUCR |= (1<<SE);

    while (1) {
	encPoll();
	// Sleep is needed here to prevent timer events to be missed.
	asm("sleep");
    }

    return 0;
}
