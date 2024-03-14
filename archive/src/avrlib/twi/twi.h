/*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: lcd.h 109 2007-08-04 21:22:53Z stefan $
 *
 * TWI driver functions.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __AVRLIB_TWI_TWI_H__
#define __AVRLIB_TWI_TWI_H__

#include <avr/io.h>

#include <config/twi_settings.h>

/* ==================================================== */
/* Constants, Macros                                    */
/* ==================================================== */


/* ==================================================== */
/* Functions                                            */
/* ==================================================== */


void twiInit(void);

void twiStart(void);

void twiStop(void);



#endif 
