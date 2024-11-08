#include <iostream>

#include <stdint.h>

#include <wiringPi.h>

#include "config.h"
#include "commonTypes.h"

#include "SerialInterface.h"
#include "PanelDriver.h"

SerialInterface serial;
PanelDriver panel;

bool ReceivedStatus = false;
StatusInfo LastStatus;


void onButtonPress(Panel panel, int button, bool longPress)
{
    std::cout << "Got Button " << button << ": " << (longPress ? "LONG" : "SHORT") << std::endl;

    if (!ReceivedStatus) {
        return;
    }

    if (panel == Panel::PANEL_MAIN) {
        switch (button) {
            case BTN_ENABLE_USB:
                

                serial.requestStatus();
                break;
            case BTN_ENABLE_MIDI:


                serial.requestStatus();
                break;
            case BTN_ENABLE_COUPLER:


                serial.requestStatus();
                break;
            case BTN_ENABLE_SENDMIDI:


                serial.requestStatus();
                break;
            case BTN_PAGE_NEXT:
                serial.sendPageTurn(PageTurn::PT_NEXT);
                break;
            case BTN_PAGE_PREV:
                serial.sendPageTurn(PageTurn::PT_PREV);
                break;
            case BTN_ENCODER_1:
                break;
            case BTN_ENCODER_2:
                break;
            case BTN_OK:
                break;
            case BTN_BACK:
                break;
            case BTN_HOME:
                break;
            case BTN_SETTINGS:
                break;
            case BTN_PRESETS:
                break;
            case BTN_AUDIO:
                break;
            case BTN_ROUTER:
                break;
            case BTN_COUPLER:
                break;
            case BTN_SEQUENCER:
                break;
            case BTN_SYNTH:
                break;
        }
    }
}

void onVolumeChange(VolumeChannel channel, int volume)
{
    std::cout << "Got Volume " << channel << ": " << volume << std::endl;

    serial.sendVolume(channel, volume);
}

void onEncoderChange(int encoder, int delta)
{
    std::cout << "Got Encoder " << encoder << ": " << delta << std::endl;
}

void onStatusInfo(StatusInfo &status)
{
    uint8_t leds = 0x00;
    leds |= status.USBOutputEnabled  ? 1 << BTN_ENABLE_USB      : 0;
    leds |= status.MIDIOutputEnabled ? 1 << BTN_ENABLE_MIDI     : 0;
    leds |= status.CouplerEnabled    ? 1 << BTN_ENABLE_COUPLER  : 0;
    leds |= status.SendMIDIMessages  ? 1 << BTN_ENABLE_SENDMIDI : 0;
    leds |= 1 << LED_CONTROL;
    panel.setMainPanelLEDs(leds);

    if (!ReceivedStatus) {
        // first time we receive an answer from main controller,
        // read the status from the panel and sync main controller
        panel.readMainPanelStatus();
    }
    ReceivedStatus = true;
    LastStatus = status;
}

int main(void)
{
    wiringPiSetup();

    serial.setStatusCallback(onStatusInfo);

    panel.setButtonPressCallback(onButtonPress);
    panel.setVolumeChangeCallback(onVolumeChange);
    panel.setEncoderChangeCallback(onEncoderChange);

    if (serial.begin() < 0) { 
        return -1;
    }
    if (panel.begin() < 0) {
        return -1;
    }

    for (;;) {
        if (!ReceivedStatus) {
            serial.requestStatus();
        }

        panel.loop();
        serial.loop();

        delay(20);
    }

    return 0;
}

