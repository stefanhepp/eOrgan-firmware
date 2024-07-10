/**
 * Author: Stefan Hepp (stefan@stefant.org)
 *
 **/

#include <MiniMIDI.h>

#include <avr/io.h>
#include <avr/interrupt.h>


static const uint8_t BUFFER_SIZE = 64;

static uint8_t Buffer[BUFFER_SIZE];
static uint8_t BufferLength = 0;
static uint8_t BufferPos = 0;

static void writeByte(uint8_t data)
{
    uint8_t head = (BufferPos + BufferLength) % BUFFER_SIZE;
    Buffer[head] = data;
    BufferLength++;
}

MiniMIDI::MiniMIDI()
: mChannel(0), mThruMode(ThruMode::Off)
{
}

void MiniMIDI::turnThruOff()
{
    mThruMode = ThruMode::Off;
}

void MiniMIDI::turnThruOn(ThruMode mode)
{
    mThruMode = mode;
}

void MiniMIDI::setInputChannel(uint8_t channel)
{
    mChannel = channel;
}

void MiniMIDI::writeMessage2(uint8_t opcode, uint8_t channel, uint8_t data1)
{
    cli();
    if (BufferLength + 2 <= BUFFER_SIZE) {
        if (channel == 0xFF) {
            channel = mChannel;
        }
        writeByte(opcode | (channel & 0x0F));
        writeByte(data1 & 0x7F);
    }
    sei();
}

void MiniMIDI::writeMessage3(uint8_t opcode, uint8_t channel, uint8_t data1, uint8_t data2)
{
    cli();
    if (BufferLength + 3 <= BUFFER_SIZE) {
        if (channel == 0xFF) {
            channel = mChannel;
        }
        writeByte(opcode | (channel & 0x0F));
        writeByte(data1 & 0x7F);
        writeByte(data2 & 0x7F);
    }
    sei();
}

void MiniMIDI::begin(uint8_t channel)
{
    mChannel = channel;

    // Setup UART for sending at 31250 baud, 8N1


}

void MiniMIDI::sendNoteOn(uint8_t note,  uint8_t velocity, uint8_t channel)
{
    writeMessage3(0x90, channel, note, velocity);
}

void MiniMIDI::sendNoteOff(uint8_t note,  uint8_t velocity, uint8_t channel)
{
    writeMessage3(0x80, channel, note, velocity);
}

void MiniMIDI::sendControlChange(uint8_t controller, uint8_t value, uint8_t channel)
{
    writeMessage3(0xB0, channel, controller, value);
}

void MiniMIDI::sendPitchBend(int pitch, uint8_t channel)
{
    writeMessage3(0xE0, channel, (pitch >> 7), pitch & 0x7F);
}

void MiniMIDI::sendProgramChange(uint8_t program, uint8_t channel)
{
    writeMessage2(0xC0, channel, program);
}

void MiniMIDI::sendAllSoundOff(uint8_t channel)
{
    sendControlChange(120, 0);
}

void MiniMIDI::sendAllNotesOff(uint8_t channel)
{
    sendControlChange(123, 0);
}
