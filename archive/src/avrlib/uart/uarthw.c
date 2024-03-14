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

#include <avr/interrupt.h>

#include <avrlib/common/common.h>
#include <avrlib/common/buffer.h>

#include <config/uart_settings.h>

#if FLAG_IS_SET(UART_HW_ENABLE)

void uartHWSetRxFunc(void (*func)(uint8_t line, uint8_t byte))
{
}

void uartHWSetTxCompleteFunc(void (*func)(uint8_t line))
{
}

void uartHWSetErrorHandler(void (*func)(uint8_t line, uint8_t error))
{
}


#if FLAG_IS_SET(UART_HW0_ENABLE)

void uartHW0Init(int baudrate, uint8_t parity, uint8_t stopbits)
{
}

void uartHW0Start(void)
{
}

void uartHW0Stop(void)
{
}

uint8_t uartHW0Send(uint8_t byte)
{
    return ERR_OK;
}

#if UART_HW0_TX_BUFSIZE > 0
uint8_t uartHW0BufSend(uint8_t byte)
{
    return ERR_OK;
}

uint8_t uartHW0BufSendBlock(uint8_t *data, uint8_t length)
{
    return ERR_OK;
}
#endif

#endif // UART_HW0_ENABLE


#if FLAG_IS_SET(UART_HW1_ENABLE)

void uartHW1Init(int baudrate, uint8_t parity, uint8_t stopbits)
{
}

void uartHW1Start(void)
{
}

void uartHW1Stop(void)
{
}

uint8_t uartHW1Send(uint8_t byte)
{
    return ERR_OK;
}

#if UART_HW1_TX_BUFSIZE > 0
uint8_t uartHW1BufSend(uint8_t byte)
{
    return ERR_OK;
}

uint8_t uartHW1BufSendBlock(uint8_t *data, uint8_t length)
{
    return ERR_OK;
}
#endif

#endif // UART_HW1_ENABLE


#if FLAG_IS_SET(UART_HW2_ENABLE)

void uartHW2Init(int baudrate, uint8_t parity, uint8_t stopbits)
{
}

void uartHW2Start(void)
{
}

void uartHW2Stop(void)
{
}

uint8_t uartHW2Send(uint8_t byte)
{
    return ERR_OK;
}

#if UART_HW2_TX_BUFSIZE > 0
uint8_t uartHW2BufSend(uint8_t byte)
{
    return ERR_OK;
}

uint8_t uartHW2BufSendBlock(uint8_t *data, uint8_t length)
{
    return ERR_OK;
}
#endif

#endif // UART_HW2_ENABLE


#if FLAG_IS_SET(UART_HW3_ENABLE)

void uartHW3Init(int baudrate, uint8_t parity, uint8_t stopbits)
{
}

void uartHW3Start(void)
{
}

void uartHW3Stop(void)
{
}

uint8_t uartHW3Send(uint8_t byte)
{
    return ERR_OK;
}

#if UART_HW3_TX_BUFSIZE > 0
uint8_t uartHW3BufSend(uint8_t byte)
{
    return ERR_OK;
}

uint8_t uartHW3BufSendBlock(uint8_t *data, uint8_t length)
{
    return ERR_OK;
}
#endif

#endif // UART_HW3_ENABLE


#endif // UART_HW_ENABLE
