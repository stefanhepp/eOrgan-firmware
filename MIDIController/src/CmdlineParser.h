/**
 * @project     MidiController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Commandline parser
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#pragma once

#include <inttypes.h>
#include <common_config.h>

#include <MIDI.h>

enum CmdErrorCode
{
    CmdOK,
    CmdNextArgument,
    CmdError
};

class CommandParser
{
    protected:
        bool parseDivision(const char* arg, MIDIDivision &division);

        bool parseInteger(const char* arg, int &value, int minValue, int maxValue);

    public:
        /**
         * Start parsing a new command "cmd".
         * 
         * @return True if there is another argument expected.
         */
        virtual CmdErrorCode startCommand(const char* cmd) = 0;

        virtual CmdErrorCode parseNextArgument(int argNo, const char* arg) { return CmdErrorCode::CmdError; }
};

static const int MAX_PARSERS = 8;

class CmdlineParser
{
    private:
        CommandParser* mParsers[MAX_PARSERS];
        const char* mCommands[MAX_PARSERS];
        int mNumCommands = 0;

    public:
        explicit CmdlineParser();

        void addCommand(const char* cmd, CommandParser *parser);

        void begin();

        void loop();
};