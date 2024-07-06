/*
 * @project     TechnicsController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * BendWheel processing.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "BendWheel.h"

#include <inttypes.h>
#include <avr/io.h>

#include <avrlib.h>

#include "config.h"

BendWheel::BendWheel() {
}

void BendWheel::setOnChange( void(*handler)(uint8_t value) ) {
    mWheelChangeHandler = handler;
}

void BendWheel::begin()
{
}

void BendWheel::startCalibration()
{

}

void BendWheel::stopCalibration()
{

}

void BendWheel::poll()
{
    
}
