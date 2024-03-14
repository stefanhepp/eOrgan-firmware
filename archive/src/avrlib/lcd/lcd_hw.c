/*
 * @project     AVR Library
 * @author      Stefan Hepp, s.hepp@gentics.com
 * @date        $Date: 2007-08-04 23:22:53 +0200 (Sam, 04 Aug 2007) $
 * @version     $Id: lcd_hw.c 109 2007-08-04 21:22:53Z stefan $
 *
 * Implementation of the low level lcd interface using SW/HW SPI.
 *
 * Copyright (c) 2007 Stefan Hepp
 * License: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 */
#include "lcd_hw.h"

#include <avr/interrupt.h>

#include <config/lcd_settings.h>

#include "lcd_const.h"

// Internal constants
#define CMD_DELAY               0x10
#define CMD_START               0x20
#define CMD_DATA                0x30
#define CMD_WAIT		0x40

#define SW_UART_PRESCALE	(1<<CS00)

// last start byte value and a delay counter for the timer
static uint8_t lastStart = 0;
static int16_t delayCounter = -1;

// buffer for command bytes
static uint8_t cmd_buf[LCD_CMD_BUF_SIZE];
static volatile uint8_t cmd_buf_start = 0;
static volatile uint8_t cmd_buf_size = 0;

static volatile uint8_t mode = LCD_MODE_EMPTY;

#if (LCD_HW_SPI == 0)
static uint8_t spi_byte = 0;
#endif 

/**
 * send a byte using SPI
 *
 * @param byte the byte to send.
 **/
static inline void send_spi_byte( uint8_t byte ) 
{
#if (LCD_HW_SPI == 0)
	spi_byte = (byte); 
	// enable timer
	TCCR0 |= SW_UART_PRESCALE;
#else
	SPDR = (byte);
#endif
}

/**
 * get the next cmd byte from the buffer and send it.
 * @return ERR_BUF_EMPTY if buffer is empty, else ERR_OK.
 **/
static int8_t send_next_byte(void) 
{
	uint8_t byte;
	uint8_t cmd;

	if ( cmd_buf_size == 0 ) {
		mode = LCD_MODE_EMPTY;
		return LCD_ERR_BUF_EMPTY;
	}

	cli();
	
	// get the byte from the buffer
	byte = cmd_buf[ (cmd_buf_start - cmd_buf_size + LCD_CMD_BUF_SIZE)%LCD_CMD_BUF_SIZE ];
	cmd_buf_size--;
	
	sei();
	
	cmd = byte & 0xF0;

	// send the byte or start a delay
	if ( cmd == CMD_START ) {
		send_spi_byte( 0x0F | ((byte & 0x0F)<<4) );
		mode = LCD_MODE_SEND;
	} else if ( cmd == CMD_DATA ) {
		send_spi_byte( byte & 0x0F );
		mode = LCD_MODE_SEND;
	} else if ( cmd == CMD_DELAY ) {
		// multiply delay by 4 to get 0.4ms delays
		delayCounter = (byte & 0x0F) << 2;
		mode = LCD_MODE_DELAY;
	} else if ( cmd == CMD_WAIT ) {
		// TODO implement wait
		delayCounter = 0;
		mode = LCD_MODE_DELAY;
	}

	return LCD_ERR_OK;
}

/**
 * store a cmd byte in the buffer, start sending if buffer is empty.
 * 
 * If the buffer is full and LCD_BUSYWAIT_ON_FULL is 1, delay until the byte can be stored.
 * 
 * @param cmd byte to store, upper nibble is type, lower nibble is data.
 * @return an error code if buffer is full, else ERR_OK.
 **/
static int8_t store_cmd( uint8_t cmd )
{

	// check if buffer is not full
#if (LCD_BUSYWAIT_ON_FULL == 1)
	sei();
	while ( cmd_buf_size >= LCD_CMD_BUF_SIZE ) ;
#else
	if ( cmd_buf_size >= LCD_CMD_BUF_SIZE ) {
		return LCD_ERR_BUF_FULL;
	}
#endif
	
	cli();
	
	// add byte to buffer
	cmd_buf[cmd_buf_start] = cmd;

	cmd_buf_start = (cmd_buf_start+1)%LCD_CMD_BUF_SIZE;
	cmd_buf_size++;

	sei();
	
	// if nothing is done now, send this byte.
	if ( mode == LCD_MODE_EMPTY ) {
		send_next_byte();
	}

	return LCD_ERR_OK;
}

int8_t LcdWriteDelay( uint8_t delay )
{
	int8_t ret;
	uint8_t i;

	if ( delay == 0 ) {
		return LCD_ERR_OK;
	}

	// store delay (up to 15)
	if ( 0x0F & delay ) {
	   ret = store_cmd( CMD_DELAY | (0x0F & delay) );
	   if ( ret != LCD_ERR_OK ) {
	    	return ret;
	    }
	}
	
	// store greater delays
	for ( i = 0; i < (delay>>4); i++ ) {
		ret = store_cmd( CMD_DELAY | 0x0F );
		if ( ret != LCD_ERR_OK ) {
			return ret;
		}
	}

	return LCD_ERR_OK;
}

