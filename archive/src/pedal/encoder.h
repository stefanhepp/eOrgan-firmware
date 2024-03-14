/*
 * @project     Midi Pedalboard
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:21:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: input.h 108 2007-08-04 21:21:53Z stefan $
 *
 * Input encoding and processing routines.
 *
 * Copyright 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __ENCODER_H__
#define __ENCODER_H__

/**
 * Initialize all input ports and routines.
 **/
void encInit(void);

/** 
 * poll input ports for changes, do everything it takes to process
 * data. Also sends notes if something has changed.
 **/
void encPoll(void);

#endif
