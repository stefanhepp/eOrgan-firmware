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
        int mRangeMax;

        int mInputMin;
        int mInputMax;

        int mLastValue;

        bool mCalibrating;

        void (*mOnCalibration)(void* payload) = nullptr;
        void (*mOnChange)(int value, void* payload) = nullptr;

        void* mCalibrationPayload = nullptr;
        void* mChangePayload = nullptr;

    public:
        explicit CalibratedAnalogInput();

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


