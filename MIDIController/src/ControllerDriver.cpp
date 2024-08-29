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

static const int PIN_INT_KEYBOARD   = 32;
static const int PIN_INT_TECHNICS   = 31;
static const int PIN_INT_STOP_RIGHT = 41;
static const int PIN_INT_STOP_LEFT  = 40;
static const int PIN_INT_TOESTUD    = 30;
static const int PIN_INT_PANEL      = 36;


ControllerDriver::ControllerDriver() 
{
    for (int i = 0; i < MAX_PISTON_LED_DIVISIONS; i++) {
        for (int j = 0; j < MAX_PISTON_LED_BYTES; j++) {
            mPistonLEDState[i][j] = 0;
        }
    }
}

int getPistonIndex(MIDIDivision division) {
    switch (division) {
        case MIDIDivision::MD_Pedal:
        case MIDIDivision::MD_Choir:
            return 0;
        case MIDIDivision::MD_Swell:
            return 1;
        case MIDIDivision::MD_Solo:
            return 2;
        case MIDIDivision::MD_Control:
            // Should not be reached!
            return 0;
    }
    return 0;
}

void ControllerDriver::selectController(Controller controller)
{
    switch (controller) {
        case Controller::MC_Keyboard:
        case Controller::MC_Piston_Keyboard:
            MP.selectChannel(6);
            break;
        case Controller::MC_Technics:
        case Controller::MC_Piston_Technics:
            MP.selectChannel(5);
            break;
        case Controller::MC_Pedal:
        case Controller::MC_ToeStud:
            MP.selectChannel(4);
            break;
        case Controller::MC_Panel:
            MP.selectChannel(0);
            break;
        case Controller::MC_PowerControl:
            // On StopLeft port
            MP.selectChannel(2);
            break;
    }
}

void ControllerDriver::beginTransmission(Controller controller)
{
    selectController(controller);
    Wire.beginTransmission(controller);
}

void ControllerDriver::requestTransmission(Controller controller, uint8_t length)
{
    selectController(controller);
    Wire.requestFrom(controller, length);
}

void ControllerDriver::resetAll()
{
    beginTransmission(Controller::MC_Keyboard);
    Wire.write(I2C_CMD_RESET);
    Wire.endTransmission();

    beginTransmission(Controller::MC_Piston_Keyboard);
    Wire.write(I2C_CMD_RESET);
    Wire.endTransmission();

    beginTransmission(Controller::MC_Technics);
    Wire.write(I2C_CMD_RESET);
    Wire.endTransmission();

    beginTransmission(Controller::MC_Piston_Technics);
    Wire.write(I2C_CMD_RESET);
    Wire.endTransmission();

    beginTransmission(Controller::MC_Pedal);
    Wire.write(I2C_CMD_RESET);
    Wire.endTransmission();

    beginTransmission(Controller::MC_ToeStud);
    Wire.write(I2C_CMD_RESET);
    Wire.endTransmission();
}

MIDIDivision ControllerDriver::getPistonDivision(Controller controller, uint8_t kbd, uint8_t &btnIndex) {
    switch (controller) {
        case Controller::MC_ToeStud:
            return MIDIDivision::MD_Control;

        case Controller::MC_Piston_Technics:
            // Lower part of buttons are Pedal buttons
            if (btnIndex >= CHOIR_PISTON_OFFSET) {
                btnIndex -= CHOIR_PISTON_OFFSET;
            }
            return btnIndex < CHOIR_PISTON_OFFSET 
                   ? MIDIDivision::MD_Pedal : MIDIDivision::MD_Choir;

        case Controller::MC_Piston_Keyboard:
            return kbd ? MIDIDivision::MD_Solo : MIDIDivision::MD_Swell;

        default:
            // Should never be reached
            return MIDIDivision::MD_Solo;
    }
}

void ControllerDriver::readPistons(Controller controller) {

    uint8_t queueLength = Wire.read();

    while (Wire.available()) {
        uint8_t btn = Wire.read();
        if (btn == 0xFF) {
            break;
        }

        uint8_t index = (btn & ~(1<<7))>>1;
        uint8_t kbd = btn & (1<<7);
        uint8_t longPress = btn & 0x01;

        MIDIDivision division = getPistonDivision(controller, kbd, index);

        if (mPistonPressCallback) {
            mPistonPressCallback(division, index, longPress > 0);
        }
    }
}

