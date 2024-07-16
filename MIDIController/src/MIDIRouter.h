/**
 * @project     MidiController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * MIDI router implementatoin
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#include <inttypes.h>


class MIDIRouter
{
    private:

    public:
        explicit MIDIRouter();

        // TODO enable/disable routes

        // TODO set channel mapping per input -> output

        void begin();

        void loop();
};
