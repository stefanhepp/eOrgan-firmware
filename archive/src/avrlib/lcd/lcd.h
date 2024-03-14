/*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: lcd.h 109 2007-08-04 21:22:53Z stefan $
 *
 * LCD Display driver and methods to set the display bglight.
 *
 * It is assumed that interrupts are enabled when the Lcd* methods are called.
 * The methods must not interrupt each other.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __AVRLIB_LCD_H__
#define __AVRLIB_LCD_H__

#include <avr/io.h>

/* ==================================================== */
/* Constants                                            */
/* ==================================================== */

/**
 * flags for disp functions.
 **/
#define LCD_PAD_ZERO		0x01
#define LCD_JUSTIFY_RIGHT	0x02


/* ==================================================== */
/* Types                                                */
/* ==================================================== */

/**
 * a structure for custom characters.
 * 
 * Each line contains one character line. The lower 6 bytes
 * define the pixels in the line, the higher 2 bytes enable
 * blinking if blinking is enabled.
 **/
typedef struct {
	uint8_t lines[8];
} LcdMyChar;

/* ==================================================== */
/* Functions                                            */
/* ==================================================== */

/** 
 * initialize the display.
 * @
 **/
extern int8_t LcdInit(void);

/**
 * clear everything on the display.
 **/
extern int8_t LcdClear(void);

/**
 * set display mode.
 *
 * @param mode bitmask with LCF_DISP_ON, LCF_CURSOR_ON, LCF_BLINK_ON.
 * @return an error code, or ERR_OK on success.
 **/
extern int8_t LcdSetDispMode( uint8_t mode );

/**
 * set the display scroll mode.
 * Enable scrolling per line. Each bit corresponds to a line, starts with LSB for the 
 * first line. If a bit is set, scrolling is enabled.
 * 
 * @param line lines to enable scrolling.
 * @return an error code, or ERR_OK on success.
 **/ 
extern int8_t LcdSetScroll( uint8_t lines );

/**
 * enable the extended function mode (RE bit).
 *
 * This method takes care of all flags to be set correctly.
 * 
 * @param enable 1 if extended mode should be enabled, else extended mode is disabled.
 * @return an error code or ERR_OK on success.
 **/
extern int8_t LcdEnableExtMode( uint8_t enable );

/**
 * store a user defined character at a given address.
 *
 * @param c the character to store.
 * @param address a valid address for the character.
 **/
extern int8_t LcdSetMyChar( LcdMyChar *c, uint8_t address );

/**
 * delay until all pending changes are written to the display.
 *
 * Interrupts must be enabled while this method is called.
 * 
 * @return an error code or ERR_OK on success.
 **/
extern int8_t LcdSync(void);

/**
 * set the ddram cursor pos.
 * All further data writes will go to ddram.
 *
 * @param row the row of the cursor to set.
 * @param col the col of the cursor to set.
 * @return an error code or ERR_OK on success.
 **/
extern int8_t LcdSetCursor( uint8_t row, uint8_t col );

/**
 * display a 16 byte unsigned int, pad with spaces or zero.
 *
 * @param i the number to display.
 * @param row the row of the number.
 * @param start the column of the leftmost char, or the rightmost if PAD_RIGHT is given.
 * @param dispMode a bitmask of LCD_PAD_ZERO and LCD_JUSTIFY_RIGHT.
 * @param digits the number of digits to show, or 0 to display as many digits as needed.
 * @return an error code or ERR_OK if successful.
 **/
extern int8_t LcdDispU16F( uint16_t i, uint8_t row, uint8_t start, uint8_t dispMode, uint8_t digits );

/**
 * Display a signed 8bit integer.
 *
 * @param i the number to display.
 * @param row the row of the number.
 * @param start the column of the leftmost char.
 * @return an error code or ERR_OK if successful.
 **/
extern int8_t LcdDispSInt( int8_t i, uint8_t row, uint8_t start );

/**
 * Display an unsigned 8bit integer.
 *
 * @param i the number to display.
 * @param row the row of the number.
 * @param start the column of the leftmost char.
 * @return an error code or ERR_OK if successful.
 **/
extern int8_t LcdDispUInt( uint8_t i, uint8_t row, uint8_t start );

/**
 * Display an unsigned 8bit number as hex value (without 0x-prefix).
 *
 * @param i the number to display.
 * @param row the row of the number.
 * @param start the column of the leftmost char.
 * @return an error code or ERR_OK if successful.
 **/
extern int8_t LcdDispHex( uint8_t i, uint8_t row, uint8_t start );


/**
 * Display a character at a given position.
 *
 * @param c the code of the character to display.
 * @param row the row of the char.
 * @param start the column of the char.
 * @return an error code or ERR_OK if successful.
 **/
extern int8_t LcdDispChar( uint8_t c, uint8_t row, uint8_t start );

/**
 * Display null-terminated string at a given position.
 *
 * @param s the null-terminated string to display.
 * @param row the row of the string.
 * @param start the column of the leftmost char.
 * @return an error code or ERR_OK if successful.
 **/
extern int8_t LcdDispString( char *s, uint8_t row, uint8_t start );

#endif
