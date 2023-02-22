/*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2008-09-22 14:33:44 +0200 (Mon, 22 Sep 2008) $
 * @version     $Id: common.h 195 2008-09-22 12:33:44Z stefan $
 *
 * some common macros and defines.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __AVRLIB_COMMON_COMMON_H__
#define __AVRLIB_COMMON_COMMON_H__

#include <avr/io.h>
#include <inttypes.h>

/* ==================================================== */
/* Constants                                            */
/* ==================================================== */

// Constants for some common errors 
#define ERR_OK		0
#define ERR_BUF_FULL	1
#define ERR_BUF_EMPTY	2
#define ERR_INVALID	3
#define ERR_RANGE	4
#define ERR_NO_CALLBACK	5
#define ERR_OUT_OF_MEM	6

/* ==================================================== */
/* Macros                                               */
/* ==================================================== */

// macro to test if a macro is enabled (tests if set and equals 1)
#define FLAG_IS_SET(x)  (x == 1)

// define some macros for easier port access. IO_CONCAT_INT is needed for macro argument expansion.
#ifndef IO_CONCAT_INT
#define IO_CONCAT_INT(A,B) A ## B
#endif
#ifndef IO_CONCAT
#define IO_CONCAT(A,B) IO_CONCAT_INT(A,B)
#endif
// access a given port data direction (0 for in, 1 for out)
#ifndef IO_DDR
#define IO_DDR(A) IO_CONCAT_INT(DDR,A)
#endif
// Port control (0 for low/tristate, 1 for high/pullup)
#ifndef IO_PORT
#define IO_PORT(A) IO_CONCAT_INT(PORT,A)
#endif
// Port pin data readout
#ifndef IO_PIN
#define IO_PIN(A) IO_CONCAT_INT(PIN,A)
#endif


// Interrupt definition macros .. INTERRUPT is mysteriously gone from avr-libc, so reintroduce it here
#if defined(__GNUC__) && (__GNUC__ > 3)
#define __ISR_ATTRS used, externally_visible
#else
#define __ISR_ATTRS used
#endif

#ifdef __cplusplus

#define ISR_NOBLOCK(vector)                                  \
extern "C" void vector(void) __attribute__ ((interrupt,__INTR_ATTRS));     \
void vector (void)
#define ISR_BLOCK(vector)                                  \
extern "C" void vector(void) __attribute__ ((signal,__INTR_ATTRS));     \
void vector (void)
#define ISR_NAKED(vector)                                  \
extern "C" void vector(void) __attribute__ ((signal,naked,__INTR_ATTRS));     \
void vector (void)

#else

#define ISR_NOBLOCK(vector)                                  \
void vector (void) __attribute__ ((interrupt,__INTR_ATTRS));               \
void vector (void)
#define ISR_BLOCK(vector)                                  \
void vector (void) __attribute__ ((signal,__INTR_ATTRS));               \
void vector (void)
#define ISR_NAKED(vector)                                  \
void vector (void) __attribute__ ((signal,naked,__INTR_ATTRS));               \
void vector (void)

#endif


/* ==================================================== */
/* Types                                                */
/* ==================================================== */

typedef struct {
    uint8_t *buf,
    uint8_t bufsize,
    uint8_t start,
    uint8_t end
} ringbuffer_t;

/* ==================================================== */
/* Functions                                            */
/* ==================================================== */

/**
 * Function to clear interrupts with counter.
 **/
void intLock(void);

/**
 * Function to enable interrupts if all locks are released.
 * @return number of remaining locks, 0 if interrupts are enabled.
 **/
uint8_t intUnlock(void);


#endif

