/*
 * @project     Midi Pedalboard
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:21:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: input.h 108 2007-08-04 21:21:53Z stefan $
 *
 * LED intensity and mode interface.
 *
 * Copyright 2007 Stefan Hepp
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
