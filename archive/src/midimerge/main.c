/*
 * @project     Midi Merger
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-05 03:00:41 +0200 (Son, 05 Aug 2007) $
 * @version     $Id: main.c 111 2007-08-05 01:00:41Z stefan $
 *
 * This is the main program for the Midi Merger firmware.
 *
 * Copyright 2008 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include <avr/io.h>

#include "config/common.h"
#include "merger.h"
#include "control.h"
#include "filter.h"

int main(void)
{
    filterInit();
    controlInit();
    mergerInit();

    sei();

    controlStart();
    mergerStart();

    MCUCR |= (1<<SE);

    while (1) {
	asm("sleep");
	// main loop will not continue until interrupts are completed
	while (mergerHasData()) {
	    mergerProcessData();
	}
    }

    return 0;
}
