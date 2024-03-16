/**
 * Author: Stefan Hepp (stefan@stefant.org)
 *
 * A library of various helpers for AVR programming
 **/

#ifndef __AVRLIB_H__
#define __AVRLIB_H__

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


#endif
