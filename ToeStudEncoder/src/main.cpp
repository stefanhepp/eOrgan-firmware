/**
 * Author: Stefan Hepp (stefan@stefant.org)
 * 
 * Implementation of the pedalboard controller.
 * Reads pedal switches and generates MIDI messages.
 * Configurable either via config switch or I2C interface.
 * 
 * Pins:
 * - PB0-7: IN;  Keyboard data bytes
 * - PC3:   OUT; Interrupt to signal I2C data ready
 * - PC4,5: I2C
 * - PC6:   In;  Reset (via ISP)
 * - PD0,1  UART
 * - PD2-4: OUT; Select line
 * - PD5-6: OUT; Select keyboard
 **/

#include <Arduino.h>
#include <MiniMIDI.h>
#include <MegaWire.h>
#include <CalibratedAnalogInput.h>

#include <common_config.h>

#include "config.h"
#include "Settings.h"
#include "ToeStuds.h"

// Create device driver instances
MiniMIDI MIDI;
MegaWire Wire;

Settings settings;
ToeStuds toestuds;

// Index into the pedals array below. Defines the order on I2C.
static const uint8_t PEDAL_CRESCENDO = 0;
static const uint8_t PEDAL_SWELL = 1;
static const uint8_t PEDAL_CHOIR = 2;

CalibratedAnalogInput pedals[3];

static uint8_t MIDIChannel;
static uint8_t MIDIChannelSwell;
static uint8_t MIDIChannelChoir;

static const uint8_t BUFFER_SIZE = 16;
static uint8_t ToeStudBuffer[BUFFER_SIZE];
static uint8_t ToeStudBufferLength = 0;

static const uint8_t IRQ_PEDALS = 0;
static const uint8_t IRQ_TOESTUDS = 1;

static uint16_t IRQFlags = 0x00;

static uint8_t SendMode;

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

static void updateMIDIChannel(uint8_t channel, uint8_t channelSwell, uint8_t channelChoir) {
    MIDIChannel = channel;
    MIDIChannelSwell = channelSwell;
    MIDIChannelChoir = channelChoir;
    settings.setMIDIChannel(channel, channelSwell, channelChoir);
}

static void resetEncoder(void)
{
    toestuds.reset();

    clearIRQ(IRQ_PEDALS);
    clearIRQ(IRQ_TOESTUDS);
}

void onPedalCalibrate(void* payload) {
    const uint8_t pedal = *((const uint8_t *)payload);
    AICalibrationData data;
    pedals[pedal].getCalibrationData(data);
    settings.setCalibrationData(pedal, data);
}

void onPedalChange(int value, void* payload) {
    const uint8_t pedal = *((const uint8_t *)payload);

    static uint8_t lastControlValue = 0;

    if (SendMode & ToeStudMode::TSM_MIDI) {
        switch (pedal) {
            case PEDAL_CHOIR:
                MIDI.sendPitchBend(value * 16, MIDIChannelChoir);
                break;
            case PEDAL_SWELL:
                MIDI.sendPitchBend(value * 16, MIDIChannelSwell);
                break;
            case PEDAL_CRESCENDO:
                uint8_t control = value >> 4;
                // since we scale down, only send MIDI message if the change is large enough for a new control value
                if (control != lastControlValue) { 
                    MIDI.sendControlChange(MidiControlChangeNumber::ExpressionController, control, MIDIChannel);
                    lastControlValue = control;
                }
                break;
        }
    }

    if (SendMode & ToeStudMode::TSM_I2C) {
        // Just set IRQ, read latest values via I2C
        sendIRQ(IRQ_PEDALS);
    }
}

void onToeStudPress(uint8_t button, bool longPress) {
    if (SendMode & ToeStudMode::TSM_MIDI) {

        // TODO send event
        //MIDI.sendProgramChange(button, MIDIChannel);

    }

    if (SendMode & ToeStudMode::TSM_I2C) {
        noInterrupts();
        if (ToeStudBufferLength < BUFFER_SIZE) {
            ToeStudBuffer[ToeStudBufferLength++] = (button << 1) | longPress;
        }
        sendIRQ(IRQ_TOESTUDS);
        interrupts();
    }
}

