/*
 * @project     Midi Pedalboard
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * LED intensity and mode implementation.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "LED.h"

#include "config.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrlib.h>

// ledState: bit 0: led on/off; bit 1: blink state; bit 2: blink mode
#define STATE_LED_ON	0x01
#define STATE_BLINK_ON	0x02
#define STATE_BLINK	    0x04

static uint8_t ledState;

static uint16_t ledCount;

void StatusLED::updateIntensity() 
{
    OCR1A = (mIntensity << 4) + 5;
}

void StatusLED::begin(uint8_t intensity)
{
    ledState = 0x00;
    ledCount = 0;

    mIntensity = intensity;
    updateIntensity();

    // disable output pin, 8-bit PWM
    TCCR1A = (1<<WGM10);

    // set prescaler to CK/64
    TCCR1B &= ~(1<<CS12);
    TCCR1B |= (1<<CS11)|(1<<CS10);

    // Enable interrupts
    TIMSK1 |= (1<<OCIE1A)|(1<<TOIE1);

    // enable led output
    pinMode(PIN_LED, OUTPUT);
}

void StatusLED::reset() 
{
    mIntensity = 0x0F;
    updateIntensity();
}

void StatusLED::setMode(LEDMode mode)
{
    if (mode == LEDModeNormal) {
    	ledState &= ~(STATE_BLINK|STATE_BLINK_ON);
	    ledCount = 0;
    } else {
	    ledState |= STATE_BLINK;
    }
}

void StatusLED::setIntensity(uint8_t intensity)
{
    if ( intensity < MAX_INTENSITY ) {
	    mIntensity = intensity;
    } else {
        mIntensity = MAX_INTENSITY;
    }
	updateIntensity();
}

ISR(TIMER1_OVF_vect)
{
    ledState &= ~STATE_LED_ON;

    if ( ledState & STATE_BLINK ) {
        if (ledCount > 400) {
            ledCount = 0;
        } else {
            ledCount++;
        }
    }
}

ISR(TIMER1_COMPA_vect) 
{
    if ( ledState & STATE_LED_ON ) {
	
	    ledState &= ~STATE_LED_ON;

	    if ( ledCount < 200 ) {
	        digitalWrite(PIN_LED, HIGH);
	    }

    } else {

	    ledState |= STATE_LED_ON;

	    digitalWrite(PIN_LED, LOW);

    }
}
