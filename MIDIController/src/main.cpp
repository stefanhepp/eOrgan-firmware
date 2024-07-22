/**
 * @author Stefan Hepp (stefan@stefant.org)
 * 
 * Main program of the central MIDI controller.
 */

#include <Arduino.h>

#include "CommandLine.h"
#include "MIDIRouter.h"
#include "ControllerDriver.h"
#include "OrganStateManager.h"

// Serial6: UART Panel

CommandLine Cmdline;
MIDIRouter MIDI;
ControllerDriver Control;
OrganStateManager StateMngr(MIDI, Control);

static const int PRINT_KEYBOARD = 0x01;
static const int PRINT_TECHNICS = 0x02;
static const int PRINT_PEDAL    = 0x04;
static const int PRINT_TOESTUD  = 0x08;
static const int PRINT_ALL = PRINT_KEYBOARD | PRINT_TECHNICS | PRINT_PEDAL | PRINT_TOESTUD;

static int PrintNextStatus = 0;

static bool KeyboardIsLearning = false;

static const char* divisionName(MIDIDivision division) {
    switch (division) {
        case MIDIDivision::MD_Choir:
            return "Choir";
        case MIDIDivision::MD_Control:
            return "Control";
        case MIDIDivision::MD_Pedal:
            return "Pedal";
        case MIDIDivision::MD_Solo:
            return "Solo";
        case MIDIDivision::MD_Swell:
            return "Swell";
    }
    return "";
}

class ResetParser: public CommandParser
{
    public:
        ResetParser() {}

        virtual CmdErrorCode startCommand(const char* cmd) {
            Control.resetAll();
            return CmdErrorCode::CmdOK;
        }
};

class StatusParser: public CommandParser
{
    public:
        StatusParser() {}

        virtual CmdErrorCode startCommand(const char* cmd) {
            // Setup mngr for printing next status
            PrintNextStatus = PRINT_ALL;

            // Trigger a status read
            Control.readAll();
            return CmdErrorCode::CmdOK;
        }
};

class ChannelParser: public CommandParser
{
    private:
        MIDIDivision mDivision;

    public:
        ChannelParser() {}

        virtual void printArguments() { 
            Serial.print("<division> <channel>");
        }

        virtual CmdErrorCode startCommand(const char* cmd) {
            return CmdErrorCode::CmdNextArgument;
        }

        virtual CmdErrorCode parseNextArgument(int argNo, const char* arg) {
            switch (argNo) {
                case 0:
                    if (parseDivision(arg, mDivision)) {
                        return CmdErrorCode::CmdNextArgument;
                    } else {
                        return CmdErrorCode::CmdError;
                    }
                case 1:
                    int channel;
                    if (parseInteger(arg, channel, 0, 15)) {

                        StateMngr.setDivisionChannel(mDivision, channel);

                        return CmdErrorCode::CmdOK;
                    } else {
                        return CmdErrorCode::CmdError;
                    }                    
                default:
                    return CmdErrorCode::CmdError;
            }
        }
};

class CalibrationParser: public CommandParser
{
    public:
        CalibrationParser() {}

        virtual void printArguments() { 
            Serial.print("analog|kb1|kb2");
        }

        virtual CmdErrorCode startCommand(const char* cmd) {
            return CmdErrorCode::CmdNextArgument;
        }

        virtual CmdErrorCode parseNextArgument(int argNo, const char* arg) {
            if (strcmp(arg, "analog") == 0) {
                Control.startCalibrateAnalogInputs();
                return CmdErrorCode::CmdOK;
            }
            if (strcmp(arg, "kb1") == 0) {
                Control.trainKeyboard(0);
                return CmdErrorCode::CmdOK;
            }
            if (strcmp(arg, "kb2") == 0) {
                Control.trainKeyboard(1);
                return CmdErrorCode::CmdOK;
            }
            return CmdErrorCode::CmdInvalidArgument;
        }
};

void onKeyboardStatus(uint8_t channel1, uint8_t channel2, bool training)
{
    if (PrintNextStatus & PRINT_KEYBOARD) {
        PrintNextStatus &= ~PRINT_KEYBOARD;
        Serial.printf("Keyboard: chan1=%d chan2=%d", channel1, channel2);
        if (training) {
            Serial.print(" training");
        }
        Serial.println();
    }

    if (training & !KeyboardIsLearning) {
        Serial.println("Keyboard: training started!");
    }    
    if (!training & KeyboardIsLearning) {
        Serial.println("Keyboard: training complete!");
    }

    KeyboardIsLearning = training;
}

void onTechnicsStatus(uint8_t channel, uint16_t wheel)
{
    if (PrintNextStatus & PRINT_TECHNICS) {
        PrintNextStatus &= ~PRINT_TECHNICS;
        Serial.printf("Technics: chan=%d wheel=%d", channel, wheel);
        Serial.println();
    }

    // Note: Wheel is sent as MIDI bend control event
}

void onToeStudStatus(uint8_t channel, uint16_t crescendo, uint16_t swell, uint16_t choir)
{
    if (PrintNextStatus & PRINT_TOESTUD) {
        PrintNextStatus &= ~PRINT_TOESTUD;
        Serial.printf("ToeStuds: chan=%d cresendo=%d swell=%d choir=%d", channel, crescendo, swell, choir);
        Serial.println();
    }

    // TODO if mode is I2C (not MIDI), process I2C pedals

}

void onPedalStatus(uint8_t channel, uint8_t ledIntensity)
{
    if (PrintNextStatus & PRINT_PEDAL) {
        PrintNextStatus &= ~PRINT_PEDAL;
        Serial.printf("Pedal: chan=%d leds=%d", channel, ledIntensity);
        Serial.println();
    }
}

void onPistonPress(MIDIDivision division, uint8_t button, bool longPress)
{
    Serial.printf("Btn: %s.%d", divisionName(division), button);
    if (longPress) {
        Serial.print(" long");
    }
    Serial.println();

    // TODO handle button presses
    
}

void setup()
{
    Cmdline.addCommand("reset", new ResetParser());
    Cmdline.addCommand("status", new StatusParser());
    Cmdline.addCommand("channel", new ChannelParser());
    Cmdline.addCommand("calibrate", new CalibrationParser());

    Control.setKeyboardStatusCallback(onKeyboardStatus);
    Control.setTechnicsStatusCallback(onTechnicsStatus);
    Control.setToeStudStatusCallback(onToeStudStatus);
    Control.setPedalStatusCallback(onPedalStatus);
    Control.setPistonPressCallback(onPistonPress);

    Cmdline.begin();
    Control.begin();
    MIDI.begin();
    StateMngr.begin();
}

void loop() {
    Cmdline.loop();
    Control.loop();
    MIDI.loop();
    StateMngr.loop();
}