int8_t LcdWriteCmd( uint8_t data, uint8_t rs )
{
	// start nibble is High, RW (=0), RS, End(=0)
	uint8_t start = 0x01 | ( rs << 2 );
	uint8_t ret;
	
	// send start byte, only if it differs from the last start byte
	if ( start != lastStart ) {
		ret = store_cmd( CMD_START | start );
		if ( ret != LCD_ERR_OK ) {
			lastStart = 0;
			return ret;
		}
		lastStart = start;
	}

	// send low data nibble
	ret = store_cmd( CMD_DATA | (0x0F & data ) );
	if ( ret != LCD_ERR_OK ) {
		lastStart = 0;
		return ret;
	}

	// send high data nibble
	ret = store_cmd( CMD_DATA | (data>>4) );
	if ( ret != LCD_ERR_OK ) {
		lastStart = 0;
		return ret;
	}

	return LCD_ERR_OK;
}

int8_t LcdWriteWait(void)
{
	return store_cmd( CMD_WAIT );
}

int8_t LcdHwInit(void)
{
	// init timer1 for bg light
	TIMER_TCCRA |= TIMER_TCCRA_SET;
	TIMER_TCCRA &= ~(TIMER_TCCRA_CLR);
	TIMER_TCCRB |= TIMER_TCCRB_SET;
	TIMER_TCCRB &= ~(TIMER_TCCRB_CLR);

	// enable overflow interrupt
	TIMER_TIMSK |= (1<<TIMER_OIE);

	TIMER_TOP = 200;

	LCD_TIMER_OCR_BGLIGHT = 140;
	LCD_TIMER_OCR_CONTRAST = 60;

	// set contrast, bglight
#ifdef LCD_BGLIGHT_PIN
	LCD_BGLIGHT_DDR |= (1<<LCD_BGLIGHT_PIN);
	LCD_BGLIGHT_PORT |= (1<<LCD_BGLIGHT_PIN);
#endif
#ifdef LCD_CONTRAST_PIN
	LCD_CONTRAST_DDR |= (1<<LCD_CONTRAST_PIN);
	LCD_CONTRAST_PORT &= ~(1<<LCD_CONTRAST_PIN);
#endif

	// set SPI IO Pins, enable slave
	SPI_DDR |= (1<<SPI_SS_PIN)|(1<<SPI_MOSI_PIN)|(1<<SPI_SCK_PIN);
	SPI_PORT &= ~(1<<SPI_SS_PIN);

#if (LCD_HW_SPI == 0)
	
	// TODO use timer1 too? config timer per header
	// timer0, prescale 1, CTC, 2µs
	TCCR0 |= (1<<WGM01);
	OCR0 = 32;
	TIMSK |= (1<<OCIE0);
	
#else
	
	// Enable SPI, enable interrupts, set prescaler 64 (~4µs clk, 35µs/Byte), master, LSB first
	SPCR |= (1<<SPE)|(1<<SPIE)|(1<<SPR1)|(1<<MSTR)|(1<<DORD);
	SPCR &= ~(1<<SPR0);
	
#endif

	return LCD_ERR_OK;
}

void LcdSetBgLight(uint8_t duty_cycle) 
{
	LCD_TIMER_OCR_BGLIGHT = duty_cycle << 1;
}

void LcdSetContrast(uint8_t duty_cycle)
{
	LCD_TIMER_OCR_CONTRAST = duty_cycle << 1;
}

uint8_t LcdGetMode(void)
{
	return mode;
}

SIGNAL(TIMER_SIG_OVERFLOW)
{
	// TODO check: if ints are enabled here, pwm is not generated
	
        if ( delayCounter == 0 ) {
                delayCounter = -1;

		sei();
		send_next_byte();

        } else if ( delayCounter > 0 ) {
                delayCounter--;
        }
}

#if (LCD_HW_SPI == 0)

SIGNAL(SIG_OUTPUT_COMPARE0)
{
	sei();
	
        static uint8_t mask = 1;

        static uint16_t cnt = 0;
        if ( cnt < 10 ) {
            cnt ++;
            return;
        }
        cnt = 0;

        // if clk is high to low, set data byte
        if ( SPI_PORT & (1<<SPI_SCK_PIN) ) {
                if ( spi_byte & mask ) {
                        SPI_PORT |= (1<<SPI_MOSI_PIN);
                } else {
                        SPI_PORT &= ~(1<<SPI_MOSI_PIN);
                }
                mask = (mask << 1);
        } else if ( mask == 0 ) {
                // end of transmission
                mask = 1;
                TCCR0 &= ~(SW_UART_PRESCALE);

                send_next_byte();
                return;
        }

        // toggle clock
        SPI_PORT ^= (1<<SPI_SCK_PIN);
}

#else // HW_SPI

SIGNAL(SIG_SPI)
{
	sei();
	
        send_next_byte();
}

#endif // HW_SPI

