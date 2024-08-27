/**
 * Author: Stefan Hepp (stefan@stefant.org)
 *
 **/

#include <CalibratedAnalogInput.h>

#include <Arduino.h>

CalibratedAnalogInput::CalibratedAnalogInput()
: mPin(0), mRangeMax(1023), mCalibrating(false)
{
    mInputMin = 0;
    mInputMax = 1023;
    mCenter = 1023/2;
    mCenterDeadspot = 0;
    mHysteresis = 1;
    mCalibrating = false;
    mLastValue = center();
}

void CalibratedAnalogInput::setRange(int maxRange) {
    mRangeMax = maxRange;
}

void CalibratedAnalogInput::setSensitivy(int hysteresis, int deadspot) {
    mHysteresis = hysteresis;
    mCenterDeadspot = deadspot;
}

void CalibratedAnalogInput::setCalibrationData(const AICalibrationData &data)
{
    mInputMin = data.min;
    mInputMax = data.max;
    mCenter = data.center;
    mCalibrating = false;
}

void CalibratedAnalogInput::getCalibrationData(AICalibrationData &data) const
{
    data.min = mInputMin;
    data.max = mInputMax;
    data.center = mCenter;
}

void CalibratedAnalogInput::resetCalibration()
{
    mInputMin = 1023;
    mInputMax = 0;
    // Center point detected when calibration is started
    mCenter = analogRead(mPin);
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
    return map(mCenter, mInputMin, mInputMax, 0, mRangeMax);
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
        if (input > mInputMax) {
            mInputMax = input;
            updated = true;
        }
        if (updated) {
            if (mOnCalibration != nullptr) {
                mOnCalibration(mCalibrationPayload);
            }
        }
    }

    int mappedValue = map(input, mInputMin, mInputMax, 0, mRangeMax);

    int mappedCenter = center();
    
    if (abs(mappedValue - mappedCenter) <= mCenterDeadspot) {
        mappedValue = mappedCenter;
    }

    if (mappedValue < mLastValue - mHysteresis || mappedValue > mLastValue + mHysteresis) {
        mLastValue = mappedValue;
        if (mOnChange != nullptr) {
            mOnChange(mappedValue, mChangePayload);
        }
    }
}