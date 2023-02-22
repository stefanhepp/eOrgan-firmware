/*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: lcd_const.h 109 2007-08-04 21:22:53Z stefan $
 *
 * LCD driver common constants definitions.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __AVRLIB_LCD_CONST_H__
#define __AVRLIB_LCD_CONST_H__

/* ==================================================== */
/* Constants                                            */
/* ==================================================== */

/**
 * LCD error codes
 **/
#define LCD_ERR_OK      	0
#define LCD_ERR_INVALID_ADDR	1
#define LCD_ERR_BUF_FULL	2
#define LCD_ERR_BUF_EMPTY       3
#define LCD_ERR_COLLISION	4

/**
 * Constants for lcd command bytes and flags.
 * 
 * These values are for the KS0073 lcd display controller.
 **/
// functions
#define LC_CLEAR_DISP		0x01
#define LC_CURSOR_HOME		0x02
#define LC_ENTRY_MODE		0x04
#define LC_DISP_CTL		0x08
#define LC_DISP_SHIFT		0x10
#define LC_FUNCTION_SET		0x20
#define LC_SET_CGRAM_ADDR	0x40
#define LC_SET_DDRAM_ADDR	0x80

// extended functions 
#define LCE_EXT_FUNC_SET	0x08
#define LCE_SCROLL_ENABLE	0x10
#define LCE_SET_SEGRAM_ADDR	0x40

// flags for entry mode
#define LCF_CURSOR_INC		0x02
#define LCF_SHIFT_ENABLE	0x01
// flags for disp ctl
#define LCF_DISP_ON		0x04
#define LCF_CURSOR_ON		0x02
#define LCF_BLINK_ON		0x01
// flags for disp shift
#define LCF_DISP_SHIFT		0x08
#define LCF_SHIFT_RIGHT		0x04
// flags for function set
#define LCF_DATA_8BIT		0x10
#define LCF_DISP_2LINE		0x08
#define LCF_REG_EXT		0x04
#define LCF_BLINK_ENABLE	0x02
#define LCF_LOW_POWER		0x01
// flags for ext. func. set
#define LCF_DISP_4LINE		0x01
#define LCF_CURSOR_INVERT	0x02
#define LCF_FONT_6DOT		0x04


#endif

