#include <Arduino.h>
#include <Wire.h>
#include <MIDI.h>
//#include <USB-MIDI.h>

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1,    midiA);
//USBMIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  midiA.begin(1);
  Wire.begin(8);
}

void loop() {
  
}
