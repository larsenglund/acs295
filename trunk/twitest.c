/*
 * By Lars Englund 2007
 * lars . englund at gmail . com
 *
 * Controls a ACS295 subwoofer, see project page for more information:
 * http://code.google.com/p/acs295/
 *
 * $Id$
 */

#define DEBUG 1
#define F_CPU 8000000UL	// System clock in Hz.

#include <inttypes.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef DEBUG
#include <avr/io.h>
#endif


#ifndef UCSRB
# ifdef UCSR1A		// ATmega128
#  define UCSRA UCSR1A
#  define UCSRB UCSR1B
#  define UBRR UBRR1L
#  define UDR UDR1
# else				// ATmega8
#  define UCSRA USR
#  define UCSRB UCR
# endif
#endif
#ifndef UBRR
#  define UBRR UBRRL
#endif


#define TWI_SLA_TDA7433	0x8a
#define TWI_SCL			PC1		// Output	
#define TWI_SDA			PC0		// Output
#define TWI_OUT_PORT	PORTC
#define TWI_DDR_PORT	DDRC	// Set bits to 1 here to make them outputs

#define low(port, pin) (port &= ~_BV(pin))
#define high(port, pin) (port |= _BV(pin))

#define twi_low(which) low(TWI_OUT_PORT, which);
#define twi_high(which) high(TWI_OUT_PORT, which);
#define twi_delay() _delay_us(10);		// This can probably be lower as the TDA7433 handles 500kbps but speed is not an issue


void ioinit(void)
{
#if F_CPU <= 1000000UL
  UCSRA = _BV(U2X);						// Slow system clock, double baud rate to improve rate error.
  UBRR = (F_CPU / (8 * 9600UL)) - 1;	// 9600 baud
#else
  UBRR = (F_CPU / (16 * 9600UL)) - 1;	// 9600 baud
#endif
  UCSRB = _BV(TXEN);					// tx enable

  TWI_DDR_PORT = _BV(TWI_SDA) | _BV(TWI_SCL);
}


void twi_start()
{
	twi_high(TWI_SDA);
	twi_delay();
	twi_high(TWI_SCL);
	twi_delay();
	twi_low(TWI_SDA);
	twi_delay();
	twi_low(TWI_SCL);
	twi_delay();
}


void twi_stop() 
{
	twi_low(TWI_SDA);
	twi_delay();
	twi_high(TWI_SCL);
	twi_delay();
	twi_high(TWI_SDA);
	twi_delay();
}

void twi_write(uint8_t d) {
	uint8_t n, c;

	for (n=8; n; n--) {
		if (d & 0x80) {
			twi_high(TWI_SDA);
		}
		else {
			twi_low(TWI_SDA);
		}
		twi_high(TWI_SCL);
		twi_delay();
		d <<= 1;
		twi_low(TWI_SCL);
		twi_delay();
	}
	twi_high(TWI_SDA);
	twi_delay();
	// Uncomment all comments below in this function to print I2C ACK:s
	//TWI_DDR_PORT = _BV(TWI_SCL);
	//printf("SDA: %d\n", PINC);
	twi_high(TWI_SCL);
	twi_delay();
	//printf("SDA: %d\n", PINC);
	twi_low(TWI_SCL);
	//TWI_DDR_PORT = _BV(TWI_SDA) | _BV(TWI_SCL);
	twi_delay();
}


int uart_putchar(char c, FILE *unused)
{
	if (c == '\n') {
		uart_putchar('\r', 0);
	}
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = c;
	return 0;
}


FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);


int main(void)
{
  ioinit();

  stdout = &mystdout;

  printf("Altec Lansing ACS295\n");

  // See the datasheet for TDA7433 for details on the following values

  twi_start();
  twi_write(0x8a);
  twi_write(0x00);	// select input
  twi_write(0x19);
  twi_stop();

  twi_start();
  twi_write(0x8a);
  twi_write(0x01);	// set volume
  twi_write(0x10);
  twi_stop();

  twi_start();
  twi_write(0x8a);
  twi_write(0x02);	// set bass&treble
  twi_write(0x8f);
  twi_stop();

 /*
  twi_start();
  twi_write(0x8a);
  twi_write(0x04);	// set atten LR
  twi_write(0x00);
  twi_stop();

  twi_start();
  twi_write(0x8a);
  twi_write(0x06);	// set atten RR
  twi_write(0x00);
  twi_stop();
*/

/*
  twi_start();
  twi_write(0x8a);
  twi_write(0x01);	// set volume
  twi_write(0x10);
  twi_stop();

  twi_start();
  twi_write(0x8a);
  twi_write(0x00);	// select input
  twi_write(0x01);
  twi_stop();
*/

  printf("Done\n");
  return 0;
}
