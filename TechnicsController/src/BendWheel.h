/*
 * @project     Bend wheel driver
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Bender Wheel processing
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __BENDWHEEL_H__
#define __BENDWHEEL_H__

#include <inttypes.h>

class BendWheel
{
    private:
        uint8_t mWheelValue;

        // callback for key changes
        void (*mWheelChangeHandler)(uint8_t value) = nullptr;
    public:
        explicit BendWheel();

        uint8_t getWheel() { return mWheelValue; };

        void setOnChange(void (*handler)(uint8_t value) );

        /**
         * Initialize all pins and routines.
         **/
        void begin();

        /**
         * Begin a calibration cycle.
         */
        void startCalibration();

        /**
         * Force calibration stop.
         */
        void stopCalibration();

        /** 
         * poll input ports for changes, call handler on changed notes.
         **/
        void poll();
};

#endif