void i2cReceive(uint8_t length) {
    uint8_t cmd = Wire.read();

    switch (cmd) {
        case I2C_CMD_RESET: 
            resetEncoder();
            break;
        case I2C_CMD_SET_CHANNEL:
            if (Wire.available() > 2) {
                uint8_t channel = Wire.read();
                uint8_t channelSwell = Wire.read();
                uint8_t channelChoir = Wire.read();
                updateMIDIChannel(channel, channelSwell, channelChoir);
            }
            break;
        case I2C_CMD_CALIBRATE:
            for (uint8_t i = 0; i < 3; i++) {
                pedals[i].resetCalibration();
            }
            break;
        case I2C_CMD_STOP_CALIBRATE:
            for (uint8_t i = 0; i < 3; i++) {
                pedals[i].stopCalibration();
            }
            break;
        case I2C_CMD_SET_MODE:
            if (Wire.available()) {
                uint8_t value = Wire.read();
                SendMode = value;
                settings.setSendMode(value);
            }
            break;
    }
}

void i2cRequest()
{
    Wire.write(MIDIChannel);

    for (uint8_t i = 0; i < 3; i++) {
        Wire.write((uint8_t)(pedals[i].value() >> 8));
        Wire.write((uint8_t)(pedals[i].value() & 0xFF));
    }
    clearIRQ(IRQ_PEDALS);

    for (uint8_t i = 0; i < ToeStudBufferLength; i++) {
        Wire.write(ToeStudBuffer[i]);
    }
    ToeStudBufferLength = 0;

    clearIRQ(IRQ_TOESTUDS);
}

void setupPedal(const uint8_t *pedal, uint8_t pin)
{
    pedals[*pedal].onCalibrating(onPedalCalibrate, (void*) pedal);
    pedals[*pedal].onChange(onPedalChange, (void*) pedal);
    if (settings.hasCalibrationData()) {
        AICalibrationData data;
        settings.getCalibrationData(*pedal, data);
        pedals[*pedal].setCalibrationData(data);
    }
    pedals[*pedal].begin(pin);
}

void setup() {
    // Enable pullups for unconnected pins
    pinMode(PIN_PB4, INPUT_PULLUP);
    pinMode(PIN_PB5, INPUT_PULLUP);
    pinMode(PIN_PB6, INPUT_PULLUP);
    pinMode(PIN_PB7, INPUT_PULLUP);
    pinMode(PIN_PD2, INPUT_PULLUP);

    // Set output pin modes
    // Set pin value first before turing on output mode, to prevent spurious signals
    digitalWrite(PIN_INTERRUPT, LOW);
    pinMode(PIN_INTERRUPT, OUTPUT);

    MIDIChannel = settings.getMIDIChannel();
    MIDIChannelSwell = settings.getMIDIChannelSwell();
    MIDIChannelChoir = settings.getMIDIChannelChoir();

    SendMode = settings.getSendMode(ToeStudMode::TSM_MIDI);

    MIDI.begin(MIDIChannel);

    setupPedal(&PEDAL_CRESCENDO, PIN_CRESCENDO);
    setupPedal(&PEDAL_SWELL, PIN_PEDAL_SWELL);
    setupPedal(&PEDAL_CHOIR, PIN_PEDAL_CHOIR);

    Wire.onReceive(i2cReceive);
    Wire.onRequest(i2cRequest);
    Wire.begin(Controller::MC_ToeStud);

    toestuds.setHandleButton(onToeStudPress);
    toestuds.begin();
}

void loop() {
    toestuds.poll();
    for (uint8_t i = 0; i < 3; i++) {
        pedals[i].poll();
    }
    
    // wait for any MIDI messages being sent and add a small gap to resync
    while (MIDI.sending()) {
        delayMicroseconds(50);
    }
    delayMicroseconds(400);
}
