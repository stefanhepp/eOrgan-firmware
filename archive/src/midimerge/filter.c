/*
 * @project     Midi Merger
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-05 03:00:41 +0200 (Son, 05 Aug 2007) $
 * @version     $Id: main.c 111 2007-08-05 01:00:41Z stefan $
 *
 * Implementation of the midi filter.
 *
 * Copyright 2008 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "filter.h"

#include <avrlib/common/common.h>

#include <midi/midi/midi.h>
#include <midi/midi/midi_const.h>

#include "config/common.h"
#include "control.h"
#include "merger.h"

void filterInit(void)
{

}

void filterReset()
{

}

void filterSetupLine(uint8_t line, uint8_t flags)
{

}

void filterMidiProcess(uint8_t line, MidiMsg_t *msg)
{
    msg->status = MIDI_SET_CHANNEL(msg->status, line);

    mergerMidiSend(MERGER_SEND_BOTH, msg);
}

