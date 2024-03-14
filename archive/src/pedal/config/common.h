/*
 * @project     Midi Pedalboard
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:21:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: common.h 108 2007-08-04 21:21:53Z stefan $
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
#define XTAL		    4000000
#define F_CPU		    XTAL

#define PORT_IN		    B
#define PORT_LINE	    D
#define PORT_BTN	    D
#define PORT_LED	    D

// pin of lowest line. other four lines must be the following pins
#define PIN_LINE0	    2
#define PIN_BTN		    6
#define PIN_LED		    0

// midi number of lowest key on pedal (great C)
#define LOWEST_NOTE	    36

#endif

