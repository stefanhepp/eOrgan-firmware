 /*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: lcd.c 109 2007-08-04 21:22:53Z stefan $
 *
 * Implementation of buffer functions.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "buffer.h"

#include <stdlib.h>

uint8_t queueInit(Queue_t *q, uint8_t size)
{
    if ( q->buf ) {
	free(q->buf);
    }

    q->length = 0;
    q->end = 0;
    q->size = 0;

    q->buf = malloc(size);
    if ( !q->buf ) {
	return ERR_OUT_OF_MEM;
    }

    q->size = size;

    return ERR_OK;
}

uint8_t queueClear(Queue_t *q)
{
    q->length = 0;
}

uint8_t queueWrite(Queue_t *q, uint8_t byte)
{
    uint8_t start;

    if ( q->length >= q->size ) {
	return ERR_BUF_FULL;
    }

    start = (q->end + q->length);
    // wrap around, overflow if start < end
    if ( start >= q->size || start <= q->end ) {
	start -= q->size;
    }

    q->buf[start] = byte;
    
    q->length++;

    return ERR_OK;
}

uint8_t queueRead(Queue_t *q, uint8_t *byte)
{
    if ( q->length == 0 ) {
	return ERR_BUF_EMPTY;
    }

    *byte = q->buf[q->end];

    q->end++;
    // wrap around
    if ( q->end >= q->size ) {
	q->end = 0;
    }

    return ERR_OK;
}

uint8_t queueWriteBlock(Queue_t *q, void *data, uint8_t size)
{
    uint8_t i, start;

    if ( queueMemAvail(q) < size ) {
	return ERR_BUF_FULL;
    }

    start = q->end + q->length;
    // wrap around, overflow if start < q->end
    if ( start >= q->size || start < q->end ) {
	start -= q->size;
    }

    // TODO split into two loops at q->size, remove tests in loop ?
    for ( i = 0; i < size; i++ ) {
	q->buf[start++] = *(data++);
	if ( start >= q->size ) {
	    start = 0;
	}
    }

    q->length += size;

    return ERR_OK;
}

uint8_t queueReadBlock(Queue_t *q, void *data, uint8_t size)
{
    uint8_t i, end;

    if ( q->length < size ) {
	return ERR_BUF_EMPTY;
    }

    end = q->end;
    for ( i = 0; i < size; i++ ) {
	*(data++) = q->buf[end++];
	if ( end >= q->size ) {
	    end = 0;
	}
    }

    q->end = end;

    return ERR_OK;
}

uint8_t queueMemAvail(Queue_t *q)
{
    return q->size - q->length;
}

uint8_t queueMemUsed(Queue_t *q)
{
    return q->length;
}

uint8_t queueIsEmpty(Queue_t *q)
{
    return q->length == 0;
}

uint8_t queueIsFull(Queue_t *q)
{
    return q->length >= q->size;
}

