/*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: lcd_hw.h 109 2007-08-04 21:22:53Z stefan $
 *
 * LCD Display low level hardware access functions.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __AVRLIB_LCD_HW_H__
#define __AVRLIB_LCD_HW_H__

#include <avr/io.h>


/* ==================================================== */
/* Constants                                            */
/* ==================================================== */

#define LCD_MODE_EMPTY          0
#define LCD_MODE_SEND           1
#define LCD_MODE_DELAY          2
#define LCD_MODE_WAIT		3


/* ==================================================== */
/* Functions                                            */
/* ==================================================== */

/** 
 * initialize the display hardware ports and timer.
 * 
 * @return an error code, or ERR_OK on success.
 **/
extern int8_t LcdHwInit(void);

/**
 * set the duty_cycle of the lcd display bg light.
 *
 * @param duty_cycle new duty cycle to set, between 0 and 100 for full brightness.
 **/
extern void LcdSetBgLight(uint8_t duty_cycle);

/**
 * set the duty_cycle of the the lcd display contrast.
 *
 * @param duty_cycle new duty cycle to set, between 0 and 100.
 **/
extern void LcdSetContrast(uint8_t duty_cycle);

/**
 * get the current mode of the send state machine.
 * @return one of LCD_MODE_*.
 **/
extern uint8_t LcdGetMode(void);

/**
 * write a command to the lcd display.
 *
 * This method must not be interrupted by another LcdWrite command, else 
 * the commands may be mixed up.
 *
 * @param byte a data byte or command to send.
 * @param rs the register select (RS) bit, 0 for commands, 1 for data.
 * @return an error code or ERR_OK on success.
 **/
extern int8_t LcdWriteCmd( uint8_t byte, uint8_t rs );

/**
 * insert a delay after the last cmd has been sent.
 * 
 * @param the delay in ~0.4ms.
 * @return an error code or ERR_OK on success.
 **/
extern int8_t LcdWriteDelay( uint8_t delay );

/**
 * add a wait cmd after the last cmd. 
 *
 * This checks the busy-wait of the display to wait until the busywait-flag is clear.
 *
 * @return an error code or ERR_OK on success.
 **/
extern int8_t LcdWriteWait(void);

#endif
