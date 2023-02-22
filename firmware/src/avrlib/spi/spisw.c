 /*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: lcd.c 109 2007-08-04 21:22:53Z stefan $
 *
 * Implementation of the SPI software driver.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "spi.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <config/spi_settings.h>

void spiSWInit(uint8_t baudrate)
{
}

void spiSWSetMaster(uint8_t master)
{
}

void spiSWSetRxFunc(void (*func)(uint8_t line, uint8_t byte))
{
}

void spiSWSetTxCompleteFunc(void (*func)(uint8_t line))
{
}

void spiSWSetErrorHandler(void (*func)(uint8_t line, uint8_t error))
{
}

void spiSWStart(void)
{
}

void spiSWStop(void)
{
}

uint8_t spiSWSend(uint8_t byte)
{
    return 0;
}

#if SPI_SW_TX_BUFSIZE > 0
uint8_t spiSWBufSend(uint8_t byte)
{
    return 0;
}
#endif

