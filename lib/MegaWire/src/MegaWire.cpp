/**
 * Author: Stefan Hepp (stefan@stefant.org)
 *
 **/

#include <MegaWire.h>

#include <avr/io.h>
#include <avr/interrupt.h>

/**
 * Set this flag to enable the Master implementation.
 */
//#define I2C_ENABLE_MASTER

#define I2C_BUFFER_SIZE 16

static uint8_t Buffer[I2C_BUFFER_SIZE];

static uint8_t BufferLength;
static uint8_t BufferPos;

static void (*OnReceiveFct)(uint8_t) = nullptr;
static void (*OnRequestFct)(void) = nullptr;

#ifdef I2C_ENABLE_MASTER
static uint8_t SlaveAddress;
static uint8_t RequestSize;
#endif

MegaWire::MegaWire()
{
}

void MegaWire::begin() {
#ifdef I2C_ENABLE_MASTER
    TWCR = (1<<TWEA)|(1<<TWEA)|(1<<TWIE);
#endif
}

void MegaWire::begin(uint8_t slaveAddr) {
    // Set 7-bit slave address, ignore general call.
    TWAR = slaveAddr << 1;
    // Enable I2C, ack, interrupts
    TWCR = (1<<TWEN)|(1<<TWEA)|(1<<TWIE);
}

void MegaWire::requestFrom(uint8_t address, uint8_t quantity, bool stop) {
#ifdef I2C_ENABLE_MASTER
    // Store a SLA+R read address
    SlaveAddress = (address << 1);
    RequestSize = quantity;
    BufferPos = 0;
    BufferLength = 0;
    TWCR |= (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
#endif
}

void MegaWire::beginTransmission(uint8_t address) {
#ifdef I2C_ENABLE_MASTER
    // Store a SLA+W write address
    SlaveAddress = (address << 1) | 1;
    // Clear buffer for writing
    BufferLength = 0;
    BufferPos = 0;
#endif    
}

void MegaWire::endTransmission(bool stop) {
#ifdef I2C_ENABLE_MASTER
    if (BufferLength > 0) {
        // Must contain at least one byte to transmit
        // Send start bit to start transmission
        TWCR |= (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    }
#endif
}

void MegaWire::write(uint8_t value) {
    if (BufferLength < I2C_BUFFER_SIZE) {
        Buffer[BufferLength++] = value;
    }
}

void MegaWire::write(const uint8_t *data, uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {
        write(data[i]);
    }
}

void MegaWire::write(const char* str) {
    while (*str != 0) {
        write(*str);
        str++;
    }
}

uint8_t MegaWire::available() {
    if (BufferPos < BufferLength) {
        return BufferLength - BufferPos;
    } else {
        return 0;
    }
}

uint8_t MegaWire::read() {
    if (BufferPos < BufferLength) {
        return Buffer[BufferPos++];
    } else {
        return 0;
    }
}

void MegaWire::onReceive( void(*callback)(uint8_t length) ) {
    OnReceiveFct = callback;
}

void MegaWire::onRequest( void(*callback)() ) {
    OnRequestFct = callback;
}

ISR(TWI_vect) {
    switch (TWSR & 0xF8) {
#ifdef I2C_ENABLE_MASTER
        case 0x08: // START has been transmitted
        case 0x10: // Repeated START was transmitted
            // Send slave address
            TWDR = SlaveAddress;
            // Clear start bit
            TWCR &= ~(1<<TWSTA);
            // Send address
            TWCR |= (1<<TWINT);
            break;
        case 0x18: // SLA+W transmitted, ACK received

            // TODO start data transfer, or send STOP/repeated START

            break;
        case 0x20: // SLA+W transmitted, NACK received

            break;
        case 0x28: // Data transmitted, ACK received

            break;
        case 0x30: // Data transmitted, NOT ACK received

            break;
        case 0x38: // Arbitration lost
            // Try sending START again when bus is free
            TWCR |= (1<<TWINT)|(1<<TWSTA);
            break;
        case 0x40: // SLA+R transmitted, ACR received

            break;
        case 0x48: // SLA+R transmitted, NOT ACK received

            break;
        case 0x50: // Data received, ACK was sent

            break;
        case 0x58: // Data received, NOT ACK was sent

            break;
#endif
        case 0x60: // SLA+W received, ACK returned
            // Transmission about to start, clear buffer
            BufferLength = 0;
            BufferPos = 0;
            // Clear Interrupt flag, ACK next receive
            TWCR |= (1<<TWINT)|(1<<TWEA);
            break;
        case 0x80: // Previously addressed with SLA+W, data received, ACK sent
            if (BufferLength < I2C_BUFFER_SIZE) {
                // Buffer received data
                Buffer[BufferLength++] = TWDR;
            }
            if (BufferLength < I2C_BUFFER_SIZE - 1) {
                // Still buffer free, receive more by sending ACK
                TWCR |= (1<<TWEA);
            } else {
                // Buffer will be full, send NOT ACK
                TWCR &= ~(1<<TWEA);
            }
            // Clear interrupt, release SCL to receive next byte
            TWCR |= (1<<TWINT);
            break;
        case 0x88: // previously addressed with SLA+W, data received, NOT ACK sent
            if (BufferLength < I2C_BUFFER_SIZE) {
                // Buffer received data
                Buffer[BufferLength++] = TWDR;
            }
            if (OnReceiveFct != nullptr && BufferLength > 0) {
                // Transmission finished wiht NACK, process data
                OnReceiveFct(BufferLength);
            }
            // Clear interrupt, return to receive mode
            TWCR |= (1<<TWINT)|(1<<TWEA);
            break;
        case 0xA0: // STOP condition, or repeated START condition in Slave mode
            if (OnReceiveFct != nullptr && BufferLength > 0) {
                // Transmission finished, process data
                OnReceiveFct(BufferLength);
            }
            // Finished reception, ready for next transmission.
            TWCR |= (1<<TWINT)|(1<<TWEA);
            break;
        case 0xA8: // SLA+R received, ACK returned
            // New read request, clear buffer and get data
            BufferLength = 0;
            BufferPos = 0;
            if (OnRequestFct != nullptr) {
                OnRequestFct();
            }
            // Intentional Fallthrough here!!
            // Execute 0xB8 next to send first byte!
        case 0xB8: // Data byte transmitted, ACK received
            if (BufferPos < BufferLength) {
                TWDR = Buffer[BufferPos++];
            } else {
                // No more data, send 0xFF
                TWDR = 0xFF;
            }
            // Expect ACK or NACK depending on if there is remaining data
            if (BufferPos < BufferLength) {
                TWCR |= (1<<TWEA);
            } else {
                // Last byte sent, expect NACK
                TWCR &= ~(1<<TWEA);
            }
            TWCR |= (1<<TWINT);
            break;
        case 0xC0: // Data byte transmitted, NOT ACK received
            // No more data received, finish transmission
            // Return to ready state
            TWCR |= (1<<TWINT)|(1<<TWEA);
            break;
        case 0xC8: // Last data byte transmitted, ACK received
            // Master wants more data, but we have no more to send.
            // Return to not addressed state
            TWCR |= (1<<TWINT)|(1<<TWEA);
            break;
        default:
            // Any other state: Clear interrupt flag, reset status by setting STOP bit
            TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
            break;
    }
}
