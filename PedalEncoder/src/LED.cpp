/*
 * @project     Midi Pedalboard
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:21:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: input.h 108 2007-08-04 21:21:53Z stefan $
 *
 * LED intensity and mode implementation.
 *
 * Copyright 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "LED.h"

#include "config.h"

#include <avr/io.h>
#include <avr/interrupt.h>

// bit 0: led on/off; bit 1: blink state; bit 2: blink mode
#define STATE_LED_ON	0x01
#define STATE_BLINK_ON	0x02
#define STATE_BLINK	0x04

void StatusLED::updateIntensity() 
{
    OCR1A = (mIntensity << 4) + 5;
}

void StatusLED::begin(uint8_t intensity)
{
    mMode = LEDModeNormal;
    mBlinkCount = 0;
    mBlinkOn = false;

    mIntensity = intensity;
    updateIntensity();

    // disable output pin, 8-bit PWM
    /*
    TCCR1A &= ~((1<<COM1A1)|(1<<COM1A0)|(1<<PWM11));
    TCCR1A |= (1<<PWM10);

    // set prescaler to CK/8
    TCCR1B &= ~((1<<CS12)|(1<<CS10));
    TCCR1B |= (1<<CS11);

    // Enable interrupts
    TIMSK |= (1<<OCIE1A)|(1<<TOIE1);

    // enable led output
    IO_DDR(PORT_LED) |= (1<<PIN_LED);
    IO_PORT(PORT_LED) |= (1<<PIN_LED);
    */
}

void StatusLED::reset() 
{
    mIntensity = 0x0F;
    updateIntensity();
}

void StatusLED::setMode(LEDMode mode)
{
    mMode = mode;
    mBlinkCount = 0;
    mBlinkOn = false;
}

void StatusLED::decreaseIntensity()
{
    if ( mIntensity > 0 ) {
	    mIntensity--;
	    updateIntensity();
    }
}

void StatusLED::increaseIntensity()
{
    if ( mIntensity < 0x0F ) {
    	mIntensity++;
	    updateIntensity();
    }
}

/*
ISR(SIG_OVERFLOW1)
{
    sei();

    state &= ~STATE_LED_ON;

    if ( state & STATE_BLINK ) {

	if (cntBlink > 600) {
	    cntBlink = 0;
	} else {
	    cntBlink++;
	}
    }
}

ISR(SIG_OUTPUT_COMPARE1A) 
{
    sei();

    if ( state & STATE_LED_ON ) {
	
	state &= ~STATE_LED_ON;

	if ( cntBlink < 320 ) {
	    //IO_PORT(PORT_LED) |= (1<<PIN_LED);
	}

    } else {

	state |= STATE_LED_ON;

	//IO_PORT(PORT_LED) &= ~(1<<PIN_LED);

    }
}
*/
