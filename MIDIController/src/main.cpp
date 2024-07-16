/**
 * @author Stefan Hepp (stefan@stefant.org)
 * 
 * Main program of the central MIDI controller.
 */

#include <Arduino.h>

#include "Cmdline.h"
#include "MIDIRouter.h"
#include "ConfigController.h"

// Serial6: UART Panel

Cmdline cmdline;
MIDIRouter MIDI;
ConfigController Config;

void setup() {

    Config.begin();
    MIDI.begin();
}

void loop() {
    Serial.println("Sending");

    delay(1000);

    Config.loop();
    MIDI.loop();
}
