/**
 * @project     MidiController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * I2C configuration controller
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#include <inttypes.h>

class ConfigController
{
    private:

    public:
        explicit ConfigController();

        void begin();

        void loop();
};
