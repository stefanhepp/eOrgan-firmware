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
#include <avrlib/timer/timer.h>

#include <config/uart_settings.h>

#if FLAG_IS_SET(UART_SW_ENABLE)

void uartSWInit(int baudrate, uint8_t parity, uint8_t stopbits)
{

}

void uartSWSetRxFunc(void (*func)(uint8_t line, uint8_t byte))
{

}

void uartSWSetTxCompleteFunc(void (*func)(uint8_t line))
{

}

void uartSWSetErrorHandler(void (*func)(uint8_t line, uint8_t error))
{

}

uint8_t uartSWStart(void)
{

    return ERR_OK;
}

uint8_t uartSWStop(void)
{

    return ERR_OK;
}

uint8_t uartSWSend(uint8_t line, uint8_t byte)
{

    return ERR_OK;
}

#if UART_SW_TX_BUFSIZE > 0
uint8_t uartSWBufSend(uint8_t line, uint8_t byte)
{

    return ERR_OK;
}

uint8_t uartSWBufSendBlock(uint8_t line, uint8_t *data, uint8_t length)
{

    return ERR_OK;
}
#endif

#endif  // UART_SW_ENABLE

