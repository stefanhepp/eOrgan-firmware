/**
 * @author Stefan Hepp (stefan@stefant.org)
 * 
 * Main program of the central MIDI controller.
 */

#include <Arduino.h>

#include "CommandLine.h"
#include "MIDIRouter.h"
#include "ControllerDriver.h"

// Serial6: UART Panel

CommandLine Cmdline;
MIDIRouter MIDI;
ControllerDriver Config;

class ResetParser: public CommandParser
{
    public:
        ResetParser() {}

        virtual CmdErrorCode startCommand(const char* cmd) {
            Config.resetAll();
            return CmdErrorCode::CmdOK;
        }
};

class StatusParser: public CommandParser
{
    public:
        StatusParser() {}

        virtual CmdErrorCode startCommand(const char* cmd) {
            Config.readAll();
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
                        switch (mDivision) {
                            case MIDIDivision::MD_Pedal:
                                Config.setPedalChannel(channel);
                                break;
                            case MIDIDivision::MD_Choir:
                                Config.setTechnicsChannel(channel);
                                break;
                            case MIDIDivision::MD_Great:

                                break;
                            case MIDIDivision::MD_Swell:

                                break;
                            case MIDIDivision::MD_Solo:

                                break;
                            case MIDIDivision::MD_Control:

                                break;
                        }
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
            Serial.print("<division>");
        }

        virtual CmdErrorCode startCommand(const char* cmd) {
            return CmdErrorCode::CmdNextArgument;
        }

        virtual CmdErrorCode parseNextArgument(int argNo, const char* arg) {
            switch (argNo) {
                case 0:
                    MIDIDivision division;
                    if (parseDivision(arg, division)) {
                        switch (division) {
                            case MIDIDivision::MD_Choir:
                                
                                break;
                            default:
                                return CmdErrorCode::CmdError;
                        }

                        return CmdErrorCode::CmdOK;
                    } else {
                        return CmdErrorCode::CmdError;
                    }
                default:
                    return CmdErrorCode::CmdError;
            }
        }
};

void setup()
{
    Cmdline.addCommand("reset", new ResetParser());
    Cmdline.addCommand("status", new StatusParser());
    Cmdline.addCommand("channel", new ChannelParser());
    Cmdline.addCommand("calibrate", new CalibrationParser());

    Cmdline.begin();
    Config.begin();
    MIDI.begin();
}

void loop() {
    Cmdline.loop();
    Config.loop();
    MIDI.loop();
}
