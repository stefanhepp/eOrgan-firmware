/**
 * Author: Stefan hepp (stefan@stefant.org)
 *
 * TWI/I2C library for AtMega controllers.
 **/

#ifndef __MegaWire_H__
#define __MegaWire_H__

#include <inttypes.h>

/**
 * Implementation of the Wire interface for AtMega 8bit controllers.
 * 
 * Only supports a single hardware TWI interface.
 * Only supports a single instantiation of this class.
 */
class MegaWire
{
    private:

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
        void requestFrom(uint8_t address, uint8_t quantity, bool stop = true);

        void beginTransmission(uint8_t address);

        void write(uint8_t value);

        void write(const uint8_t* data, uint8_t length);

        void write(const char* str);

        void endTransmission(bool stop = true);

        /**
         * Returns the number of bytes available for reading.
         */
        uint8_t available();

        /**
         * Consume the next byte from the read queue.
         */
        uint8_t read();

        /**
         * Register receive callback handler.
         * 
         * The callback will be executed in the interrupt handler, keep operation short.
         * Received data can be read via repeated read() calls, while available() > 0.
         **/
        void onReceive( void(*callback)(uint8_t length) );
        
        /**
         * Register request callback handler
         * 
         * The callback will be executed in the interrupt handler, keep operation short.
         * Use write() to put data into the transmit buffer. Transmission will start the
         * callback function returns.
         * 
         * Do not call beginTransmission() or endTransmission() inside the callback function.
         **/
        void onRequest( void(*callback)(void) );
};

#endif


