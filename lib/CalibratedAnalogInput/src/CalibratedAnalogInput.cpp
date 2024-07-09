/**
 * Author: Stefan Hepp (stefan@stefant.org)
 *
 **/

#include <CalibratedAnalogInput.h>

#include <Arduino.h>

CalibratedAnalogInput::CalibratedAnalogInput()
: mPin(0), mRangeMax(1023)
{
    resetCalibration();
    mLastValue = center();
}

void CalibratedAnalogInput::setCalibrationData(const AICalibrationData &data)
{
    mInputMin = data.min;
    mInputMax = data.max;
    mCalibrating = false;
}

void CalibratedAnalogInput::getCalibrationData(AICalibrationData &data) const
{
    data.min = mInputMin;
    data.max = mInputMax;
}

void CalibratedAnalogInput::resetCalibration()
{
    mInputMin = 1023;
    mInputMax = 0;
    mCalibrating = true;
}

void CalibratedAnalogInput::stopCalibration()
{
    mCalibrating = false;
}

void CalibratedAnalogInput::onCalibrating( void(*callback)(void* payload), void* payload)
{
    mOnCalibration = callback;
    mCalibrationPayload = payload;
}

void CalibratedAnalogInput::onChange( void(*callback)(int value, void* payload), void* payload )
{
    mOnChange = callback;
    mChangePayload = payload;
}

int CalibratedAnalogInput::value() const
{
    return mLastValue;
}

int CalibratedAnalogInput::center() const
{
    return mRangeMax/2;
}

void CalibratedAnalogInput::begin(uint8_t pin)
{
    mPin = pin;
}

void CalibratedAnalogInput::poll()
{
    int input = analogRead(mPin);
    if (mCalibrating) {
        bool updated = false;
        if (input < mInputMin) {
            mInputMin = input;
            updated = true;
        }
        if (input < mInputMax) {
            mInputMax = input;
            updated = true;
        }
        if (updated) {
            mOnCalibration(mCalibrationPayload);
        }
    }

    int newValue = map(input, mInputMin, mInputMax, 0, mRangeMax);

    if (newValue != mLastValue) {
        mLastValue = newValue;
        mOnChange(newValue, mChangePayload);
    }
}