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
         **/
        void begin();

        /**
         * Setup as slave, using the given slave address.
         **/
        void begin(uint8_t slaveAddr);

        uint8_t read();

        uint8_t receive();

        /**
         * Register receive callback handler
         **/
        void onReceive( void(*callback)(int) );
        
        /**
         * Register request callback handler
         **/
        void onRequest( void(*callback)(void) );
};

#endif


