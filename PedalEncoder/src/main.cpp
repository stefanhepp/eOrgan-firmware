/**
 * Author: Stefan Hepp (stefan@stefant.org)
 * 
 * Implementation of the pedalboard controller.
 * Reads pedal switches and generates MIDI messages.
 * Configurable either via config switch or I2C interface.
 * 
 * Pins:
 * - PB0-7: IN;  Pedal switches in
 * - PC0:   IN;  Config enable (HIGH = config mode, LOW = normal mode)
 * - PC3:   OUT; Signal I2C data ready to main controller (Open collect)
 * - PC4,5: I2C
 * - PC6:   In;  Reset (via ISP)
 * - PD0,1  UART
 * - PD3:   OUT; PWM for LED
 * - PD4-7: OUT; Select pedal rows 
 **/

#include <Arduino.h>
#include <MiniMIDI.h>
#include <MegaWire.h>

#include <common_config.h>

#include "config.h"
#include "Settings.h"
#include "Pedal.h"
#include "LED.h"

// Create device driver instances
//MIDI_CREATE_DEFAULT_INSTANCE();

MiniMIDI MIDI;
MegaWire Wire;

Settings   settings;
Pedal      pedal;
StatusLED  led;

static bool ConfigMode;

static uint8_t MIDIChannel;

static const uint8_t IRQ_CONFIG = 0;
static const uint8_t IRQ_CHANNEL = 1;

static uint16_t IRQFlags = 0x00;

static inline char getNote( char pedalKey ) 
{
    return MIDI_NOTE_C2 + pedalKey;
}

static void sendIRQ(uint8_t flag)
{
    IRQFlags |= (1<<flag);
    digitalWrite(PIN_INTERRUPT, HIGH);
}

static void clearIRQ(uint8_t flag)
{
    IRQFlags &= ~(1<<flag);
    if (IRQFlags == 0x00) {
        digitalWrite(PIN_INTERRUPT, LOW);
    }
}

/**
 * send notes off for all pressed notes.
 * @param force if non-zero, send a note-off controller msg on the current channel.
 **/
static void sendAllNotesOff()
{
    MIDI.sendControlChange(MidiControlChangeNumber::AllNotesOff, 0, MIDIChannel);
}

/**
 * start config mode.
 **/
static void startConfig(void)
{
    sendAllNotesOff();

    led.setMode(LEDModeBlink);
}

/**
 * cleanup after config button is released.
 **/
static void finalizeConfig(void)
{
    led.setMode(LEDModeNormal);
}

static void updateMIDIChannel(uint8_t channel) {
    MIDIChannel = channel;
    MIDI.setInputChannel(channel);
    settings.setMIDIChannel(channel);
}

static void updateLEDIntensity(uint8_t intensity) {
    led.setIntensity(intensity);
    settings.setLEDIntensity(intensity);
}

static void decreaseLEDIntensity() {
    uint8_t intensity = led.getIntensity();
    if (intensity > 0) {
        updateLEDIntensity(intensity - 1);
    }
}

static void increaseLEDIntensity() {
    uint8_t intensity = led.getIntensity();
    if (intensity < StatusLED::MAX_INTENSITY) {
        updateLEDIntensity(intensity + 1);
    }
}

static void resetEncoder(void)
{
    // disable all notes on current channel
    sendAllNotesOff();

    led.reset();
    settings.setLEDIntensity(led.getIntensity());

    // Reset channel
    updateMIDIChannel(MIDI_CHANNEL_PEDAL);
}

/**
 * process configuration input.
 **/
static void processConfigInput(uint8_t key, uint8_t velocity)
{
    if (velocity == 0) {
        return;
    }

    // lower 16 keys set channel
    if ( key < 16 ) {
        // set new channel
        MIDIChannel = key;
        MIDI.setInputChannel(MIDIChannel);
        settings.setMIDIChannel(MIDIChannel);
        sendIRQ(IRQ_CHANNEL);
	}
    // highest notes for config
	if ( key == 28 ) {
	    resetEncoder();
	} else if ( key == 24 ) {
	    decreaseLEDIntensity();
        sendIRQ(IRQ_CONFIG);
	} else if ( key == 26 ) {
	    increaseLEDIntensity();
        sendIRQ(IRQ_CONFIG);
    } else {
    	// TODO transpose
    }
}

static void processPedalInput(uint8_t key, uint8_t velocity)
{
    if (velocity > 0) {
        MIDI.sendNoteOn(  getNote(key), velocity, MIDIChannel );
    } else {
	    // this is a note off event (velocity is 0)
        MIDI.sendNoteOff( getNote(key), velocity, MIDIChannel );
    }
}


void onKeyChange(uint8_t key, uint8_t velocity) {
    if (ConfigMode) {
	    processConfigInput(key, velocity);
    } else {
	    processPedalInput(key, velocity);
    }
}

void i2cReceive(uint8_t length) {
    uint8_t cmd = Wire.read();
    uint8_t value;

    switch (cmd) {
        case I2C_CMD_RESET: 
            resetEncoder();
            break;
        case I2C_CMD_SET_CHANNEL:
            if (Wire.available()) {
                value = Wire.read();
                updateMIDIChannel(value);
            }
            break;
        case I2C_CMD_LED_INTENSITY:
            if (Wire.available()) {
                value = Wire.read();
                updateLEDIntensity(value);
            }
            break;
    }
}

void i2cRequest() {
    Wire.write(MIDIChannel);
    Wire.write(led.getIntensity());
    clearIRQ(IRQ_CHANNEL);
    clearIRQ(IRQ_CONFIG);
}

void setup() {
    // Enable pullups for unconnected pins
    pinMode(PIN_PC1, INPUT_PULLUP);
    pinMode(PIN_PC2, INPUT_PULLUP);
    pinMode(PIN_PC6, INPUT_PULLUP);
    pinMode(PIN_PD2, INPUT_PULLUP);

    // Set output pin modes
    // Set pin value first before turing on output mode, to prevent spurious signals
    digitalWrite(PIN_INTERRUPT, LOW);
    pinMode(PIN_INTERRUPT, OUTPUT);

    pinMode(PIN_CONFIG, INPUT);

    MIDIChannel = settings.getMIDIChannel();
    MIDI.turnThruOn(ThruMode::Full);
    MIDI.begin(MIDIChannel);

    led.begin(settings.getLEDIntensity());

    Wire.onReceive(i2cReceive);
    Wire.onRequest(i2cRequest);
    Wire.begin(Controller::MC_Pedal);

    pedal.setHandleKeyChange(onKeyChange);
    pedal.begin();
}

void loop() {
    bool config = digitalRead(PIN_CONFIG);

    // check config-btn state
    if ( config && !ConfigMode ) {
	    // Entering config mode
	    startConfig();
	} else if ( !config && ConfigMode ) {
	    finalizeConfig();
	}
    ConfigMode = config;

    pedal.poll();

    // wait for any MIDI messages being sent
    while (MIDI.sending()) {
        delayMicroseconds(20);
    }
}
