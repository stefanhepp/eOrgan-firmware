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
 * Enum from midi_Defs.h from Arduino MIDI Library from Francois Best. 
 */
enum MidiControlChangeNumber: uint8_t
{
    // High resolution Continuous Controllers MSB (+32 for LSB) ----------------
    BankSelect                  = 0,
    ModulationWheel             = 1,
    BreathController            = 2,
    // CC3 undefined
    FootController              = 4,
    PortamentoTime              = 5,
    DataEntryMSB                = 6,
    ChannelVolume               = 7,
    Balance                     = 8,
    // CC9 undefined
    Pan                         = 10,
    ExpressionController        = 11,
    EffectControl1              = 12,
    EffectControl2              = 13,
    // CC14 undefined
    // CC15 undefined
    GeneralPurposeController1   = 16,
    GeneralPurposeController2   = 17,
    GeneralPurposeController3   = 18,
    GeneralPurposeController4   = 19,

    DataEntryLSB                = 38,

    // Switches ----------------------------------------------------------------
    Sustain                     = 64,
    Portamento                  = 65,
    Sostenuto                   = 66,
    SoftPedal                   = 67,
    Legato                      = 68,
    Hold                        = 69,

    // Low resolution continuous controllers -----------------------------------
    SoundController1            = 70,   ///< Synth: Sound Variation   FX: Exciter On/Off
    SoundController2            = 71,   ///< Synth: Harmonic Content  FX: Compressor On/Off
    SoundController3            = 72,   ///< Synth: Release Time      FX: Distortion On/Off
    SoundController4            = 73,   ///< Synth: Attack Time       FX: EQ On/Off
    SoundController5            = 74,   ///< Synth: Brightness        FX: Expander On/Off
    SoundController6            = 75,   ///< Synth: Decay Time        FX: Reverb On/Off
    SoundController7            = 76,   ///< Synth: Vibrato Rate      FX: Delay On/Off
    SoundController8            = 77,   ///< Synth: Vibrato Depth     FX: Pitch Transpose On/Off
    SoundController9            = 78,   ///< Synth: Vibrato Delay     FX: Flange/Chorus On/Off
    SoundController10           = 79,   ///< Synth: Undefined         FX: Special Effects On/Off
    GeneralPurposeController5   = 80,
    GeneralPurposeController6   = 81,
    GeneralPurposeController7   = 82,
    GeneralPurposeController8   = 83,
    PortamentoControl           = 84,
    // CC85 to CC90 undefined
    Effects1                    = 91,   ///< Reverb send level
    Effects2                    = 92,   ///< Tremolo depth
    Effects3                    = 93,   ///< Chorus send level
    Effects4                    = 94,   ///< Celeste depth
    Effects5                    = 95,   ///< Phaser depth
    DataIncrement               = 96,
    DataDecrement               = 97,
    NRPNLSB                     = 98,   ///< Non-Registered Parameter Number (LSB)
    NRPNMSB                     = 99,   ///< Non-Registered Parameter Number (MSB)
    RPNLSB                      = 100,  ///< Registered Parameter Number (LSB)
    RPNMSB                      = 101,  ///< Registered Parameter Number (MSB)

    // Channel Mode messages ---------------------------------------------------
    AllSoundOff                 = 120,
    ResetAllControllers         = 121,
    LocalControl                = 122,
    AllNotesOff                 = 123,
    OmniModeOff                 = 124,
    OmniModeOn                  = 125,
    MonoModeOn                  = 126,
    PolyModeOn                  = 127
};

typedef struct {
    uint8_t Opcode;

    uint8_t Channel;

    uint8_t Data1;

    uint8_t Data2;
} MIDIMessage;

using MIDIReceiveCallback = void (*)(const MIDIMessage &msg);

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

        MIDIReceiveCallback mReceiveCallback;

        /**
         * Get the MIDI status byte (first byte of message) from opcode and channel.
         */
        uint8_t getStatusByte(uint8_t opcode, uint8_t channel) const;

        void writeMessage2(uint8_t opcode, uint8_t channel, uint8_t data1);

        void writeMessage3(uint8_t opcode, uint8_t channel, uint8_t data1, uint8_t data2);

    public:
        /**
         * Protected function, callback for ISR handler.
         * 
         * Process a received MIDI message, read from ISR handler buffer.
         */
        void _processRxMessage(const MIDIMessage &msg);

    public:
        MiniMIDI();

        void setReceiveCallback(MIDIReceiveCallback callback);

        /**
         * Return true if there is data currently being sent.
         */
        bool sending() const;

        void turnThruOff();

        void turnThruOn(ThruMode mode);

        void setInputChannel(uint8_t channel);

        void begin(uint8_t channel);

        void sendNoteOn(uint8_t note,  uint8_t velocity, uint8_t channel = 0xFF);

        void sendNoteOff(uint8_t note,  uint8_t velocity, uint8_t channel = 0xFF);

        void sendControlChange(MidiControlChangeNumber controller, uint8_t value, uint8_t channel = 0xFF);

        /**
         * Send Pitch Bend as 14bit value, range 0 to 2^14-1. Center is 0x2000.
         */
        void sendPitchBend(int pitch, uint8_t channel = 0xFF);

        void sendProgramChange(uint8_t program, uint8_t channel = 0xFF);

        void sendAllSoundOff(uint8_t channel = 0xFF);

        void sendAllNotesOff(uint8_t channel = 0xFF);

};

