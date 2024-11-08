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

#define PIN_INT_LEFT  5
#define PIN_INT_RIGHT 4

#define LED_CONTROL     6

#define BTN_ENABLE_USB      5
#define BTN_ENABLE_MIDI     4
#define BTN_ENABLE_COUPLER  3
#define BTN_ENABLE_SENDMIDI 2
#define BTN_PAGE_NEXT       1
#define BTN_PAGE_PREV       0

#define BTN_ENCODER_1       6
#define BTN_ENCODER_2       7

#define BTN_OK              8
#define BTN_BACK            9

#define BTN_HOME            10
#define BTN_SETTINGS        11
#define BTN_PRESETS         12
#define BTN_AUDIO           13
#define BTN_ROUTER          14
#define BTN_COUPLER         15
#define BTN_SEQUENCER       16
#define BTN_SYNTH           17
