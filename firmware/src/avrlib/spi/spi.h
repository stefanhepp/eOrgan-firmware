/*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: lcd.h 109 2007-08-04 21:22:53Z stefan $
 *
 * SPI driver functions, hardware and software implementations.
 * Hardware is line 0, software is line 1.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __AVRLIB_SPI_SPI_H__
#define __AVRLIB_SPI_SPI_H__

#include <avr/io.h>

#include <config/spi_settings.h>

/* ==================================================== */
/* Constants, Macros                                    */
/* ==================================================== */

#define SPI_HW	    0
#define SPI_SW	    1


/* ==================================================== */
/* Functions                                            */
/* ==================================================== */

#if FLAG_IS_SET(SPI_USE_GENERIC)

void spiInit(uint8_t line, int baudrate);

void spiSetMaster(uint8_t line, uint8_t master);

void spiSetRxFunc(uint8_t line, void (*func)(uint8_t line, uint8_t byte));

void spiSetTxCompleteFunc(uint8_t line, void (*func)(uint8_t line));

void spiSetErrorHandler(uint8_t line, void (*func)(uint8_t line, uint8_t error));

void spiStart(uint8_t line);

void spiStop(uint8_t line);

uint8_t spiSend(uint8_t line, uint8_t byte);

uint8_t spiBufSend(uint8_t line, uint8_t byte);

#endif // SPI_USE_GENERIC


#if FLAG_IS_SET(SPI_HW_ENABLE)

void spiHWInit(int baudrate);

void spiHWSetMaster(uint8_t master);

void spiHWSetRxFunc(void (*func)(uint8_t line, uint8_t byte));

void spiHWSetTxCompleteFunc(void (*func)(uint8_t line));

void spiHWSetErrorHandler(void (*func)(uint8_t line, uint8_t error));

void spiHWStart(void);

void spiHWStop(void);

uint8_t spiHWSend(uint8_t byte);

#if SPI_HW_TX_BUFSIZE > 0
uint8_t spiHWBufSend(uint8_t byte);
#endif

#endif  // SPI_HW_ENABLE


#if FLAG_IS_SET(SPI_SW_ENABLE)

/**
 * Initialize the software SPI.
 **/
void spiSWInit(int baudrate);

void spiSWSetMaster(uint8_t master);

void spiSWSetRxFunc(void (*func)(uint8_t line, uint8_t byte));

void spiSWSetTxCompleteFunc(void (*func)(uint8_t line));

void spiSWSetErrorHandler(void (*func)(uint8_t line, uint8_t error));

uint8_t spiSWStart(void);

uint8_t spiSWStop(void);

uint8_t spiSWSend(uint8_t byte);

#if UART_SW_TX_BUFSIZE > 0
uint8_t spiSWBufSend(uint8_t byte);
#endif

#endif  // SPI_SW_ENABLE 


#endif