void ControllerDriver::readStatusKeyboard()
{
    requestTransmission(Controller::MC_Keyboard, 3);
    if (Wire.available() >= 3) {
        uint8_t channel1 = Wire.read();
        uint8_t channel2 = Wire.read();
        uint8_t training = Wire.read();

        if (mKeyboardStatusCallback) {
            mKeyboardStatusCallback(channel1, channel2, training > 0);
        }
    }
    
    requestTransmission(Controller::MC_Piston_Keyboard, 8);
    readPistons(Controller::MC_Piston_Keyboard);
}

void ControllerDriver::readStatusTechnics()
{
    requestTransmission(Controller::MC_Technics, 3);
    if (Wire.available() >= 3) {
        uint8_t channel   = Wire.read();
        uint8_t wheelHigh = Wire.read();
        uint8_t wheelLow  = Wire.read();

        uint16_t wheel = (wheelHigh << 8) | wheelLow;

        if (mTechnicsStatusCallback) {
            mTechnicsStatusCallback(channel, wheel);
        }
    }
    
    requestTransmission(Controller::MC_Piston_Technics, 8);
    readPistons(Controller::MC_Piston_Technics);
}

void ControllerDriver::readStatusPedal()
{
    requestTransmission(Controller::MC_Pedal, 2);
    if (Wire.available() >= 2) {
        uint8_t channel   = Wire.read();
        uint8_t intensity = Wire.read();

        if (mPedalStatusCallback) {
            mPedalStatusCallback(channel, intensity);
        }
    }
    
    requestTransmission(Controller::MC_ToeStud, 16);
    if (Wire.available() >= 8) {
        uint8_t channel    = Wire.read();
        uint8_t pedal1High = Wire.read();
        uint8_t pedal1Low  = Wire.read();
        uint8_t pedal2High = Wire.read();
        uint8_t pedal2Low  = Wire.read();
        uint8_t pedal3High = Wire.read();
        uint8_t pedal3Low  = Wire.read();

        uint16_t pedalCrescendo = pedal1High << 8 | pedal1Low;
        uint16_t pedalSwell     = pedal2High << 8 | pedal2Low;
        uint16_t pedalChoir     = pedal3High << 8 | pedal3Low;

        if (mToeStudStatusCallback) {
            mToeStudStatusCallback(channel, pedalCrescendo, pedalSwell, pedalChoir);
        }

        readPistons(Controller::MC_ToeStud);
    }
}

void ControllerDriver::readAll()
{
    readStatusKeyboard();
    readStatusTechnics();
    readStatusPedal();
}


void ControllerDriver::setPedalChannel(uint8_t channel)
{
    beginTransmission(Controller::MC_Pedal);
    Wire.write(I2C_CMD_SET_CHANNEL);
    Wire.write(channel);
    Wire.endTransmission();
}

void ControllerDriver::setPedalLEDIntensity(uint8_t intensity)
{
    beginTransmission(Controller::MC_Pedal);
    Wire.write(I2C_CMD_LED_INTENSITY);
    Wire.write(intensity);
    Wire.endTransmission();
}

void ControllerDriver::setToestudChannels(uint8_t channel, uint8_t swellChannel, uint8_t choirChannel)
{
    beginTransmission(Controller::MC_ToeStud);
    Wire.write(I2C_CMD_SET_CHANNEL);
    Wire.write(channel);
    Wire.write(swellChannel);
    Wire.write(choirChannel);
    Wire.endTransmission();
}

void ControllerDriver::setToestudMode(uint8_t mode)
{
    beginTransmission(Controller::MC_ToeStud);
    Wire.write(I2C_CMD_SET_MODE);
    Wire.write(mode);
    Wire.endTransmission();
}

void ControllerDriver::setTechnicsChannel(uint8_t channel)
{
    beginTransmission(Controller::MC_Technics);
    Wire.write(I2C_CMD_SET_CHANNEL);
    Wire.write(channel);
    Wire.endTransmission();
}

void ControllerDriver::setKeyboardChannels(uint8_t channel1, uint8_t channel2)
{
    beginTransmission(Controller::MC_Keyboard);
    Wire.write(I2C_CMD_SET_CHANNEL);
    Wire.write(channel1);
    Wire.write(channel2);
    Wire.endTransmission();
}

void ControllerDriver::startCalibrateAnalogInputs()
{
    beginTransmission(Controller::MC_Technics);
    Wire.write(I2C_CMD_CALIBRATE);
    Wire.endTransmission();

    beginTransmission(Controller::MC_ToeStud);
    Wire.write(I2C_CMD_CALIBRATE);
    Wire.endTransmission();
}

