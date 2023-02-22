/*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: common.h 109 2007-08-04 21:22:53Z stefan $
 *
 * common custom IO and config settings.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __CONFIG_COMMON_H__
#define __CONFIG_COMMON_H__

/* ==================================================== */
/* Common controller settings                           */
/* ==================================================== */


/**
 * Define crystal clock speed.
 **/
#define XTAL		    20000000


// hardware UART line of internal midi connection
#define HW_UART		    0
#define INT_MIDI_BAUDRATE   500000

#define SPI_BAUDRATE	    500000
#define SPI_LINE	    SPI_HW

// buffer is 128 bytes=32 msgs
#define MIDI_RX_BUFSIZE		128

#define MIDI_TXINT_BUFSIZE	32
#define MIDI_TX_BUFSIZE		64


#endif

