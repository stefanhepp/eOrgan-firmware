/**
 * Author: Stefan hepp (stefan@stefant.org)
 *
 * Minimalistic MIDI library implementation.
 **/

#pragma once

#include <inttypes.h>

enum ThruMode: uint8_t
{
    Off              = 0,
    Full             = 1,
    SameChannel      = 2,
    DifferentChannel = 3
};

/**
 * Implementation of the Wire interface for AtMega 8bit controllers.
 * 
 * Only supports a single hardware TWI interface.
 * Only supports a single instantiation of this class.
 */
class MiniMIDI
{
    private:
        uint8_t mChannel;

        uint8_t mThruMode;

        void writeMessage2(uint8_t opcode, uint8_t channel, uint8_t data1);

        void writeMessage3(uint8_t opcode, uint8_t channel, uint8_t data1, uint8_t data2);

    public:
        MiniMIDI();

        void turnThruOff();

        void turnThruOn(ThruMode mode);

        void setInputChannel(uint8_t channel);

        void begin(uint8_t channel);

        void sendNoteOn(uint8_t note,  uint8_t velocity, uint8_t channel = 0xFF);

        void sendNoteOff(uint8_t note,  uint8_t velocity, uint8_t channel = 0xFF);

        void sendControlChange(uint8_t controller, uint8_t value, uint8_t channel = 0xFF);

        /**
         * Send Pitch Bend as 14bit value, range 0 to 2^14-1. Center is 0x2000.
         */
        void sendPitchBend(int pitch, uint8_t channel = 0xFF);

        void sendProgramChange(uint8_t program, uint8_t channel = 0xFF);

        void sendAllSoundOff(uint8_t channel = 0xFF);

        void sendAllNotesOff(uint8_t channel = 0xFF);

};

