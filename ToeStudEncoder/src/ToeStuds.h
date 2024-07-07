/*
 * @project     ToeStudController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Input encoding and processing routines.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#ifndef __TOESTUDS_H__
#define __TOESTUDS_H__

#include <inttypes.h>

class ToeStuds
{
    private:
        // callback for key changes
        void (*mChangeHandler)(uint8_t btn, bool longPress) = nullptr;

        void readLine(const uint8_t line);
    public:
        explicit ToeStuds() {}

        void setHandleButton(void (*handler)(uint8_t btn, bool longPress) );

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
