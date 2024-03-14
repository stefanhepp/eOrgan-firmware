/*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: lcd_settings.h 109 2007-08-04 21:22:53Z stefan $
 *
 * LCD driver custom IO and config settings.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __CONFIG_LCD_SETTINGS_H__
#define __CONFIG_LCD_SETTINGS_H__

#include <config/common.h>

/* ==================================================== */
/* Common display settings                              */
/* ==================================================== */

/**
 * Display size settings
 **/
#define LCD_ROWS                4
#define LCD_COLS                20
#define LCD_USER_CHARS          8
#define LCD_ROW_LENGTH          0x20

// TODO define macro to enable synchronized writes of commands

/**
 * set to 1 to use hardware SPI
 **/
#define LCD_HW_SPI              1

/**
 * define cmd buffer size.
 **/
#define LCD_CMD_BUF_SIZE        16

/**
 * If set to 1, delay any write-ops to buffer with busy wait, if buffer is full.
 * Interrupts must then be enabled during Lcd* ops.
 **/
#define LCD_BUSYWAIT_ON_FULL    1


/* ==================================================== */
/* Constants, IO settings                               */
/* ==================================================== */

/**
 * define ports for bglight and contrast.
 * If *_PIN is not defined, no corresponding pwm signal will be generated.
 **/
#define LCD_BGLIGHT_DDR         DDRD
#define LCD_BGLIGHT_PORT        PORTD
#define LCD_BGLIGHT_PIN         PD4

#define LCD_CONTRAST_DDR        DDRD
#define LCD_CONTRAST_PORT       PORTD
#define LCD_CONTRAST_PIN        PB5

// SPI (ATmega 16)
#define SPI_DDR                 DDRB
#define SPI_PORT                PORTB
#define SPI_SS_PIN              PB4
#define SPI_MISO_PIN            PB6
#define SPI_SCK_PIN             PB7
#define SPI_MOSI_PIN            PB5


/* ==================================================== */
/* Constants, Timer, buffer settings                    */
/* ==================================================== */

// Set oc1b to clear on compare, set mode to fast pwm, 200 TOP (ICR1), 8 Prescaler (=~0.1ms/TOP)
// TODO define only timer nr and clock, calc settings, define HW/SW PWM, use LCD_ prefix
// TODO define different timer for PWM/use timer lib methods..?
#define TIMER_TCCRA             TCCR1A
#define TIMER_TCCRB             TCCR1B
#define TIMER_TCCRA_SET         (1<<COM1B1)|(1<<COM1A1)|(1<<WGM11)
#define TIMER_TCCRA_CLR         (1<<COM1B0)|(1<<COM1A0)|(1<<WGM10)
#define TIMER_TCCRB_SET         (1<<WGM12)|(1<<WGM13)|(1<<CS11)
#define TIMER_TCCRB_CLR         (1<<CS12)|(1<<CS10)
#define TIMER_TIMSK             TIMSK
#define TIMER_OIE               TOIE1
#define TIMER_SIG_OVERFLOW      SIG_OVERFLOW1
#define TIMER_TOP               ICR1

/**
 * PWM register.
 **/
#define LCD_TIMER_OCR_BGLIGHT   OCR1B
#define LCD_TIMER_OCR_CONTRAST  OCR1A


#endif

