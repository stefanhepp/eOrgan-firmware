 /*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: lcd.c 109 2007-08-04 21:22:53Z stefan $
 *
 * Implementation of the SPI hardware driver.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "spi.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <config/spi_settings.h>

void spiHWInit(uint8_t baudrate)
{
}

void spiHWSetMaster(uint8_t master)
{
}

void spiHWSetRxFunc(void (*func)(uint8_t line, uint8_t byte))
{
}

void spiHWSetTxCompleteFunc(void (*func)(uint8_t line))
{
}

void spiHWSetErrorHandler(void (*func)(uint8_t line, uint8_t error))
{
}

void spiHWStart(void)
{
}

void spiHWStop(void)
{
}

uint8_t spiHWSend(uint8_t byte)
{
    return 0;
}

#if SPI_HW_TX_BUFSIZE > 0
uint8_t spiHWBufSend(uint8_t byte)
{
    return 0;
}
#endif

