/*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-05 03:01:18 +0200 (Son, 05 Aug 2007) $
 * @version     $Id: adc.c 113 2007-08-05 01:01:18Z stefan $
 *
 * ADC driver implementation.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "adc.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrlib/common/common.h>
#include <config/adc_settings.h>

#include "adc_const.h"

#define ADC_FLAG_STOP	    0x40
#define ADC_FLAG_SWEEP	    0x80
#define ADC_FLAG_MUXMASK    0x1F
#define ADC_MUX_MASK	    ((1<<MUX4)|(1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0))

static uint8_t (*adcCallback)(uint8_t,int16_t) = 0;
static int16_t adcValue;

// stores the flags in the higher bits, and the current mux mode shifted to the lowest 5 bits
static uint8_t adcFlags;
static uint8_t adcPins;

/**
 * enable first set adc input pin.
 *
 * @param start first bit to check.
 * @return position of enabled pin, or 0xFF if none found.
 **/
static uint8_t adcEnableFirst( uint8_t start )
{
    uint8_t i;
    uint8_t pins = (adcPins >> start);

    for ( i = start; i < 8 || pins == 0; i++ ) {
	if ( pins & 1 ) {
	    adcSetSource( (i<<MUX0) );
	    return i;
	}
	pins >>= 1;
    }

    return 0xFF;
}

/**
 * update interal mux state from ADMUX register.
 **/
inline void adcUpdateMux(void)
{
    adcFlags &= ~ADC_FLAG_MUXMASK;
    adcFlags |= (ADMUX & ADC_MUX_MASK)>>MUX0;
}

void adcInit( uint8_t pins, uint8_t prescale, uint8_t refSource )
{
    adcValue = 0;
    adcFlags = ADC_FLAG_STOP;
    adcPins = pins;

    // enable data port
    IO_DDR(ADC_PORT) &= ~pins;
    IO_PORT(ADC_PORT) &= ~pins;

    // set refSource, right adjust data
    ADMUX = 0;
    ADMUX |= refSource;

    // enable first enabled pin as input
    adcEnableFirst( 0 );

    // enable adc, enable interrupt, set prescale
    ADCSRA &= ~((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));
    ADCSRA |= (1<<ADEN)|(1<<ADIE)|(prescale);
}

void adcShutdown( void )
{
    ADCSRA &= ~(1<<ADEN);
}

void adcSetCallback( uint8_t (*adcHandler)(uint8_t source, int16_t value) )
{
    adcCallback = adcHandler;
}

void adcSetSource( uint8_t mux )
{
    ADMUX &= ~ADC_MUX_MASK;
    ADMUX |= mux;
    
    // update status only if no conversion in progress
    if ( !(ADCSRA & (1<<ADSC)) ) {
        adcFlags &= ~ADC_FLAG_MUXMASK;
	adcFlags |= (mux>>MUX0);
    }
}

void adcSetMode( uint8_t mode )
{
    if ( mode == ADC_MODE_SINGLE ) {
	ADCSRA &= ~(1<<ADATE);
    } else {
	ADCSRA |= (1<<ADATE);
	
	SFIOR &= ~((1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0));
	SFIOR |= mode;
    }
}

void adcStartConversion(void)
{
    adcFlags &= ~(ADC_FLAG_STOP|ADC_FLAG_SWEEP);

    ADCSRA |= (1<<ADSC);
}

void adcStartSweep(void)
{
    adcFlags &= ~ADC_FLAG_STOP;
    adcFlags |= ADC_FLAG_SWEEP;

    adcEnableFirst( 0 );

    ADCSRA |= (1<<ADSC);
}

void adcStop(void)
{
    adcFlags |= ADC_FLAG_STOP;
    adcFlags &= ~ADC_FLAG_SWEEP;

    adcSetMode( ADC_MODE_SINGLE );
}

int16_t adcGetLastValue(void)
{
    return ADC;
}

ISR_NOBLOCK(ADC_vect)
{
    uint8_t restart = 0;

    if ( adcFlags & ADC_FLAG_STOP ) {
	adcUpdateMux();
	return;
    }

    if ( adcCallback ) {
	restart = adcCallback( adcFlags & ADC_FLAG_MUXMASK, ADC );
    }

    adcUpdateMux();

    if ( adcFlags & ADC_FLAG_SWEEP ) {
	uint8_t start = ((adcFlags & ADC_FLAG_MUXMASK) + 1) % 8;
	
	// enable next input if sweep is enabled, wrap around after last bit in mask
	if ( adcEnableFirst( start ) == 0xFF ) {
	    adcEnableFirst( 0 );
	}
    }
    
    // immediatly start a new conversion if not running, independend of mode
    if ( restart && bit_is_clear( ADCSRA, ADSC ) ) {
	ADCSRA |= (1<<ADSC);
    }
}

