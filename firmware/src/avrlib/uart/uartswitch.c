 /*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: lcd.c 109 2007-08-04 21:22:53Z stefan $
 *
 * Implementation of the UART driver.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "uart.h"

#include <avrlib/common/common.h>

#include <config/uart_settings.h>


#if FLAG_IS_SET(UART_HW_USE_GENERIC)

void uartHWInit(uint8_t line, int baudrate, uint8_t parity, uint8_t stopbits)
{
    switch (line) {
#if FLAG_IS_SET(UART_HW0_ENABLE)
	case 0:
	    uartHW0Init(baudrate, parity, stopbits);
	    break;
#endif
#if FLAG_IS_SET(UART_HW1_ENABLE)
	case 1:
	    uartHW1Init(baudrate, parity, stopbits);
	    break;
#endif
#if FLAG_IS_SET(UART_HW2_ENABLE)
	case 2:
	    uartHW2Init(baudrate, parity, stopbits);
	    break;
#endif
#if FLAG_IS_SET(UART_HW3_ENABLE)
	case 3:
	    uartHW3Init(baudrate, parity, stopbits);
	    break;
#endif
    }
}

void uartHWStart(uint8_t line)
{
    switch (line) {
#if FLAG_IS_SET(UART_HW0_ENABLE)
	case 0:
	    uartHW0Start();
	    break;
#endif
#if FLAG_IS_SET(UART_HW1_ENABLE)
	case 1:
	    uartHW1Start();
	    break;
#endif
#if FLAG_IS_SET(UART_HW2_ENABLE)
	case 2:
	    uartHW2Start();
	    break;
#endif
#if FLAG_IS_SET(UART_HW3_ENABLE)
	case 3:
	    uartHW3Start();
	    break;
#endif
    }
}

void uartHWStop(uint8_t line)
{
    switch (line) {
#if FLAG_IS_SET(UART_HW0_ENABLE)
	case 0:
	    uartHW0Stop();
	    break;
#endif
#if FLAG_IS_SET(UART_HW1_ENABLE)
	case 1:
	    uartHW1Stop();
	    break;
#endif
#if FLAG_IS_SET(UART_HW2_ENABLE)
	case 2:
	    uartHW2Stop();
	    break;
#endif
#if FLAG_IS_SET(UART_HW3_ENABLE)
	case 3:
	    uartHW3Stop();
	    break;
#endif
    }
}

uint8_t uartHWSend(uint8_t line, uint8_t byte)
{
    switch (line) {
#if FLAG_IS_SET(UART_HW0_ENABLE)
	case 0:
	    return uartHW0Send(byte);
#endif
#if FLAG_IS_SET(UART_HW1_ENABLE)
	case 1:
	    return uartHW1Send(byte);
#endif
#if FLAG_IS_SET(UART_HW2_ENABLE)
	case 2:
	    return uartHW2Send(byte);
#endif
#if FLAG_IS_SET(UART_HW3_ENABLE)
	case 3:
	    return uartHW3Send(byte);
#endif
    }
    return ERR_INVALID;
}

uint8_t uartHWBufSend(uint8_t line, uint8_t byte)
{
    switch (line) {
#if FLAG_IS_SET(UART_HW0_ENABLE) && UART_HW0_TX_BUFSIZE > 0
	case 0:
	    return uartHW0BufSend(byte);
#endif
#if FLAG_IS_SET(UART_HW1_ENABLE) && UART_HW1_TX_BUFSIZE > 0
	case 1:
	    return uartHW1BufSend(byte);
#endif
#if FLAG_IS_SET(UART_HW2_ENABLE) && UART_HW2_TX_BUFSIZE > 0
	case 2:
	    return uartHW2BufSend(byte);
#endif
#if FLAG_IS_SET(UART_HW3_ENABLE) && UART_HW3_TX_BUFSIZE > 0
	case 3:
	    return uartHW3BufSend(byte);
#endif
    }
    return ERR_INVALID;
}

uint8_t uartHWBufSendBlock(uint8_t line, uint8_t *data, uint8_t length)
{
    switch (line) {
#if FLAG_IS_SET(UART_HW0_ENABLE) && UART_HW0_TX_BUFSIZE > 0
	case 0:
	    return uartHW0BufSendBlock(data, length);
#endif
#if FLAG_IS_SET(UART_HW1_ENABLE) && UART_HW1_TX_BUFSIZE > 0
	case 1:
	    return uartHW1BufSendBlock(data, length);
#endif
#if FLAG_IS_SET(UART_HW2_ENABLE) && UART_HW2_TX_BUFSIZE > 0
	case 2:
	    return uartHW2BufSendBlock(data, length);
#endif
#if FLAG_IS_SET(UART_HW3_ENABLE) && UART_HW3_TX_BUFSIZE > 0
	case 3:
	    return uartHW3BufSendBlock(data, length);
#endif
    }
    return ERR_INVALID;
}

#endif // UART_HW_USE_GENERIC


#endif // UART_HW_ENABLE
