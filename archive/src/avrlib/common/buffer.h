/*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2008-09-20 02:55:43 +0200 (Sam, 20 Sep 2008) $
 * @version     $Id: common.h 188 2008-09-20 00:55:43Z stefan $
 *
 * some common macros and defines.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __AVRLIB_COMMON_BUFFER_H__
#define __AVRLIB_COMMON_BUFFER_H__

#include <avr/io.h>
#include <inttypes.h>

/* ==================================================== */
/* Types                                                */
/* ==================================================== */

/**
 * Handle for a queue buffer.
 * The structure is subject to changes, do not use fields directly.
 **/
typedef struct {
    uint8_t *buf,
    uint8_t size,
    uint8_t length,
    uint8_t end
} Queue_t;

/* ==================================================== */
/* Functions                                            */
/* ==================================================== */

/**
 * Function to initialize a ringbuffer.
 **/
uint8_t queueInit(Queue_t *q, uint8_t size);

uint8_t queueClear(Queue_t *q);

uint8_t queueWrite(Queue_t *q, uint8_t byte);

uint8_t queueRead(Queue_t *q, uint8_t *byte);

uint8_t queueWriteBlock(Queue_t *q, void *data, uint8_t size);

uint8_t queueReadBlock(Queue_t *q, void *data, uint8_t size);

uint8_t queueMemAvail(Queue_t *q);

uint8_t queueMemUsed(Queue_t *q);

uint8_t queueIsEmpty(Queue_t *q);

uint8_t queueIsFull(Queue_t *q);

#endif

