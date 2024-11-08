/*
 * @project     PanelUI
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Common type definitions
 *
 * Copyright (c) 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#include <panel_commands.h>

enum Panel {
    PANEL_MAIN,
    PANEL_COUPLER
};

enum CouplerMode : uint8_t {
    CM_DISABLED = PANEL_COUPLER_DISABLED,
    CM_MIDI = PANEL_COUPLER_MIDI,
    CM_ENABLED = PANEL_COUPLER_ENABLED
};

enum VolumeChannel : uint8_t {
    VC_MASTER = PANEL_CHANNEL_MASTER,
    VC_USB = PANEL_CHANNEL_USB,
    VC_FX = PANEL_CHANNEL_FX
};

enum PageTurn : uint8_t {
    PT_NEXT = PANEL_PAGE_NEXT,
    PT_PREV = PANEL_PAGE_PREV
};
