/*
 * @project     LED Controller
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * RGB LED driver.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __LED_H__
#define __LED_H__

#include <inttypes.h>

static const uint8_t NUM_LEDS = 7;

class LEDDriver {
    private:
        uint8_t mIntensity[NUM_LEDS];

        uint8_t mPWMCounter;
        uint8_t mLastCounter;

    public:
        explicit LEDDriver();
        
        uint8_t getIntensity(int index) const { return mIntensity[index]; }

        void setIntensity(int index, uint8_t intensity) { mIntensity[index] = intensity; }

        /**
         * Initialize all input ports and routines.
         **/
        void begin();

        void reset();

        /**
         * Update LED PWM status.
         * 
         * Return true if PWM counter wrapped around (TOP was reached).
         */
        bool updateLEDs();

        uint8_t getLEDStatus(int index) { 
            return mIntensity[index] > 0 && mPWMCounter <= mIntensity[index] ? 1 : 0; 
        }
};

#endif
