/*
 * @project     PanelUI
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Common custom IO and config settings.
 *
 * Copyright (c) 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#define DEV_SERIAL "/dev/serial0"
#define DEV_I2C "/dev/i2c-0"

// These are wiringPi pin numbers (different from RPI gpio numbers)
#define PIN_INT_LEFT        21
#define PIN_INT_RIGHT       7

// Numbers sent by Panel I2C interface
#define LED_CONTROL         6

#define BTN_ENABLE_USB      5
#define BTN_ENABLE_MIDI     4
#define BTN_ENABLE_COUPLER  3
#define BTN_ENABLE_SENDMIDI 2
#define BTN_PAGE_PREV       1
#define BTN_PAGE_NEXT       0

#define BTN_ENCODER_1       7
#define BTN_ENCODER_2       6

#define BTN_OK              9
#define BTN_BACK            8

#define BTN_HOME            13
#define BTN_SETTINGS        12
#define BTN_PRESETS         11
#define BTN_AUDIO           10
#define BTN_ROUTER          17
#define BTN_COUPLER         16
#define BTN_SEQUENCER       15
#define BTN_SYNTH           14

