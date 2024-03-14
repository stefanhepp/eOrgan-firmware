/*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: adc_settings.h 109 2007-08-04 21:22:53Z stefan $
 *
 * SPI driver custom IO and config settings.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __CONFIG_SPI_SETTINGS_H__
#define __CONFIG_SPI_SETTINGS_H__

/* ==================================================== */
/* Hardware UART settings                               */
/* ==================================================== */

// Use hardware UART
#define SPI_HW_ENABLE		1

// set to 1 to create functions to switch between sw/hw in runtime
#define SPI_USE_GENERIC	0

// Set TX buffer size, zero to disable buffer
#define SPI_HW_TX_BUFSIZE	0
// Set rx-complete callback function name, disables compilation of setter
//#define SPI_HW_RX_CALLBACK_FUNC

/* ==================================================== */
/* Software UART settings                               */
/* ==================================================== */

// Use software UART, compile code for it (set to 1 to enable)
#define SPI_SW_ENABLE		0

// Set rx-complete callback function name, disables compilation of setter
//#define SPI_SW_RX_CALLBACK_FUNC

// Number of the timer to use for SW uart
#define SPI_SW_TIMER		1

// Ports and pins to use for SW uart
#define SPI_SW_PORT		A
#define SPI_SW_MISO_PIN		0
#define SPI_SW_MOSI_PIN		1
#define SPI_SW_SCK_PIN		2
#define SPI_SW_SS_PIN		3

#endif

