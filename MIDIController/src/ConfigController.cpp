/*
 * @project     MIDIController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * MIDI Router implementation
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "ConfigController.h"

#include <Arduino.h>
#include <Wire.h>
#include <TCA9548.h>

#include <inttypes.h>

#include <common_config.h>


// I2C Multiplexer:
TCA9548 MP(0x70);

// I2C Multiplexer Ports
static const int I2C_PORT_PANEL     = 0;
static const int I2C_PORT_I2C1      = 1;
static const int I2C_PORT_STOPLEFT  = 2;
static const int I2C_PORT_STOPRIGHT = 3;
static const int I2C_PORT_TOESTUDS  = 4;
static const int I2C_PORT_TECHNICS  = 5;
static const int I2C_PORT_KEYBOARD  = 6;
static const int I2C_PORT_I2C2      = 7;


ConfigController::ConfigController() 
{

}

void ConfigController::begin()
{
    Wire.begin();
    MP.begin();
}

void ConfigController::loop()
{

}
