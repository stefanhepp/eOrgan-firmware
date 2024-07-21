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
#include "ControllerDriver.h"

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


ControllerDriver::ControllerDriver() 
{
}

void ControllerDriver::selectController(Controller controller)
{
    switch (controller) {
        case Controller::MC_Keyboard:

            break;
        case Controller::MC_Pedal:

            break;
        case Controller::MC_Technics:

            break;
        case Controller::MC_ToeStud:

            break;
    }
}

void ControllerDriver::resetAll()
{
}

void ControllerDriver::readAll()
{
}

void ControllerDriver::setPedalChannel(uint8_t channel)
{
}

void ControllerDriver::setPedalLEDIntensity(uint8_t intensity)
{
}

void ControllerDriver::setToestudChannels(uint8_t channel, uint8_t swellChannel, uint8_t choirChannel)
{
}

void ControllerDriver::setToestudMode(uint8_t mode)
{
}

void ControllerDriver::setTechnicsChannel(uint8_t channel)
{
}

void ControllerDriver::setKeyboardChannels(uint8_t channel1, uint8_t channel2)
{
}

void ControllerDriver::startCalibrateAnalogInputs()
{
}

void ControllerDriver::stopCalibrateAnalogInputs()
{
}

void ControllerDriver::trainKeyboard(uint8_t keyboard)
{
}

bool ControllerDriver::isPistonPressed(MIDIDivision division, uint8_t piston)
{
}

void ControllerDriver::setPistonLED(MIDIDivision division, uint8_t piston, bool ledOn)
{
}


void ControllerDriver::begin()
{
    Wire.begin();
    MP.begin();
}

void ControllerDriver::loop()
{
}
