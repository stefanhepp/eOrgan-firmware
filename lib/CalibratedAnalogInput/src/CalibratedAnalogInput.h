/**
 * Author: Stefan hepp (stefan@stefant.org)
 *
 * Analog input sensor with calibrated 
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

        void (*mOnCalibration)(void) = nullptr;
        void (*mOnChange)(int value) = nullptr;

    public:
        explicit CalibratedAnalogInput(uint8_t pin);

        void setCalibrationData(const AICalibrationData &data);

        void getCalibrationData(AICalibrationData &data) const;

        void resetCalibration();

        void stopCalibration();

        void onCalibrating( void(*callback)(void) );

        void onChange( void(*callback)(int value) );

        int value() const;

        int center() const;

        void begin();

        void poll();
};

#endif


