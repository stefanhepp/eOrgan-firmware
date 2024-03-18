/*
 * @project     Midi Pedalboard
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Input encoding and processing routines.
 *
 * Copyright 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __PEDAL_H__
#define __PEDAL_H__

#include <inttypes.h>

class Pedal
{
    private:
        // current active pedal line
        uint8_t mPedalLine;
        // last state of pedal switches
        uint8_t mPedalState[4];

        // callback for key changes
        void (*mKeyChangeHandler)(uint8_t key, uint8_t velocity);
    public:
        explicit Pedal();

        void setHandleKeyChange(void (*handler)(uint8_t key, uint8_t velocity) );

        /**
         * Initialize all pins and routines.
         **/
        void begin();

        /** 
         * poll input ports for changes, call handler on changed notes.
         **/
        void poll();
};

#endif
