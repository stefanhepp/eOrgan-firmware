/**
 * Author: Stefan Hepp (stefan@stefant.org)
 *
 **/

#include <MegaWire.h>

#include <avr/io.h>
#include <avr/interrupt.h>

MegaWire::MegaWire() {
}

void MegaWire::begin() {
    mIsMaster = true;

    TWCR = (1<<TWEA)|(1<<TWEA)|(1<<TWIE);
}

void MegaWire::begin(uint8_t slaveAddr) {
    mIsMaster = false;
    mSlaveAddr = slaveAddr;

    TWCR = (1<<TWEA)|(1<<TWEA)|(1<<TWIE);
    TWAR = slaveAddr << 1;
}

uint8_t MegaWire::read() {

    return 0;
}

uint8_t MegaWire::receive() {

    return 0;
}

ISR(TWI_vect) {
    
}
