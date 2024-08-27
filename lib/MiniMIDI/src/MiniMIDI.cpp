/**
 * Author: Stefan Hepp (stefan@stefant.org)
 *
 **/

#include <MiniMIDI.h>

#include <avr/io.h>
#include <avr/interrupt.h>


static const uint8_t RX_BUFFER_SIZE = 4;
static const uint8_t TX_BUFFER_SIZE = 64;

static uint8_t RxBuffer[RX_BUFFER_SIZE];
static uint8_t RxBufferLength = 0;

static uint8_t RxExpectedLength = 0;

static ThruMode MidiThruMode = ThruMode::Off;
static uint8_t MidiChannel = 0;

static uint8_t TxBuffer[TX_BUFFER_SIZE];
static uint8_t TxBufferLength = 0;
static uint8_t TxBufferPos = 0;

static MiniMIDI* MIDI = nullptr;

/**
 * Get the number of data bytes for a particular MIDI status byte
 */
static uint8_t midiMessageLength(uint8_t status) {
    if ((status & 0xF8) == 0xF8) {
        // System Real-time message
        return 0;
    }
    uint8_t opcode = status & 0xF0;
    if (opcode == 0xF0) {
        if (status == 0xF2) {
            // Song Position Pointer
            return 2;
        }
        if (status == 0xF1 || status == 0xF3) {
            // MIDI Time Quarter Code, Song Select
            return 1;
        }
        if (status == 0XF0) {
            // System Exclusive.. unknown size.
            return 0;
        }
        // Any other System message is 0 bytes;
        return 0;
    }
    if (opcode == 0xC0 || opcode == 0xD0) {
        // Program Change or Channel Pressure
        return 1;
    }
    // Any other voice message is 2 bytes
    return 2;
}

static void writeByte(uint8_t data)
{
    uint8_t head = (TxBufferPos + TxBufferLength) % TX_BUFFER_SIZE;
    TxBuffer[head] = data;
    TxBufferLength++;

    // Make sure to trigger an interrupt to send the queued data
    UCSR0B |= (1<<UDRIE0);
}

/**
 * We finished receiving a message in the receive buffer, process it.
 */
static void receivedMessage() 
{    
    uint8_t channel = (RxBuffer[0] & 0x0F) + 1;
    uint8_t opcode = RxBuffer[0] & 0xF0;
    bool sysExMsg = false;
    if (opcode == 0xF0) {
        // System message
        opcode = RxBuffer[0];
        channel = 0;
        sysExMsg = true;
    }

    bool forward = (MidiThruMode == ThruMode::Full)
                // Forward depending on the current channel
                || (MidiThruMode == ThruMode::DifferentChannel && !sysExMsg && channel != MidiChannel)                
                || (MidiThruMode == ThruMode::SameChannel      && !sysExMsg && channel == MidiChannel)
                // System real-time messages are forwarded in all modes
                || (MidiThruMode != ThruMode::Off              && sysExMsg);

    // Forward the whole message to the transmit buffer
    if (forward) {
        for (uint8_t i = 0; i < RxBufferLength; i++) {
            writeByte(RxBuffer[i]);
        }
    }

    // Call receive handler
    MIDIMessage msg;
    msg.Opcode = opcode;
    msg.Channel = channel;
    msg.Data1 = (RxBufferLength > 1) ? RxBuffer[1] : 0x00;
    msg.Data1 = (RxBufferLength > 2) ? RxBuffer[2] : 0x00;

    if (MIDI != nullptr) {
        MIDI->_processRxMessage(msg);
    }

    // clear the buffer
    RxBufferLength = 0;
    RxExpectedLength = 0;
}

MiniMIDI::MiniMIDI()
: mChannel(0), mReceiveCallback(nullptr)
{
    // Register this instance for callbacks
    MIDI = this;
}

void MiniMIDI::_processRxMessage(const MIDIMessage &msg)
{
    if (mReceiveCallback != nullptr) {
        mReceiveCallback(msg);
    }
}

void MiniMIDI::setReceiveCallback(MIDIReceiveCallback callback)
{
    mReceiveCallback = callback;
}

void MiniMIDI::turnThruOff()
{
    MidiThruMode = ThruMode::Off;
}

void MiniMIDI::turnThruOn(ThruMode mode)
{
    MidiThruMode = mode;
}

void MiniMIDI::setInputChannel(uint8_t channel)
{
    mChannel = channel;
    MidiChannel = channel;
}

