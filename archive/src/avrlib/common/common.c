 /*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: lcd.c 109 2007-08-04 21:22:53Z stefan $
 *
 * Implementation of common functions.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "common.h"

static volatile uint8_t lock_cnt = 0;

void intLock(void)
{
    cli();
    lock_cnt++;
}

uint8_t intUnlock(void)
{
    if ( lock_cnt > 0 )  {
	lock_cnt--;
    }
    if ( lock_cnt == 0 ) {
	sei();
    }
    return lock_cnt;
}

