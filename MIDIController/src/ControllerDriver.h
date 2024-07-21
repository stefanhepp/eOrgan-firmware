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

using KeyboardStatusCallback =  void(*)(uint8_t channel1, uint8_t channel2, bool training);
using TechnicsStatusCallback = void(*)(uint8_t channel, uint16_t wheel);
using ToeStudStatusCallback = void(*)(uint8_t channel, uint16_t crescendo, uint16_t swell, uint16_t choir); 
using PedalStatusCallback = void(*)(uint8_t channel, uint8_t ledIntensity);

using PistonPressCallback = void(*)(MIDIDivision division, uint8_t button, bool longPress);

class ControllerDriver
{
    private:
        KeyboardStatusCallback mKeyboardStatusCallback = nullptr;
        TechnicsStatusCallback mTechnicsStatusCallback = nullptr;
        ToeStudStatusCallback  mToeStudStatusCallback = nullptr;
        PedalStatusCallback    mPedalStatusCallback = nullptr;

        PistonPressCallback    mPistonPressCallback = nullptr;

        void selectController(Controller controller);

    public:
        explicit ControllerDriver();

        void setKeyboardStatusCallback(KeyboardStatusCallback callback) { mKeyboardStatusCallback = callback; }

        void setTechnicsStatusCallback(TechnicsStatusCallback callback) { mTechnicsStatusCallback = callback; }

        void setToeStudStatusCallback(ToeStudStatusCallback callback) { mToeStudStatusCallback = callback; }

        void setPedalStatusCallback(PedalStatusCallback callback) { mPedalStatusCallback = callback; }

        void setPistonPressCallback(PistonPressCallback callback) { mPistonPressCallback = callback; }


        void resetAll();

        void readAll();


        void setPedalChannel(uint8_t channel);

        void setPedalLEDIntensity(uint8_t intensity);

        void setToestudChannels(uint8_t channel, uint8_t swellChannel, uint8_t choirChannel);

        void setToestudMode(uint8_t mode);

        void setTechnicsChannel(uint8_t channel);

        void setKeyboardChannels(uint8_t channel1, uint8_t channel2);


        void startCalibrateAnalogInputs();

        void stopCalibrateAnalogInputs();

        void trainKeyboard(uint8_t keyboard);


        bool isPistonPressed(MIDIDivision division, uint8_t piston);

        void setPistonLED(MIDIDivision division, uint8_t piston, bool ledOn);


        void begin();

        void loop();
};
