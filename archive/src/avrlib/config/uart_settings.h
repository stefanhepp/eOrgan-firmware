/*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: adc_settings.h 109 2007-08-04 21:22:53Z stefan $
 *
 * UART driver custom IO and config settings.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __CONFIG_UART_SETTINGS_H__
#define __CONFIG_UART_SETTINGS_H__

#include <config/common.h>

#include <avrlib/uart/uart_const.h>

/* ==================================================== */
/* Hardware UART settings                               */
/* ==================================================== */

// Use hardware UART
#define UART_HW_ENABLE		1

// set to 1 to create functions to switch between lines in runtime
#define UART_HW_USE_GENERIC	0

// Enable first HW UART
#define UART_HW0_ENABLE		1
// Set TX buffer size, zero to disable buffer
#define UART_HW0_TX_BUFSIZE	0
// Set rx-complete callback function name, disables compilation of setter
//#define UART_HW0_RX_CALLBACK_FUNC

#define UART_HW1_ENABLE		0
// Set TX buffer size, zero to disable buffer
#define UART_HW1_TX_BUFSIZE	0
// Set rx-complete callback function name, disables compilation of setter
//#define UART_HW1_RX_CALLBACK_FUNC

#define UART_HW2_ENABLE		0
// Set TX buffer size, zero to disable buffer
#define UART_HW2_TX_BUFSIZE	0
// Set rx-complete callback function name, disables compilation of setter
//#define UART_HW2_RX_CALLBACK_FUNC

#define UART_HW3_ENABLE		0
// Set TX buffer size, zero to disable buffer
#define UART_HW3_TX_BUFSIZE	0
// Set rx-complete callback function name, disables compilation of setter
//#define UART_HW3_RX_CALLBACK_FUNC

/* ==================================================== */
/* Software UART settings                               */
/* ==================================================== */

// Use software UART, compile code for it (set to 1 to enable)
#define UART_SW_ENABLE		0

// Maximum number of software UARTs (1..8)
#define UART_SW_RX_COUNT	2
#define UART_SW_TX_COUNT	1

// Set rx-complete callback function name, disables compilation of setter
//#define UART_SW_RX_CALLBACK_FUNC

// Number of the timer to use for SW uart
#define UART_SW_TIMER		0

// Ports and pins to use for SW uart
#define UART_SW_RX_PORT		B
#define UART_SW_RX_FIRST_PIN	0
#define UART_SW_TX_PORT		B
#define UART_SW_TX_FIRST_PIN	4

#endif

