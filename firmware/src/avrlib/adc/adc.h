/*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-05 03:01:18 +0200 (Son, 05 Aug 2007) $
 * @version     $Id: adc.h 113 2007-08-05 01:01:18Z stefan $
 *
 * Analog to Digital Converter driver library.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __AVRLIB_ADC_ADC_H__
#define __AVRLIB_ADC_ADC_H__

#include <avr/io.h>

/* ==================================================== */
/* Constants                                            */
/* ==================================================== */




/* ==================================================== */
/* Functions                                            */
/* ==================================================== */

/**
 * initialize the ADC unit and input pints.
 *
 * @param pins a bitmask of all ADC input pins.
 * @param prescale ADC prescaler value, one of the ADC_PRESCALE_* defines.
 * @param refSource ADC reference source selection, one of the ADC_REF_* defines.
 **/
void adcInit( uint8_t pins, uint8_t prescale, uint8_t refSource );

/**
 * Disable the ADC unit. Needs to be called before entering 
 * sleep mode to save power. adcInit() has to be called 
 * before any conversion can be started again.
 **/
void adcShutdown( void );

/**
 * Set the callback function to call when a conversion is complete.
 *
 * The used source and the conversion result will be passed to the callback
 * handler. 
 *
 * If the function returns true, a new conversion will be started, if not already  running.
 * If the function returns false, no new conversion is will be started. This will not 
 * disable freerunning mode or any triggered conversions. To do this, call adcStop().
 **/
void adcSetCallback( uint8_t (*adcHandler)(uint8_t source, int16_t value) );

/**
 * Set the ADC input source. Can be any of the ADC_MUX_* defines.
 * 
 * Default is the first enabled ADC input pin.
 *
 * @param mux the input source.
 **/
void adcSetSource( uint8_t mux );

/**
 * Set adc trigger mode. This can be one of the ADC_MODE_* defines. 
 * 
 * Default is single shot mode.
 *
 * @param mode any of the ADC_MODE_* values.
 **/
void adcSetMode( uint8_t mode );

/**
 * Start a conversion with the current settings, if no conversion is currently running.
 *
 * When the conversion is complete, the callback set with adcSetCallback will be called.
 **/
void adcStartConversion(void);

/**
 * Start a conversion with the current settings, sweeping over all enabled inputs.
 *
 * This starts with the first enabled input. Every time a conversion is complete,
 * the next enabled input pin will be selected as source. This overwrites any 
 * previous source setting.
 **/
void adcStartSweep(void);

/**
 * stops the current conversion process and sets the mode to single shot.
 *
 * This is the same as adcSetMode(ADC_MODE_SINGLE), except that any currently
 * running conversion will not trigger a callback.
 **/
void adcStop(void);

/**
 * Get the result of the last completed conversion.
 **/
int16_t adcGetLastValue(void);


#endif