uint8_t MiniMIDI::getStatusByte(uint8_t opcode, uint8_t channel) const
{
    if ( (opcode & 0xF0) == 0xF0 ) {
        // SysEx message
        return opcode;
    } else {
        if (channel == 0xFF) {
            channel = mChannel;
        }
        // Channel message
        return opcode | ((channel & 0x0F) - 1);
    }
}

void MiniMIDI::writeMessage2(uint8_t opcode, uint8_t channel, uint8_t data1)
{
    cli();
    if (TxBufferLength + 2 <= TX_BUFFER_SIZE) {
        writeByte(getStatusByte(opcode, channel));
        writeByte(data1 & 0x7F);
    }
    sei();
}

void MiniMIDI::writeMessage3(uint8_t opcode, uint8_t channel, uint8_t data1, uint8_t data2)
{
    cli();
    if (TxBufferLength + 3 <= TX_BUFFER_SIZE) {
        writeByte(getStatusByte(opcode, channel));
        writeByte(data1 & 0x7F);
        writeByte(data2 & 0x7F);
    }
    sei();
}

void MiniMIDI::begin(uint8_t channel)
{
    setInputChannel(channel);

    // Setup UART for sending at 31250 baud (at 8Mhz system clock)
    UBRR0H = 0;
    // for 8Mhz system clock:
    UBRR0L = 15; 
    // For 1Mhz default system clock:
    //UBRR0L = 1;

    // Format 8N1, async mode
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Enabler RX and TX, enable interrupts
    UCSR0B = (1 << RXCIE0)|(1 << RXEN0)|(1 << TXEN0);
}

bool MiniMIDI::sending() const
{
    // Check if there is any data still in the buffer or UART being sent.
    return TxBufferLength > 0 || !(UCSR0A & (1<<UDRE0));
}

void MiniMIDI::sendNoteOn(uint8_t note,  uint8_t velocity, uint8_t channel)
{
    writeMessage3(0x90, channel, note, velocity);
}

void MiniMIDI::sendNoteOff(uint8_t note,  uint8_t velocity, uint8_t channel)
{
    writeMessage3(0x80, channel, note, velocity);
}

void MiniMIDI::sendControlChange(MidiControlChangeNumber controller, uint8_t value, uint8_t channel)
{
    writeMessage3(0xB0, channel, controller, value);
}

void MiniMIDI::sendPitchBend(int pitch, uint8_t channel)
{
    writeMessage3(0xE0, channel, pitch & 0x7F, (pitch >> 7) & 0x7F);
}

void MiniMIDI::sendProgramChange(uint8_t program, uint8_t channel)
{
    writeMessage2(0xC0, channel, program);
}

void MiniMIDI::sendAllSoundOff(uint8_t channel)
{
    sendControlChange(MidiControlChangeNumber::AllSoundOff, 0);
}

void MiniMIDI::sendAllNotesOff(uint8_t channel)
{
    sendControlChange(MidiControlChangeNumber::AllNotesOff, 0);
}

ISR(USART_RX_vect)
{

    if ((UCSR0A & (1<<DOR0)) || (UCSR0A & (1<<FE0)) ) {
        // Got a frame error or data overrun
        // Clear the data and reset the receive message
        (void)UDR0;
        RxExpectedLength = 0;
        RxBufferLength = 0;
        return;
    }

    uint8_t data = UDR0;

    if (data == 0xF0) {
        // Ignore System Exlusive messages here for now. Ignore any data.
        RxExpectedLength = 0;
    }
    else if (data & 0x80) {
        // we received a new message. If there is anything still in the receive buffer, ignore it.
        RxBuffer[0] = data;
        RxBufferLength = 1;

        // check how many data bytes are expected
        RxExpectedLength = midiMessageLength(data);

        // single-byte message, then process immediately.
        if (RxExpectedLength == 0) {
            receivedMessage();
        }
    }
    else if (RxExpectedLength > 0) {
        // got additional data byte
        RxBuffer[RxBufferLength++] = data;
        RxExpectedLength--;

        // Full message received?
        if (RxExpectedLength == 0) {
            receivedMessage();
        }
    }
}

ISR(USART_UDRE_vect)
{
    if (TxBufferLength > 0) {
        // enqueue the next byte for transmission
        UDR0 = TxBuffer[TxBufferPos];

        // Move buffer start to next byte
        TxBufferLength--;
        TxBufferPos = (TxBufferPos + 1) % TX_BUFFER_SIZE;
    } else {
        // Buffer empty, stop queuing data
        UCSR0B &= ~(1<<UDRIE0);
    }
}