void ControllerDriver::stopCalibrateAnalogInputs()
{
    beginTransmission(Controller::MC_Technics);
    Wire.write(I2C_CMD_STOP_CALIBRATE);
    Wire.endTransmission();

    beginTransmission(Controller::MC_ToeStud);
    Wire.write(I2C_CMD_STOP_CALIBRATE);
    Wire.endTransmission();
}

void ControllerDriver::trainKeyboard(uint8_t keyboard)
{
    beginTransmission(Controller::MC_Keyboard);
    Wire.write(I2C_CMD_CALIBRATE);
    Wire.write(keyboard);
    Wire.endTransmission();
}

void ControllerDriver::setPistonLED(MIDIDivision division, uint8_t piston, bool ledOn)
{
    if (piston > MAX_PISTON_LED_BYTES*8) {
        return;
    }

    int idx = getPistonLEDIndex(division);

    if (division == MIDIDivision::MD_Choir) {
        // Lower pistons are Pedal pistons, start with offset
        piston += CHOIR_PISTON_OFFSET;
    }

    uint8_t state = mPistonLEDState[idx][piston/8];
    if (ledOn) {
        state |=  (1<<(piston % 8));
    } else {
        state &= ~(1<<(piston % 8));
    }
    mPistonLEDState[idx][piston/8] = state;

    switch (division) {
        case MIDIDivision::MD_Choir:
        case MIDIDivision::MD_Pedal:
            beginTransmission(Controller::MC_Piston_Technics);
            Wire.write(I2C_CMD_SET_LEDS);
            for (int i = 0; i < MAX_PISTON_LED_BYTES; i++) {
                Wire.write(mPistonLEDState[idx][i]);
            }
            Wire.endTransmission();
            break;
        case MIDIDivision::MD_Swell:
        case MIDIDivision::MD_Solo:
            beginTransmission(Controller::MC_Piston_Keyboard);
            Wire.write(I2C_CMD_SET_LEDS);
            for (int i = 0; i < MAX_PISTON_LED_BYTES; i++) {
                uint8_t mask = mPistonLEDState[idx][i];
                if (division == MIDIDivision::MD_Solo && i == 0) {
                    // Set highest bit to indicate keyboard number 2
                    mask |= (1<<7);
                }
                Wire.write(mask);
            }
            Wire.endTransmission();
            break;
        default:
            // Ignore other controllers
            break;
    }
}

void ControllerDriver::printIRQStatus()
{
    uint8_t irqKeyboard  = digitalRead(PIN_INT_KEYBOARD);
    uint8_t irqTechnics  = digitalRead(PIN_INT_TECHNICS);
    uint8_t irqToestud   = digitalRead(PIN_INT_TOESTUD);
    uint8_t irqPanel     = digitalRead(PIN_INT_PANEL);
    uint8_t irqStopLeft  = digitalRead(PIN_INT_STOP_LEFT);
    uint8_t irqStopRight = digitalRead(PIN_INT_STOP_RIGHT);

    Serial.printf("IRQ: Kbd=%s Technics=%s", irqKeyboard ? "HI":"LO", irqTechnics ? "HI":"LO");
    Serial.printf(" ToeStud=%s Panel=%s", irqToestud ? "HI":"LO", irqPanel ? "HI":"LO");
    Serial.printf(" StopLeft=%s StopRight=%s", irqStopLeft ? "HI":"LO", irqStopRight ? "HI":"LO");
    Serial.println();
}

void ControllerDriver::begin()
{
    pinMode(PIN_INT_KEYBOARD,   INPUT);
    pinMode(PIN_INT_TECHNICS,   INPUT);
    pinMode(PIN_INT_TOESTUD,    INPUT);
    pinMode(PIN_INT_PANEL,      INPUT);
    pinMode(PIN_INT_STOP_LEFT,  INPUT);
    pinMode(PIN_INT_STOP_RIGHT, INPUT);

    Wire.begin();
    MP.begin();
}

void ControllerDriver::loop()
{
    // Read interrupt request lines until interrupt is cleared
    if (digitalRead(PIN_INT_KEYBOARD) == LOW) {
        readStatusKeyboard();
    }
    if (digitalRead(PIN_INT_TECHNICS) == LOW) {
        readStatusTechnics();
    }
    if (digitalRead(PIN_INT_TOESTUD) == LOW) {
        readStatusPedal();
    }
}
