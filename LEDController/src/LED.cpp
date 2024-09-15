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

LEDDriver::LEDDriver() 
: mPWMCounter(0), mLastCounter(0)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        mIntensity[i] = 0;
    }
}

void LEDDriver::begin()
{
    // no output, 8-bit normal PWM
    TCCR2A = 0; 

    // set prescaler to CK/256 (~10ms period)
    TCCR2B = (1<<CS22)|(1<<CS21);

    // Enable interrupts
    TIMSK2 = 0;
}

void LEDDriver::reset() 
{
    for (int i = 0; i < NUM_LEDS; i++) {
        mIntensity[i] = 0;
    }
}

bool LEDDriver::updateLEDs() {
    mLastCounter = mPWMCounter;
    mPWMCounter = TCNT2;
    return mPWMCounter < mLastCounter;
}
