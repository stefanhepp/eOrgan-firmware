
/*
 * @project     MIDIController
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * Audio effects and router
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "AudioProcessor.h"

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S2           i2s2_1;         //xy=88,97
AudioInputUSB            usb1;           //xy=190,238
AudioMixer4              mixer2;         //xy=445,234
AudioMixer4              mixer1;         //xy=449,133
AudioOutputI2S2          i2s2_2;         //xy=636,179
AudioConnection          patchCord2(usb1, 0, mixer1, 1);
AudioConnection          patchCord3(usb1, 1, mixer2, 1);
AudioConnection          patchCord4(i2s2_1, 0, mixer1, 0);
AudioConnection          patchCord5(i2s2_1, 0, mixer2, 0);
AudioConnection          patchCord6(mixer2, 0, i2s2_2, 1);
AudioConnection          patchCord7(mixer1, 0, i2s2_2, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=359,376
// GUItool: end automatically generated code

AudioProcessor::AudioProcessor()
{
}

void AudioProcessor::setUSBInputGain(float gain)
{
    mixer1.gain(1, gain);
    mixer2.gain(1, gain);
}

void AudioProcessor::setFXInputGain(float gain)
{
    mixer1.gain(0, gain);
    mixer2.gain(0, gain);
}

void AudioProcessor::setVolume(float volume) {
    sgtl5000_1.volume(volume);
}

void AudioProcessor::begin()
{
    AudioMemory(10);
    sgtl5000_1.enable();
    sgtl5000_1.volume(0.9);
    mixer1.gain(0, 1.0);
    mixer1.gain(1, 1.0);
    mixer2.gain(0, 1.0);
    mixer2.gain(1, 1.0);
}
