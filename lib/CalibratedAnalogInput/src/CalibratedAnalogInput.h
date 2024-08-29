/**
 * Author: Stefan hepp (stefan@stefant.org)
 *
 * Analog input sensor with range calibration.
 **/

#ifndef __CalibratedAnalogInput_H__
#define __CalibratedAnalogInput_H__

#include <inttypes.h>

/**
 * Container for analog input calibration data
 */
class AICalibrationData 
{
    public:
        int center;
        int min;
        int max;
};

/**
 * Implementation of the Wire interface for AtMega 8bit controllers.
 * 
 * Only supports a single hardware TWI interface.
 * Only supports a single instantiation of this class.
 */
class CalibratedAnalogInput
{
    private:
        uint8_t mPin;
        // Maximum output range
        int mRangeMax;

        // Following values are in raw input scale
        int mInputMin;
        int mInputMax;
        int mInputCenter;

        // Following values are in output scale
        int mCenterDeadspot;
        int mHysteresis;
        int mCenter;

        int mLastValue;

        bool mCalibrating;

        void (*mOnCalibration)(void* payload) = nullptr;
        void (*mOnChange)(int value, void* payload) = nullptr;

        void* mCalibrationPayload = nullptr;
        void* mChangePayload = nullptr;

    public:
        explicit CalibratedAnalogInput();

        void setRange(int maxRang, int center = -1);

        void setSensitivy(int hysteresis, int deadspot = 0);

        void setCalibrationData(const AICalibrationData &data);

        void getCalibrationData(AICalibrationData &data) const;

        void resetCalibration();

        void stopCalibration();

        void onCalibrating( void(*callback)(void* payload), void* payload = nullptr);

        void onChange( void(*callback)(int value, void* payload), void* payload = nullptr);

        int value() const;

        int center() const;

        void begin(uint8_t pin);

        void poll();
};

#endif


