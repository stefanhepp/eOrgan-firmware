/*
 * @project     MIDIController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Cmdline parser implementation
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "CmdlineParser.h"

#include <Arduino.h>

#include <inttypes.h>

#include <common_config.h>

CmdlineParser::CmdlineParser()
{

}

void CmdlineParser::begin()
{
    // Serial over USB. Speed is ignored.
    Serial.begin(9600);
}

void CmdlineParser::loop()
{
    Serial.println("Sending");

    delay(1000);
}
