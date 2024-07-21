/*
 * @project     MIDIController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Cmdline parser implementation
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "CommandLine.h"

#include <Arduino.h>

#include <inttypes.h>
#include <cstring>
#include <string>

#include <common_config.h>

bool CommandParser::parseDivision(const char* arg, MIDIDivision &division)
{
    if (strcmp(arg, "pedal")) {
        division = MIDIDivision::MD_Pedal;
        return true;
    }
    if (strcmp(arg, "choir")) {
        division = MIDIDivision::MD_Choir;
        return true;
    }
    if (strcmp(arg, "Swell")) {
        division = MIDIDivision::MD_Swell;
        return true;
    }
    if (strcmp(arg, "solo")) {
        division = MIDIDivision::MD_Solo;
        return true;
    }
    if (strcmp(arg, "control")) {
        division = MIDIDivision::MD_Control;
        return true;
    }
    return false;
}

bool CommandParser::parseInteger(const char* arg, int &value, int minValue, int maxValue)
{
    const char* c = arg;
    while (*c != '\0') {
        if (*c < '0' || *c > '9') {
            return false;
        }
        c++;
    }

    value = std::stoi(arg);

    if (value < minValue || value > maxValue) {
        return false;
    }

    // conversion and range check successful
    return true;
}

CommandLine::CommandLine()
{
}

void CommandLine::addCommand(const char* cmd, CommandParser* parser)
{
    if (mNumCommands >= MAX_PARSERS) {
        return;
    }
    mCommands[mNumCommands] = cmd;
    mParsers[mNumCommands] = parser;
    mNumCommands++;
}

void CommandLine::printCommandHelp(int cmd)
{
    Serial.print(mCommands[cmd]);
    Serial.print(' ');
    mParsers[cmd]->printArguments();
    Serial.println();
}

void CommandLine::printHelp()
{
    for (int i = 0; i < mNumCommands; i++) {
        printCommandHelp(i);
    }
}

void CommandLine::handleRetCode(CmdErrorCode ret)
{
    switch (ret) {
        case CmdErrorCode::CmdOK:
            Serial.println("OK");

            // no arguments expected
            mExpectCommand = false;
            break;
        case CmdErrorCode::CmdNextArgument:
            mExpectCommand = true;
            break;
        case CmdErrorCode::CmdInvalidArgument:
            Serial.print("Error parsing command: ");
            Serial.println(mToken);

            mExpectCommand = false;
            mCurrentCommand = -1;
            break;
        case CmdErrorCode::CmdError:
            Serial.println("FAIL");

            mExpectCommand = false;
            mCurrentCommand = -1;
            break;
    }
}

void CommandLine::selectCommand()
{
    for (int i = 0; i < mNumCommands; i++) {
        if (strcmp(mToken, mCommands[i]) == 0) {
            // Found a valid command
            mCurrentCommand = i;
            mArgumentNo = 0;

            CmdErrorCode ret = mParsers[i]->startCommand(mToken);

            // OK, continue, fail?
            handleRetCode(ret);

            // found a command, abort
            return;
        }
    }
    
    // only reached if no command matched
    Serial.println("Invalid command! See 'help'.");

    mExpectCommand = false;
    mCurrentCommand = -1;
}

void CommandLine::processArgument()
{
    if (mCurrentCommand == -1) {
        return;
    }
    
    CmdErrorCode ret = mParsers[mCurrentCommand]->parseNextArgument(mArgumentNo++, mToken);

    // OK, continue, fail?
    handleRetCode(ret);
}

void CommandLine::abortCommand()
{
    if (mCurrentCommand != -1) {
        mParsers[mCurrentCommand]->resetCommand();
        mExpectCommand = false;
        mCurrentCommand = -1;
    }
}

void CommandLine::processToken(bool eol)
{
    // zero-terminate token
    if (mTokenLength < MAX_TOKEN_LENGTH) {
        mToken[mTokenLength++] = '\0';
    } else {
        // Forcefully terminate at the end. Should never be reached.
        mToken[MAX_TOKEN_LENGTH-1] = '\0';
    }

    if (mExpectCommand) {
        if (mCurrentCommand == -1) {
            // We are starting a new command
            selectCommand();
        } else {
            // Got another argument
            processArgument();
        }

    } else {
        if (mCurrentCommand != -1) {
            // Received another argument that the command was not expecting
            Serial.print("Ignoring unexpected argument: ");
            Serial.print(mToken);
            Serial.println();

            mCurrentCommand = -1;

        } else {
            // Silently ignore any other arguments. Error was already reported.
        }
    }
    if (eol) {
        if (mExpectCommand && mCurrentCommand != -1) {
            // We are still waiting for arguments, but end of line. Terminate current command.
            Serial.print("Missing arguments: ");
            printCommandHelp(mCurrentCommand);

            abortCommand();
        }
        mExpectCommand = true;
        mCurrentCommand = -1;
    }
}

void CommandLine::begin()
{
    // Serial over USB. Speed is ignored.
    Serial.begin(9600);
}

void CommandLine::loop()
{
    while (Serial.available()) {
        char c = Serial.read();

        switch (c) {
            case '\r':
                break;
            case ' ':
            case '\n':
                if (mTokenLength > 0) {
                    processToken(c == '\n');
                }
                break;
            default:
                // We leave one byte space in the buffer for zero-terminiation
                if (mTokenLength < MAX_TOKEN_LENGTH - 1) {
                    mToken[mTokenLength++] = c;
                } else {
                    // raise error?
                }
                break;
        }
    }
}
