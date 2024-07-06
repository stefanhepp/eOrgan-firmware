/*
 * @project     Midi Pedalboard
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * LED intensity and mode interface.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __LED_H__
#define __LED_H__

#include <inttypes.h>

enum LEDMode {
    LEDModeNormal = 0,
    LEDModeBlink  = 1,
};

class StatusLED {
    private:
        uint8_t mIntensity;

        void updateIntensity();

    public:
        static const uint8_t MAX_INTENSITY = 0x0F;

        explicit StatusLED() : mIntensity(0) {}
        
        uint8_t getIntensity() const { return mIntensity; }

        void setIntensity(uint8_t intensity);

        /**
         * Initialize all input ports and routines.
         **/
        void begin(uint8_t intensity);

        void reset();

        void setMode(LEDMode mode);

};

#endif
