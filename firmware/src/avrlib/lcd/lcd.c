 /*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: lcd.c 109 2007-08-04 21:22:53Z stefan $
 *
 * Implementation of the high-level lcd driver functions.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "lcd.h"

#include <avr/interrupt.h>

#include <config/lcd_settings.h>

#include "lcd_hw.h"
#include "lcd_const.h"

int8_t LcdInit(void)
{
	LcdHwInit();

	LcdSetBgLight( 50 );
	LcdSetContrast( 50 );
	
	// wait for lcd init
	LcdWriteDelay( 120 );
	
	// set 4/8-bit mode, lines
#if (LCD_ROWS == 4)
	LcdEnableExtMode( 1 );
	LcdWriteCmd( LCE_EXT_FUNC_SET | LCF_DISP_4LINE, 0 );
	LcdEnableExtMode( 0 );
#else
	LcdEnableExtMode( 0 );
#endif

	LcdSetDispMode( LCF_DISP_ON );
	LcdClear();

	return LCD_ERR_OK;
}

int8_t LcdEnableExtMode( uint8_t enable )
{
	// TODO check if 4bit-mode is used (SPI is always 8bit)
	uint8_t cmd = LC_FUNCTION_SET | LCF_DATA_8BIT;
	
	if ( enable ) {
	    cmd |= LCF_REG_EXT;
	}
	
	return LcdWriteCmd( cmd, 0 ); 
}

int8_t LcdClear(void)
{
	uint8_t res;

	res = LcdWriteCmd( LC_CLEAR_DISP, 0 );
	if ( res != LCD_ERR_OK ) {
		return res;
	}

	return LcdWriteDelay( 20 );
}

int8_t LcdSetDispMode( uint8_t mode )
{
	return LcdWriteCmd( LC_DISP_CTL | mode, 0 );
}

int8_t LcdSetScroll( uint8_t lines )
{
	
	return LCD_ERR_OK;
}

int8_t LcdSetMyChar( LcdMyChar *c, uint8_t address )
{
        uint8_t ret,i;

        if ( address >= LCD_USER_CHARS ) {
                return LCD_ERR_INVALID_ADDR;
        }

        // Set address in CGRAM
        ret = LcdWriteCmd( LC_SET_CGRAM_ADDR | (address<<3), 0 );
        if ( ret != LCD_ERR_OK ) {
                return ret;
        }

        // write data
        for ( i = 0; i < 8; i++ ) {
                ret = LcdWriteCmd( c->lines[i], 1 );
                if ( ret != LCD_ERR_OK ) {
                        return ret;
                }
        }

	return LCD_ERR_OK;
}

int8_t LcdSync(void)
{
	while ( LcdGetMode() != LCD_MODE_EMPTY ) ;
	
	return LCD_ERR_OK;
}

int8_t LcdSetCursor( uint8_t row, uint8_t col )
{
	uint8_t addr;
	
	if ( row >= LCD_ROWS || col >= LCD_COLS ) {
		return LCD_ERR_INVALID_ADDR;
	}

	// calc ram address, write as new ddram address
	addr = LCD_ROW_LENGTH * row + col;
	
	return LcdWriteCmd( LC_SET_DDRAM_ADDR | addr, 0 );
}

int8_t LcdDispU16F( uint16_t i, uint8_t row, uint8_t start, uint8_t dispMode, uint8_t digits )
{
	uint8_t ret;
	uint8_t digit,pos;
	
	// get rightmost char (only if justify to left)
	if ( !( dispMode & LCD_JUSTIFY_RIGHT ) ) {
	    if ( digits > 0 ) {
		start += digits - 1;
	    } else {
		// TODO get length of number if not fixed width
	    }
	}
	
	ret = LcdSetCursor( row, start );
	if ( ret != LCD_ERR_OK ) {
	    return ret;
	}
	
	// set shift mode to right
	ret = LcdWriteCmd( LC_ENTRY_MODE, 0 );
	if ( ret != LCD_ERR_OK ) {
	    return ret;
	}
	
	pos = 0;
	while ( 1 ) {
	    
	    digit = i % 10;
	    i = i / 10;

	    // break if no more digits to write and not fixed width
	    if ( i == 0 && pos > 0 && digits == 0 ) {
		break;
	    }
	    
	    // write digit or pad with zero
	    if ( pos > 0 && digit == 0 && i == 0 && !(dispMode & LCD_PAD_ZERO) ) {
		LcdWriteCmd( ' ', 1 );
	    } else {
		LcdWriteCmd( '0' + digit, 1 );
	    }
	    
	    pos++;

	    // break on last digit
	    if ( pos == digits ) {
		break;
	    }
	}
	
	// reset shift mode
	return LcdWriteCmd( LC_ENTRY_MODE | LCF_CURSOR_INC, 0 );
}

int8_t LcdDispSInt( int8_t i, uint8_t row, uint8_t start )
{
	return 0;
}

int8_t LcdDispUInt( uint8_t i, uint8_t row, uint8_t start )
{
	return LcdDispU16F( i, row, start, LCD_PAD_ZERO, 3);
}

int8_t LcdDispHex( uint8_t i, uint8_t row, uint8_t start )
{
	uint8_t res, d;

	res = LcdSetCursor( row, start );
	if ( res != LCD_ERR_OK ) {
		return res;
	}

	// write higher bit
	d = i >> 4;
	if ( d < 10 ) {
	    res = LcdWriteCmd( '0' + d, 1 );
	    if ( res != LCD_ERR_OK ) {
		return res;
	    }
	} else {
	    res = LcdWriteCmd( 'A' + d - 10, 1 );
	    if ( res != LCD_ERR_OK ) {
		return res;
	    }
	}

	// write lower bit
	d = i & 0x0F;
	if ( d < 10 ) {
	    res = LcdWriteCmd( '0' + d, 1 );
	    if ( res != LCD_ERR_OK ) {
		return res;
	    }
	} else {
	    res = LcdWriteCmd( 'A' + d - 10, 1 );
	    if ( res != LCD_ERR_OK ) {
		return res;
	    }
	}
	
	return LCD_ERR_OK;
}

int8_t LcdDispChar( uint8_t c, uint8_t row, uint8_t start )
{
	uint8_t res;

	res = LcdSetCursor( row, start );
	if ( res != LCD_ERR_OK ) {
		return res;
	}

	return LcdWriteCmd( c, 1 );
}

int8_t LcdDispString( char *s, uint8_t row, uint8_t start )
{
	uint8_t res;

	res = LcdSetCursor( row, start );
	if ( res != LCD_ERR_OK ) {
		return res;
	}

	while ( *s ) {
		res = LcdWriteCmd( *(s++), 1 );
		if ( res != LCD_ERR_OK ) {
		    return res;
		}
	}
	
	return LCD_ERR_OK;
}

