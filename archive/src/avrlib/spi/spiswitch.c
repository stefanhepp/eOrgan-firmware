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

#if FLAG_IS_SET(SPI_USE_GENERIC)

void spiInit(uint8_t line, int baudrate)
{

}

void spiSetMaster(uint8_t line, uint8_t master)
{

}

void spiSetRxFunc(uint8_t line, void (*func)(uint8_t line, uint8_t byte))
{

}

void spiSetTxCompleteFunc(uint8_t line, void (*func)(uint8_t line))
{

}

void spiSetErrorHandler(uint8_t line, void (*func)(uint8_t line, uint8_t error))
{

}

void spiStart(uint8_t line)
{

}

void spiStop(uint8_t line)
{

}

uint8_t spiSend(uint8_t line, uint8_t byte)
{

    return ERR_INVALID;
}

uint8_t spiBufSend(uint8_t line, uint8_t byte)
{

    return ERR_INVALID;
}

#endif // SPI_USE_GENERIC

