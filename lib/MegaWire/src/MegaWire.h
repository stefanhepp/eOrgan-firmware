/**
 * Author: Stefan hepp (stefan@stefant.org)
 *
 * TWI/I2C library for AtMega controllers.
 **/

#ifndef __MegaWire_H__
#define __MegaWire_H__

#include <inttypes.h>

class MegaWire
{
    private:
        bool mIsMaster = false;
        uint8_t mSlaveAddr = 0;

    public:
        MegaWire();

        /**
         * Setup as master.
         */
        void begin();

        /**
         * Setup as slave, using the given slave address.
         * 
         * @param slaveAddr the 7 bit address of this device.
         */
        void begin(uint8_t slaveAddr);

        /**
         * Send a read request (master only). Sends a stop message after the request.
         */
        void requestFrom(uint8_t address, uint8_t quantity);

        void beginTransmission(uint8_t address);

        void write(uint8_t value);

        void write(const uint8_t* data, uint8_t length);

        void write(const char* str);

        void endTransmission();

        /**
         * Returns the number of bytes available for reading.
         */
        uint8_t available();

        /**
         * Consume the next byte from the read queue.
         */
        uint8_t read();

        /**
         * Register receive callback handler
         **/
        void onReceive( void(*callback)(uint8_t length) );
        
        /**
         * Register request callback handler
         **/
        void onRequest( void(*callback)(void) );
};

#endif


