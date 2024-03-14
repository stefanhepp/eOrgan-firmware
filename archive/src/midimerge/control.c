/*
 * @project     Midi Merger
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-05 03:00:41 +0200 (Son, 05 Aug 2007) $
 * @version     $Id: main.c 111 2007-08-05 01:00:41Z stefan $
 *
 * Implementation of the midi merger controller.
 *
 * Copyright 2008 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "control.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrlib/common/common.h>
#include <avrlib/spi/spi.h>
#include <avrlib/twi/twi.h>

#include <midi/midi/midi.h>
#include <midi/midi/midi_const.h>

#include "config/common.h"
#include "merger.h"
#include "filter.h"

/**
 * callback handler for SPI receive.
 **/
void controlSpiRecvData(uint8_t line, uint8_t byte)
{
    // TODO if midi send message is received, send a message using
    // mergerMidiSend(mode, &msg);
}

void controlInit(void)
{
    spiInit(SPI_LINE, SPI_BAUDRATE);
    spiSetMaster(SPI_LINE, 0);
    
    twiInit();
}

void controlStart(void)
{
    spiStart(SPI_LINE);
    twiStart();
}

void controlReportError(uint8_t error)
{

}

