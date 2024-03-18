/*
 * @project     Midi keyboard
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

static const uint8_t PEDAL_VELOCITY = 64;

class Keyboard
{
    private:
        // callback for key changes
        void (*mKeyChangeHandler)(uint8_t kbd, uint8_t key, uint8_t velocity);
    public:
        explicit Keyboard();

        void setHandleKeyChange(void (*handler)(uint8_t kbd, uint8_t key, uint8_t velocity) );

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
