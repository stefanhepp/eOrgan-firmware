/**
 * @author Stefan Hepp (stefan@stefant.org)
 * 
 * Main program of the central MIDI controller.
 */

#include <Arduino.h>

#include "MIDIRouter.h"
#include "ConfigController.h"

// Serial6: UART Panel


MIDIRouter MIDI;
ConfigController Config;

void setup() {
    // Serial over USB. Speed is ignored.
    Serial.begin(9600);

    Config.begin();
    MIDI.begin();
}

void loop() {
    Serial.println("Sending");

    delay(1000);

    Config.loop();
    MIDI.loop();
}
