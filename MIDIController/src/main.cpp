/**
 * @author Stefan Hepp (stefan@stefant.org)
 * 
 * Main program of the central MIDI controller.
 */

#include <Arduino.h>

#include "AudioProcessor.h"
#include "CommandLine.h"
#include "MIDIRouter.h"
#include "ControllerDriver.h"
#include "OrganStateManager.h"

#ifdef TEENSY_DEBUG
  #include "TeensyDebug.h"
#endif

AudioProcessor Audio;
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

static int PrintI2C = 0;

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

class DebugParser: public CommandParser
{
    public:
        DebugParser() {}

        virtual void printArguments() { 
            Serial.print("off|on|i2c");
        }

        virtual CmdErrorCode startCommand(const char* cmd) {
            return CmdErrorCode::CmdNextArgument;
        }

        virtual CmdErrorCode parseNextArgument(int argNo, const char* arg) {
            if (strcmp("on", arg) == 0) {
                MIDI.echoMIDIMessages(true);
                PrintI2C = PRINT_ALL;
            } else if (strcmp("off", arg) == 0) {
                MIDI.echoMIDIMessages(false);
                PrintI2C = 0;
            } else if (strcmp("i2c", arg) == 0) {
                MIDI.echoMIDIMessages(false);
                PrintI2C = PRINT_ALL;
            } else {
                return CmdErrorCode::CmdInvalidArgument;
            }
            return CmdErrorCode::CmdOK;
        }
};

class StatusParser: public CommandParser
{
    public:
        StatusParser() {}

        virtual void printArguments() { 
            Serial.print("all|technics|keyboard|toestud|pedal");
        }

        virtual CmdErrorCode startCommand(const char* cmd) {
            return CmdErrorCode::CmdNextArgument;
        }

        virtual CmdErrorCode parseNextArgument(int argNo, const char* arg) {
            if (strcmp("all", arg) == 0) {
                // Setup mngr for printing next status
                PrintNextStatus = PRINT_ALL;

                // Trigger a status read
                Control.readAll();
            } else if (strcmp("keyboard", arg) == 0) {
                PrintNextStatus |= PRINT_KEYBOARD;

                Control.readStatusKeyboard();
            } else if (strcmp("technics", arg) == 0) {
                PrintNextStatus |= PRINT_TECHNICS;

                Control.readStatusTechnics();
            } else if (strcmp("pedal", arg) == 0 || strcmp("toestud", arg) == 0) {
                PrintNextStatus |= PRINT_PEDAL | PRINT_TOESTUD;

                Control.readStatusPedal();
            } else {
                return CmdErrorCode::CmdInvalidArgument;
            }

            return CmdErrorCode::CmdOK;
        }

