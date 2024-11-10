/*
 * @project     PanelUI
 * @author      Stefan Hepp, stefan@stefant.org
 *
 * UART interface to main controller implementation.
 *
 * Copyright 2024 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "SerialInterface.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>

#include <common_config.h>
#include <panel_commands.h>

#include "config.h"


SerialInterface::SerialInterface()
{
}

int SerialInterface::serialOpen(const char* device, uint32_t baud)
{
    struct termios options ;
    speed_t myBaud ;
    int     status, fd ;

    switch (baud)
    {
        case    9600:	myBaud =    B9600 ; break ;
        case   19200:	myBaud =   B19200 ; break ;
        case   38400:	myBaud =   B38400 ; break ;
        case  115200:	myBaud =  B115200 ; break ;
        case  230400:	myBaud =  B230400 ; break ;
        case  460800:	myBaud =  B460800 ; break ;
        case  500000:	myBaud =  B500000 ; break ;
        case  576000:	myBaud =  B576000 ; break ;
        case  921600:	myBaud =  B921600 ; break ;
        case 1000000:	myBaud = B1000000 ; break ;
        default:
          return -2 ;
    }

    if ((fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1) {
        return -1;
    }

    //fcntl (fd, F_SETFL, O_RDWR) ;

    // Get and modify current options:
    tcgetattr (fd, &options) ;

    cfmakeraw   (&options) ;
    cfsetispeed (&options, myBaud) ;
    cfsetospeed (&options, myBaud) ;

    options.c_cflag |= (CLOCAL | CREAD) ; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
    options.c_cflag &= ~PARENB ; // Clear parity bit, disabling parity
    options.c_cflag &= ~(PARODD | CMSPAR) ;
    options.c_cflag &= ~CSTOPB ; // Clear stop field, only one stop bit used
    options.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control
    options.c_cflag &= ~CSIZE ;
    options.c_cflag |= CS8 ;     // 8 bits per byte

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG | IEXTEN);

    options.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    options.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    options.c_oflag &= ~OPOST ;
    options.c_oflag &= ~OCRNL ;
    options.c_oflag &= ~ONLCR ; // Prevent conversion of newline to carriage return/line feed

    options.c_cc [VMIN]  =   0 ;
    options.c_cc [VTIME] = 10 ;	// 1 second (10 deciseconds)

    tcsetattr (fd, TCSANOW, &options);

    ioctl (fd, TIOCMGET, &status);

    status |= TIOCM_DTR ;
    status |= TIOCM_RTS ;

    ioctl (fd, TIOCMSET, &status);

    usleep (10000) ;	// 10mS

    return fd ;
}

void SerialInterface::serialWrite(uint8_t data)
{
    ssize_t bytes_written = write(mFdSerial, &data, 1);
    if (bytes_written != 1) {
        perror("Error writing to file descriptor");
    }
}

int SerialInterface::serialBufferSize()
{
    int result;

    if (ioctl(mFdSerial, FIONREAD, &result) == -1) {
        return -1;
    }

    return result;
}

int SerialInterface::serialRead()
{
    uint8_t data;

    if (read(mFdSerial, &data, 1) != 1) {
        return -1;
    }

    return ((int)data) & 0xFF;
}

void SerialInterface::serialFlush()
{
    // Only flush output buffer; flushing input buffer clears it.
    tcflush(mFdSerial, TCOFLUSH);
}

void SerialInterface::serialClose()
{
    close(mFdSerial);
    mFdSerial = -1;
}

uint8_t SerialInterface::readByte()
{
    if (mRxReadPos < mRxBufferLength) {
        return mRxBuffer[mRxReadPos++];
    }
    return 0xFF;
}

void SerialInterface::writeByte(uint8_t data)
{
    if (mTxBufferLength != -1 && mTxBufferLength < CMD_BUFFER_SIZE) {
        mTxBuffer[mTxBufferLength++] = data;
    }
}

void SerialInterface::beginTransmission(uint8_t command)
{
    mTxCommand = command;
    mTxBufferLength = 0;
}

void SerialInterface::endTransmission()
{
    serialWrite(0x80 | (mTxBufferLength & 0xFF));
    serialWrite(mTxCommand);
    for (int i = 0; i<mTxBufferLength; i++) {
        serialWrite(mTxBuffer[i]);
    }
    serialFlush();
    mTxBufferLength = -1;
}

void SerialInterface::processSerialData(int data)
{
    if (data < 0 || data > 255) {
        return;
    }

    if (mRxBufferLength == -1) {
        // Start of transmission: 0x80 | length
        if (data & 0x80) {
            mRxCommandLength = (data & ~0x80);
            mRxBufferLength = -2;
        }
    }
    else if (mRxBufferLength == -2)  {
        // Second byte: command
        mRxCommand = data;
        // Following bytes are payload
        mRxBufferLength = 0;
    } else {
        if (mRxBufferLength < CMD_BUFFER_SIZE) {
            mRxBuffer[mRxBufferLength] = data;
        }
        mRxBufferLength++;
    }
    if (mRxBufferLength == mRxCommandLength) {
        mRxReadPos = 0;
	std::cout << "RX process: " << (int)mRxCommand << std::endl;
        processCommand(mRxCommand);
        mRxBufferLength = -1;
    }
}

void SerialInterface::processCommand(uint8_t command)
{
    uint8_t output, routerState;
    StatusInfo statusInfo;

    switch (command) {
        case PANEL_CMD_STATUS:
            // received status
            output = readByte();
            routerState = readByte();

            statusInfo.USBOutputEnabled  = output & 0x01 ? true : false;
            statusInfo.MIDIOutputEnabled = output & 0x02 ? true : false;
            statusInfo.CouplerEnabled    = routerState & 0x01 ? true : false;
            statusInfo.SendMIDIMessages  = routerState & 0x02 ? true : false;

            if (mStatusCallback) {
                mStatusCallback(statusInfo);
            }
            break;
        default:
            std::cerr << "Received unknown command: " << command << std::endl;
            break;
    }
}

void SerialInterface::requestStatus()
{
    beginTransmission(PANEL_CMD_STATUS);
    endTransmission();
}

void SerialInterface::enableRouter(bool USB, bool MIDI)
{
    uint8_t output = 0x00;
    output |= USB  ? 0x01 : 0;
    output |= MIDI ? 0x02 : 0;

    beginTransmission(PANEL_CMD_ROUTER);
    writeByte(output);
    endTransmission();
}

void SerialInterface::sendCouplerMode(CouplerMode mode)
{
    beginTransmission(PANEL_CMD_COUPLER);
    writeByte(mode);
    endTransmission();
}

void SerialInterface::sendVolume(VolumeChannel channel, int volume)
{
    if (volume < 0) {
        volume = 0;
    }
    if (volume > 1023) {
        volume = 1023;
    }

    beginTransmission(PANEL_CMD_VOLUME);
    writeByte(volume >> 8);
    writeByte(volume & 0xFF);
    endTransmission();
}

void SerialInterface::sendPageTurn(PageTurn direction)
{
    beginTransmission(PANEL_CMD_PAGE_TURN);
    writeByte(direction);
    endTransmission();
}

int SerialInterface::begin()
{
    if ((mFdSerial = serialOpen(DEV_SERIAL, PANEL_UART_SPEED)) < 0)
    {
        std::cerr << "Unable to open serial device: " << strerror(errno) << std::endl;
        return 1;
    }
    return 0;
}

void SerialInterface::loop()
{
    while (serialBufferSize() > 0) {
        processSerialData( serialRead() );
    }
}
