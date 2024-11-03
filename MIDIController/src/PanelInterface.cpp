/*
 * @project     MIDIController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Panel UART interface handler implementation.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "PanelInterface.h"

#include <Arduino.h>
#include <inttypes.h>

#include <common_config.h>


PanelInterface::PanelInterface()
{

}

uint8_t PanelInterface::readCommandOpcode()
{
    while (Serial6.available()) {
        uint8_t value = Serial6.read();
        if (value & 0x80) {
            return value & ~(0x80);
        }
    }
    return 0;
}

void PanelInterface::receiveCommand()
{
    uint8_t cmd = readCommandOpcode();
    switch (cmd) {
        case PANEL_CMD_PING:

            break;
        default:
            break;
    }
}

void PanelInterface::begin()
{
    Serial6.begin(PANEL_UART_SPEED, SERIAL_8N1);
}

void PanelInterface::loop()
{
    while (Serial6.available()) {
        receiveCommand();
    }
}