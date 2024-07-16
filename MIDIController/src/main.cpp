/**
 * @author Stefan Hepp (stefan@stefant.org)
 * 
 * Main program of the central MIDI controller.
 */

#include <Arduino.h>

#include "CmdlineParser.h"
#include "MIDIRouter.h"
#include "ConfigController.h"

// Serial6: UART Panel

CmdlineParser Cmdline;
MIDIRouter MIDI;
ConfigController Config;

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
                            case MIDIDivision::Pedal:
                                Config.setPedalChannel(channel);
                                break;
                            case MIDIDivision::Choir:
                                Config.setTechnicsChannel(channel);
                                break;
                            case MIDIDivision::Great:

                                break;
                            case MIDIDivision::Swell:

                                break;
                            case MIDIDivision::Solo:

                                break;
                            case MIDIDivision::Control:

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

        virtual CmdErrorCode startCommand(const char* cmd) {
            return CmdErrorCode::CmdNextArgument;
        }

        virtual CmdErrorCode parseNextArgument(int argNo, const char* arg) {
            switch (argNo) {
                case 0:
                    MIDIDivision division;
                    if (parseDivision(arg, division)) {
                        switch (division) {
                            case MIDIDivision::Choir:
                                
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


    Cmdline.begin();
    Config.begin();
    MIDI.begin();
}

void loop() {
    Cmdline.loop();
    Config.loop();
    MIDI.loop();
}
