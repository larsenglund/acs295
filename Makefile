# By Lars Englund 2007
# lars . englund at gmail . com
# Controls a ACS295 subwoofer, see project page for more information:
# http://code.google.com/p/acs295/
#
# $Id$

all:			clean twitest show_size twitest_hex


CC = avr-gcc
CCFLAGS = -Os -mmcu=atmega8 -DF_CPU=8000000UL -DF_OSC=8000000 -DDEBUG=1 -Wall
OBJCOPY = avr-objcopy
PROGRAMMER = avrdude
PROGRAMMERFLAGS = -p m8 -P com9 -c avrispv2 -V -U


twitest:		
				$(CC) $(CCFLAGS) -o twitest.o twitest.c
			
twitest_hex:
				$(OBJCOPY) -O ihex -j.text -j.data twitest.o twitest.hex

prog:			
				$(PROGRAMMER) $(PROGRAMMERFLAGS) flash:w:twitest.hex

clean:
				rm -f *.o *.hex

show_size:		
				avr-size twitest.o