        virtual CmdErrorCode completeCommand(bool expectArgument) {
            if (expectArgument) {
                // no argument given, read all
                PrintNextStatus = PRINT_ALL;

                // Trigger a status read
                Control.readAll();
            }
            Control.printIRQStatus();
            
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

class PedalLEDParser: public CommandParser
{
    public:
        PedalLEDParser() {}

        virtual void printArguments() { 
            Serial.print("led 0..15");
        }

        virtual CmdErrorCode startCommand(const char* cmd) {
            return CmdErrorCode::CmdNextArgument;
        }

        virtual CmdErrorCode parseNextArgument(int argNo, const char* arg) {
            int intensity;
            if (argNo == 0) {
                if (strcmp(arg, "led") == 0) {
                    return CmdErrorCode::CmdNextArgument;
                }
            }
            if (argNo == 1) {
                // in 'led' command
                if (parseInteger(arg, intensity, 0, 15)) {
                    Control.setPedalLEDIntensity(intensity);
                    return CmdErrorCode::CmdOK;
                }
            }
            return CmdErrorCode::CmdInvalidArgument;
        }
};

class LEDControlParser: public CommandParser
{
    private:
        int mLED;
        uint8_t mValues[3];

    public:
        LEDControlParser() {}

        virtual void printArguments() { 
            Serial.print("led 0..255 | rgb1|rbg2 <R> <G> <B>");
        }

        virtual CmdErrorCode startCommand(const char* cmd) {
            return CmdErrorCode::CmdNextArgument;
        }

        virtual CmdErrorCode parseNextArgument(int argNo, const char* arg) {
            int intensity;
            if (argNo == 0) {
                if (strcmp(arg, "rgb1") == 0) {
                    mLED = 0;
                    return CmdErrorCode::CmdNextArgument;
                }
                if (strcmp(arg, "rgb2") == 0) {
                    mLED = 1;
                    return CmdErrorCode::CmdNextArgument;
                }
                if (strcmp(arg, "led") == 0) {
                    mLED = 2;
                    return CmdErrorCode::CmdNextArgument;
                }
            }
            else if (argNo < 4) {
                // in 'led' command
                if (parseInteger(arg, intensity, 0, 255)) {
                    mValues[argNo - 1] = intensity;

                    if ( (mLED < 2 && argNo == 3) || mLED == 2) {
                        Control.setLEDControllerRGB(mLED, mValues);
                        return CmdErrorCode::CmdOK;
                    }

                    return CmdErrorCode::CmdNextArgument;
                }
            }
            return CmdErrorCode::CmdInvalidArgument;
        }
};

class ToeStudModeParser: public CommandParser
{

    public:
        ToeStudModeParser() {}

        virtual void printArguments() { 
            Serial.print("mode midi|i2c|both");
        }

        virtual CmdErrorCode startCommand(const char* cmd) {
            return CmdErrorCode::CmdNextArgument;
        }

        virtual CmdErrorCode parseNextArgument(int argNo, const char* arg) {
            if (argNo == 0) {
                if (strcmp(arg, "mode") == 0) {
                    return CmdErrorCode::CmdNextArgument;
                }
            }
            if (argNo == 1) {
                // In "mode" command
                if (strcmp(arg, "midi") == 0) {
                    Control.setToestudMode(ToeStudMode::TSM_MIDI);
                    return CmdErrorCode::CmdOK;
                }
                if (strcmp(arg, "i2c") == 0) {
                    Control.setToestudMode(ToeStudMode::TSM_I2C);
                    return CmdErrorCode::CmdOK;
                }
                if (strcmp(arg, "both") == 0) {
                    Control.setToestudMode(ToeStudMode::TSM_I2C | ToeStudMode::TSM_MIDI);
                    return CmdErrorCode::CmdOK;
                }
            }
            return CmdErrorCode::CmdInvalidArgument;
        }
};

void onKeyboardStatus(uint8_t channel1, uint8_t channel2, bool training, uint8_t lastKey)
{
    char noteName[4];

    if (PrintNextStatus & PRINT_KEYBOARD || PrintI2C & PRINT_KEYBOARD) {
        PrintNextStatus &= ~PRINT_KEYBOARD;
        Serial.printf("Keyboard: chan1=%d chan2=%d", channel1, channel2);
        if (training) {
            noteToString(lastKey, noteName);
            Serial.printf(" training; learned %s\n", noteName);
        }
        Serial.println();
    }

    if (training & !KeyboardIsLearning) {
        Serial.println("Keyboard: training started!");
    }
    if (training) {
            noteToString(lastKey, noteName);
            Serial.printf(" training; learned %s\n", noteName);
    }
    if (!training & KeyboardIsLearning) {
        Serial.println("Keyboard: training complete!");
    }

    KeyboardIsLearning = training;
}

void onTechnicsStatus(uint8_t channel, uint16_t wheel)
{
    if (PrintNextStatus & PRINT_TECHNICS || PrintI2C & PRINT_TECHNICS) {
        PrintNextStatus &= ~PRINT_TECHNICS;
        Serial.printf("Technics: chan=%d wheel=%d", channel, wheel);
        Serial.println();
    }

    // Note: Wheel is sent as MIDI bend control event
}

void onToeStudStatus(uint8_t channel, uint16_t crescendo, uint16_t swell, uint16_t choir)
{
    if (PrintNextStatus & PRINT_TOESTUD || PrintI2C & PRINT_TOESTUD) {
        PrintNextStatus &= ~PRINT_TOESTUD;
        Serial.printf("ToeStuds: chan=%d cresendo=%d swell=%d choir=%d", channel, crescendo, swell, choir);
        Serial.println();
    }

    // TODO if mode is I2C (not MIDI), process I2C pedals

}

void onPedalStatus(uint8_t channel, uint8_t ledIntensity)
{
    if (PrintNextStatus & PRINT_PEDAL || PrintI2C & PRINT_PEDAL) {
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

void onLEDControllerButton(uint8_t button, uint8_t value) {
    if (button >= 4 && value) {
        // Button 4 is a press button
        Serial.printf("LEDController Button %hhu pressed\n", button);
    } else {
        // Buttons 0..3 are switches
        Serial.printf("LEDController Switch %hhu %s\n", button, value ? "ON" : "OFF");
    }
}

void setup()
{
#ifdef TEENSY_DEBUG
    debug.begin(SerialUSB1);

    // stop on startup; if not, Teensy keeps running and you
    // have to set a breakpoint or use Ctrl-C.
    halt_cpu(); 
#endif

    Cmdline.addCommand("reset", new ResetParser());
    Cmdline.addCommand("debug", new DebugParser());
    Cmdline.addCommand("status", new StatusParser());
    Cmdline.addCommand("channel", new ChannelParser());
    Cmdline.addCommand("calibrate", new CalibrationParser());
    Cmdline.addCommand("pedal", new PedalLEDParser());
    Cmdline.addCommand("toestud", new ToeStudModeParser());
    Cmdline.addCommand("led", new LEDControlParser());

    Control.setKeyboardStatusCallback(onKeyboardStatus);
    Control.setTechnicsStatusCallback(onTechnicsStatus);
    Control.setToeStudStatusCallback(onToeStudStatus);
    Control.setPedalStatusCallback(onPedalStatus);
    Control.setPistonPressCallback(onPistonPress);
    Control.setLEDControllerCallback(onLEDControllerButton);

    Cmdline.begin();
    Control.begin();
    MIDI.begin();
    StateMngr.begin();
    Audio.begin();
}

void loop() {
    Cmdline.loop();
    Control.loop();
    MIDI.loop();
    StateMngr.loop();
}
