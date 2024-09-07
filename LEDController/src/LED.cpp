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

// ledState: bit 0: blink mode
#define STATE_BLINK	    0x01

static uint8_t ledState;

static uint16_t ledCount;

void StatusLED::updateIntensity() 
{
    if (mIntensity == 0) {
        OCR2B = 0;
    } else {
        OCR2B = (mIntensity << 4) + 15;
    }
}

void StatusLED::begin(uint8_t intensity)
{
    ledState = 0x00;
    ledCount = 0;

    mIntensity = intensity;
    updateIntensity();

    // enable led output, disabled by default
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);

    // enable OC2B (PD3) output pin (clear on upcount), 8-bit phase-correct PWM
    TCCR2A = (1<<COM2B1)|(1<<WGM20); 

    // set prescaler to CK/32
    TCCR2B = (1<<CS21)|(1<<CS20);

    // Enable interrupts
    TIMSK2 |= (1<<TOIE2);
}

void StatusLED::reset() 
{
    mIntensity = 15;
    updateIntensity();
}

void StatusLED::setMode(LEDMode mode)
{
    if (mode == LEDModeNormal) {
    	ledState &= ~(STATE_BLINK);
	    ledCount = 0;
    } else {
	    ledState |= STATE_BLINK;
    }

    // Ensure PWM output is on initially
    TCCR2A |= (1<<COM2B1);
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

ISR(TIMER2_OVF_vect)
{
    if ( ledState & STATE_BLINK ) {
        ledCount++;
        if (ledCount > 600) {
            ledCount = 0;
            // enable PWM output
            TCCR2A |= (1<<COM2B1);
        } else if (ledCount > 300) {
            // disable PWM output
            TCCR2A &= ~(1<<COM2B1);
        }
    }
}
