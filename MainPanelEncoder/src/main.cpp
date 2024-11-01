/**
 * Author: Stefan Hepp (stefan@stefant.org)
 * 
 * Implementation of the main control panel controller.
 **/

#include <Arduino.h>
#include <MegaWire.h>
#include <CalibratedAnalogInput.h>

#include <common_config.h>

#include "config.h"
#include "Settings.h"
#include "MainPanel.h"

// Create device driver instances
MegaWire Wire;

Settings settings;
MainPanel Panel;

// Index into the pedals array below. Defines the order on I2C.
static const uint8_t POTI_VOLUME = 0;
static const uint8_t POTI_FX1 = 1;
static const uint8_t POTI_FX2 = 2;

CalibratedAnalogInput volume[3];

int EncoderPosition[2];

static const uint8_t BUFFER_SIZE = 16;
static uint8_t ButtonBuffer[BUFFER_SIZE];
static uint8_t ButtonBufferLength = 0;

static const uint8_t IRQ_BUTTONS = 0;
static const uint8_t IRQ_VOLUME = 1;

static uint16_t IRQFlags = 0x00;

static void sendIRQ(uint8_t flag)
{
    IRQFlags |= (1<<flag);
    digitalWrite(PIN_INTERRUPT, LOW);
}

static void clearIRQ(uint8_t flag)
{
    IRQFlags &= ~(1<<flag);
    if (IRQFlags == 0x00) {
        digitalWrite(PIN_INTERRUPT, HIGH);
    }
}

void setLEDs(uint8_t leds) {
    digitalWrite(PIN_LED1, (leds>>0)&0x01);
    digitalWrite(PIN_LED2, (leds>>1)&0x01);
    digitalWrite(PIN_LED3, (leds>>2)&0x01);
    digitalWrite(PIN_LED4, (leds>>3)&0x01);
    digitalWrite(PIN_LED5, (leds>>4)&0x01);
    digitalWrite(PIN_LED6, (leds>>5)&0x01);
    digitalWrite(PIN_LED_CTRL, (leds>>6)&0x01);
}

static void resetEncoder(void)
{
    clearIRQ(IRQ_BUTTONS);
    clearIRQ(IRQ_VOLUME);
    ButtonBufferLength = 0;
    EncoderPosition[0] = 0;
    EncoderPosition[1] = 0;
    setLEDs(0x00);
}

void onPotiCalibrate(void* payload) {
    const uint8_t knob = *((const uint8_t *)payload);
    AICalibrationData data;
    volume[knob].getCalibrationData(data);
    settings.setCalibrationData(knob, data);
}

void onPotiChange(int value, void* payload) {
    //const uint8_t knob = *((const uint8_t *)payload);
    sendIRQ(IRQ_VOLUME);
}

void onButtonPress(uint8_t button, bool longPress) {
    noInterrupts();
    if (ButtonBufferLength < BUFFER_SIZE) {
        ButtonBuffer[ButtonBufferLength++] = (button << 1) | longPress;
    }
    sendIRQ(IRQ_BUTTONS);
    interrupts();
}

void onEncoderPosition(uint8_t encoder, int position) {
    noInterrupts();
    EncoderPosition[encoder] += position;
    sendIRQ(IRQ_BUTTONS);
    interrupts();
}

void i2cReceive(uint8_t length) {
    uint8_t cmd = Wire.read();

    switch (cmd) {
        case I2C_CMD_RESET: 
            resetEncoder();
            break;
        case I2C_CMD_CALIBRATE:
            for (uint8_t i = 0; i < 3; i++) {
                volume[i].resetCalibration();
            }
            break;
        case I2C_CMD_STOP_CALIBRATE:
            for (uint8_t i = 0; i < 3; i++) {
                volume[i].stopCalibration();
            }
            break;
        case I2C_CMD_SET_LEDS:
            if (Wire.available()) {
                uint8_t value = Wire.read();
                setLEDs(value);
            }
            break;
        case I2C_CMD_SET_SENSITIVITY:
            if (Wire.available()) {
                uint8_t value = Wire.read();
                settings.setSensitivity(value);
                for (int i = 0; i < 3; i++) {
                    volume[i].setSensitivy(value);
                }
            }
            break;
    }
}

void i2cRequest()
{
    // Send poti values
    for (uint8_t i = 0; i < 3; i++) {
        int value = volume[i].value();
        Wire.write((uint8_t)(value >> 8));
        Wire.write((uint8_t)(value & 0xFF));
    }
    clearIRQ(IRQ_VOLUME);

    // Send encoder positions, clear on read (send delta only)
    Wire.write((int8_t)EncoderPosition[0]);
    Wire.write((int8_t)EncoderPosition[1]);

    // Send button press events
    Wire.write(ButtonBufferLength);
    for (uint8_t i = 0; i < ButtonBufferLength; i++) {
        Wire.write(ButtonBuffer[i]);
    }
    ButtonBufferLength = 0;

    clearIRQ(IRQ_BUTTONS);
}

void setupPoti(const uint8_t *knob, uint8_t pin)
{
    volume[*knob].onCalibrating(onPotiCalibrate, (void*) knob);
    volume[*knob].onChange(onPotiChange, (void*) knob);
    if (settings.hasCalibrationData()) {
        AICalibrationData data;
        settings.getCalibrationData(*knob, data);
        volume[*knob].setCalibrationData(data);
    }
    volume[*knob].setRange(1023);
    volume[*knob].setSensitivy(settings.getSensitivity());
    volume[*knob].begin(pin);
}
 
void setup() {
    // Set output pin modes
    // Set pin value first before turing on output mode, to prevent spurious signals
    digitalWrite(PIN_INTERRUPT, HIGH);
    pinMode(PIN_INTERRUPT, OUTPUT);

    pinMode(PIN_LED1, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    pinMode(PIN_LED3, OUTPUT);
    pinMode(PIN_LED4, OUTPUT);
    pinMode(PIN_LED5, OUTPUT);
    pinMode(PIN_LED6, OUTPUT);
    pinMode(PIN_LED_CTRL, OUTPUT);

    setupPoti(&POTI_VOLUME, PIN_VOLUME);
    setupPoti(&POTI_FX1, PIN_FX1);
    setupPoti(&POTI_FX2, PIN_FX2);

    Wire.onReceive(i2cReceive);
    Wire.onRequest(i2cRequest);
    Wire.begin(Controller::MC_MainPanel);

    Panel.setHandleButton(onButtonPress);
    Panel.begin();
}

void loop() {
    Panel.poll();
    for (uint8_t i = 0; i < 3; i++) {
        volume[i].poll();
    }
}
