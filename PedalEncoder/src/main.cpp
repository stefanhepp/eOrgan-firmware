#include <Arduino.h>
#include <MIDI.h>
#include <MegaWire.h>

#include <common.h>

// Create device driver instances
MIDI_CREATE_DEFAULT_INSTANCE();

MegaWire Wire;

void setup() {
  MIDI.begin(1);
  Wire.begin(I2C_ADDR_PEDAL);
}

void loop() {
  MIDI.sendNoteOn(42, 127, 1);
  delay(1000);
  digitalWrite(10, HIGH);
  digitalRead(14);
  digitalWrite(11, HIGH);
  digitalRead(11);
  digitalWrite(14, LOW);
  MIDI.sendNoteOff(42, 0, 1);
  delay(1000);
}
