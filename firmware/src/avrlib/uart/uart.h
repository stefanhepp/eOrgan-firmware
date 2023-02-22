/*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: lcd.h 109 2007-08-04 21:22:53Z stefan $
 *
 * UART driver functions, hardware and software implementations.
 * Hardware lines have numbers 0..3, software lines 8..15
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __AVRLIB_UART_UART_H__
#define __AVRLIB_UART_UART_H__

#include <avr/io.h>
#include <inttypes.h>

#include <avrlib/common/common.h>

#include <config/uart_settings.h>

/* ==================================================== */
/* Constants, Macros                                    */
/* ==================================================== */

#define UART_GET_HWLINE(idx)  (idx)
#define UART_GET_SWLINE(idx)  ((idx) | 8 )

#define UART_GET_LINEINDEX(line) ((line) & 8 )
#define UART_IS_SWLINE(line)     ((line) & 7 )

/* ==================================================== */
/* Types                                                */
/* ==================================================== */


/* ==================================================== */
/* Functions                                            */
/* ==================================================== */


#if FLAG_IS_SET(UART_HW_ENABLE)

void uartHWSetRxFunc(void (*func)(uint8_t line, uint8_t byte));

void uartHWSetTxCompleteFunc(void (*func)(uint8_t line));

void uartHWSetErrorHandler(void (*func)(uint8_t line, uint8_t error));


#if FLAG_IS_SET(UART_HW0_ENABLE) 

void uartHW0Init(int baudrate, uint8_t parity, uint8_t stopbits);

void uartHW0Start(void);

void uartHW0Stop(void);

uint8_t uartHW0Send(uint8_t byte);

#if UART_HW0_TX_BUFSIZE > 0
uint8_t uartHW0BufSend(uint8_t byte);

uint8_t uartHW0BufSendBlock(uint8_t *data, uint8_t length);
#endif

#endif // UART_HW0_ENABLE


#if FLAG_IS_SET(UART_HW1_ENABLE) 

void uartHW1Init(int baudrate, uint8_t parity, uint8_t stopbits);

void uartHW1Start(void);

void uartHW1Stop(void);

uint8_t uartHW1Send(uint8_t byte);

#if UART_HW1_TX_BUFSIZE > 0
uint8_t uartHW1BufSend(uint8_t byte);

uint8_t uartHW1BufSendBlock(uint8_t *data, uint8_t length);
#endif

#endif // UART_HW1_ENABLE


#if FLAG_IS_SET(UART_HW2_ENABLE) 

void uartHW2Init(int baudrate, uint8_t parity, uint8_t stopbits);

void uartHW2Start(void);

void uartHW2Stop(void);

uint8_t uartHW2Send(uint8_t byte);

#if UART_HW2_TX_BUFSIZE > 0
uint8_t uartHW2BufSend(uint8_t byte);

uint8_t uartHW2BufSendBlock(uint8_t *data, uint8_t length);
#endif

#endif // UART_HW2_ENABLE


#if FLAG_IS_SET(UART_HW3_ENABLE) 

void uartHW3Init(int baudrate, uint8_t parity, uint8_t stopbits);

void uartHW3Start(void);

void uartHW3Stop(void);

uint8_t uartHW3Send(uint8_t byte);

#if UART_HW3_TX_BUFSIZE > 0
uint8_t uartHW3BufSend(uint8_t byte);

uint8_t uartHW3BufSendBlock(uint8_t *data, uint8_t length);
#endif

#endif // UART_HW3_ENABLE


#if FLAG_IS_SET(UART_HW_USE_GENERIC)

// TODO make switch functions for both SW/HW too

void uartHWInit(uint8_t line, int baudrate, uint8_t parity, uint8_t stopbits);
void uartHWStart(uint8_t line);
void uartHWStop(uint8_t line);
uint8_t uartHWSend(uint8_t line, uint8_t byte);
uint8_t uartHWBufSend(uint8_t line, uint8_t byte);
uint8_t uartHWBufSendBlock(uint8_t line, uint8_t *data, uint8_t length);

#endif // UART_HW_USE_GENERIC

#endif  // UART_HW_ENABLE


#if FLAG_IS_SET(UART_SW_ENABLE)

/**
 * Initialize the software UART.
 **/
void uartSWInit(int baudrate, uint8_t parity, uint8_t stopbits);

void uartSWSetRxFunc(void (*func)(uint8_t line, uint8_t byte));

void uartSWSetTxCompleteFunc(void (*func)(uint8_t line));

void uartSWSetErrorHandler(void (*func)(uint8_t line, uint8_t error));

uint8_t uartSWStart(void);

uint8_t uartSWStop(void);

uint8_t uartSWSend(uint8_t line, uint8_t byte);

#if UART_SW_TX_BUFSIZE > 0
uint8_t uartSWBufSend(uint8_t line, uint8_t byte);

uint8_t uartSWBufSendBlock(uint8_t line, uint8_t *data, uint8_t length);
#endif

#endif  // UART_SW_ENABLE 


#endif
