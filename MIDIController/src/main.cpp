/**
 * @author Stefan Hepp (stefan@stefant.org)
 * 
 * Main program of the central MIDI controller.
 */

#include <Arduino.h>
#include <Wire.h>
#include <MIDI.h>
//#include <USB-MIDI.h>
#include <TCA9548.h>

// Serial6: UART Panel

// MIDI Devices
MIDI_CREATE_INSTANCE(HardwareSerial, Serial7,    MIDI1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1,    MIDI2);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3,    MIDI3);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial4,    MIDI4);
// TXD8 is not connected
MIDI_CREATE_INSTANCE(HardwareSerial, Serial8,    MIDIPedal);
// TXD2 is on I2S audio header
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2,    MIDIKbd);
// TXD3 is shared with SPDIF_OUT, on I2S audio header
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3,    MIDITechnics);

//USBMIDI_CREATE_DEFAULT_INSTANCE();

// I2C Multiplexer:
TCA9548 MP(0x70);

// I2C Multiplexer Ports
static const int I2C_PORT_PANEL     = 0;
static const int I2C_PORT_I2C1      = 1;
static const int I2C_PORT_STOPLEFT  = 2;
static const int I2C_PORT_STOPRIGHT = 3;
static const int I2C_PORT_TOESTUDS  = 4;
static const int I2C_PORT_TECHNICS  = 5;
static const int I2C_PORT_KEYBOARD  = 6;
static const int I2C_PORT_I2C2      = 7;


void setup() {
    // Serial over USB. Speed is ignored.
    Serial.begin(9600);

    MIDI1.begin(1);
    MIDI2.begin(2);
    MIDI3.begin(3);
    MIDI4.begin(4);
    MIDIPedal.begin(1);
    MIDIKbd.begin(4);
    MIDITechnics.begin(2);

    MIDI1.turnThruOff();
    MIDI2.turnThruOff();
    MIDI3.turnThruOff();
    MIDI4.turnThruOff();
    MIDIPedal.turnThruOff();
    MIDIKbd.turnThruOff();
    MIDITechnics.turnThruOff();

    Wire.begin();
    MP.begin();
}

void loop() {
    Serial.println("Sending");

    delay(1000);
}
