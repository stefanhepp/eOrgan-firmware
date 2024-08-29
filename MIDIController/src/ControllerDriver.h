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

#include <common_config.h>

using KeyboardStatusCallback =  void(*)(uint8_t channel1, uint8_t channel2, bool training);
using TechnicsStatusCallback = void(*)(uint8_t channel, uint16_t wheel);
using ToeStudStatusCallback = void(*)(uint8_t channel, uint16_t crescendo, uint16_t swell, uint16_t choir); 
using PedalStatusCallback = void(*)(uint8_t channel, uint8_t ledIntensity);

using PistonPressCallback = void(*)(MIDIDivision division, uint8_t button, bool longPress);

static const uint8_t MAX_PISTON_LED_DIVISIONS = 3;
static const uint8_t MAX_PISTON_LED_BYTES = 4;

// Offset of Pedal pistons in choir manual
static const uint8_t CHOIR_PISTON_OFFSET = 12;

class ControllerDriver
{
    private:
        KeyboardStatusCallback mKeyboardStatusCallback = nullptr;
        TechnicsStatusCallback mTechnicsStatusCallback = nullptr;
        ToeStudStatusCallback  mToeStudStatusCallback = nullptr;
        PedalStatusCallback    mPedalStatusCallback = nullptr;

        PistonPressCallback    mPistonPressCallback = nullptr;

        uint8_t mPistonLEDState[MAX_PISTON_LED_DIVISIONS][MAX_PISTON_LED_BYTES];

        int  getPistonLEDIndex(MIDIDivision division);

        void selectController(Controller controller);

        void beginTransmission(Controller controller);

        void requestTransmission(Controller controller, uint8_t length);

        /**
         * Get the division of a button message.
         * @param controller The controller that sent the message
         * @param kbd The keyboard bit of the message
         * @param btn The button index
         */
        MIDIDivision getPistonDivision(Controller controller, uint8_t kbd, uint8_t &btnIndex);

        void readPistons(Controller controller);

    public:
        explicit ControllerDriver();

        void printIRQStatus();

        void setKeyboardStatusCallback(KeyboardStatusCallback callback) { mKeyboardStatusCallback = callback; }

        void setTechnicsStatusCallback(TechnicsStatusCallback callback) { mTechnicsStatusCallback = callback; }

        void setToeStudStatusCallback(ToeStudStatusCallback callback) { mToeStudStatusCallback = callback; }

        void setPedalStatusCallback(PedalStatusCallback callback) { mPedalStatusCallback = callback; }

        void setPistonPressCallback(PistonPressCallback callback) { mPistonPressCallback = callback; }


        void resetAll();


        void readStatusKeyboard();

        void readStatusTechnics();

        void readStatusPedal();

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


        void setPistonLED(MIDIDivision division, uint8_t piston, bool ledOn);


        void begin();

        void loop();
};
