/**
 * @project     MidiController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Commandline parser
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#include <inttypes.h>

class CmdlineParser
{
    private:

    public:
        explicit Cmdline();

        void begin();

        void loop();
};