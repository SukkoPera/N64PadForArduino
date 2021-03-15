/*******************************************************************************
 * This file is part of N64Pad for Arduino.                                    *
 *                                                                             *
 * Copyright (C) 2015 by SukkoPera                                             *
 *                                                                             *
 * N64Pad is free software: you can redistribute it and/or modify              *
 * it under the terms of the GNU General Public License as published by        *
 * the Free Software Foundation, either version 3 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * N64Pad is distributed in the hope that it will be useful,                   *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU General Public License for more details.                                *
 *                                                                             *
 * You should have received a copy of the GNU General Public License           *
 * along with N64Pad. If not, see <http://www.gnu.org/licenses/>.              *
 *******************************************************************************
 *
 * See the following links for some references on the N64/GC controller
 * protocol:
 * - http://www.int03.co.uk/crema/hardware/gamecube/gc-control.htm
 * - https://www.mixdown.ca/n64dev/
 */

/* Here are a few #defines that will disable some "things happening in the
 * background" while we are polling the controller. The controller protocol is
 * very fast and we need all the speed we can use to catch up with it.
 *
 * All defaults have been tested with Arduino 1.8.12.
 */
#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__) || defined (__AVR_ATmega168__)
// This can be enabled, but does not seem necessary
//~ #define DISABLE_USART
//~ #define DISABLE_MILLIS
#elif defined ( __AVR_ATmega32U4__)
// These are absolutely necessary for reliable readings
#define DISABLE_USB_INTERRUPTS
#define DISABLE_MILLIS

// Don't touch this
#include "usbpause.h"
UsbPause usbMagic;
#endif


#include "N64PadProtocol.h"
#include "pinconfig.h"


// Delay 62.5ns on a 16MHz AtMega
#define NOP __asm__ __volatile__ ("nop\n\t")

#define delay025us() do { \
	NOP; NOP; NOP; NOP; \
} while (0)

#define delay05us() do { \
	delay025us(); \
	delay025us(); \
} while (0)

#define delay1us() do { \
	delay05us(); \
	delay05us(); \
} while (0)

#define delay2us() do { \
	delay1us(); \
	delay1us(); \
} while (0)

#define delay3us() do { \
	delay1us(); \
	delay2us(); \
} while (0)

byte repbuf2[8];
static volatile byte *curByte = &GPIOR2;
static volatile byte *curBit = &GPIOR1;

#ifdef DISABLE_MILLIS
static volatile boolean timeout = false;

ISR (TIMER1_OVF_vect) {
	N64PadProtocol::stopTimer ();
}
#endif

void N64PadProtocol::begin () {
	// Prepare interrupts: INT0 is triggered by pin 2 FALLING
	noInterrupts ();
	//~ EICRA |= (1 << ISC01);
	//~ EICRA &= ~(1 << ISC00);
	PCMSK0 |= (1 << PCINT4);
	interrupts ();
	// Do not enable INT0 here!

#ifdef DISABLE_MILLIS
	// Prepare timer 1
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1B |= (1 << CS11) | (1 << CS10);	// Prescaler = 64 -> 262ms
	TIFR1 |= (1 << TOV1);					// Clear pending interrupt, if any
#endif

	// Signalling output
	DDRC |= (1 << DDC7);
}

inline void N64PadProtocol::enableInterrupt () {
	//~ EIFR |= (1 << INTF0);
	//~ EIMSK |= (1 << INT0);
	PCIFR |= (1 << PCIF0);
	PCICR |= (1 << PCIE0);
}

inline void N64PadProtocol::disableInterrupt () {
	//~ EIMSK &= ~(1 << INT0);
	PCICR &= ~(1 << PCIE0);
}

inline void N64PadProtocol::startTimer () {
	timeout = false;
	TCNT1 = 0;								// counter = 0
	TIMSK1 |= (1 << TOIE1);					// Trigger ISR on overflow
}

inline void N64PadProtocol::stopTimer () {
	timeout = true;
	TIMSK1 &= ~(1 << TOIE1);					// Do not retrigger
}

inline static void sendLow () {
	// Switch pin to output mode, it will be low by default
	PAD_DIR |= (1 << PAD_BIT);
}

inline static void sendHigh () {
	// Switch pin to input mode (Hi-Z), pullups will be disabled by default
	PAD_DIR &= ~(1 << PAD_BIT);
}

// To send a 0 bit the data line is pulled low for 3us and let high for 1us
inline static void sendZero () {
	sendLow ();
	delay3us ();
	sendHigh ();
	delay1us ();
}

// To send a 1 the data line is pulled low for 1us and let high for 3us
inline static void sendOne () {
	sendLow ();
	delay1us ();
	sendHigh ();
	delay3us ();
}

// "Console stop bit" is line low for 1us, and high for 2us (3us total).
inline static void sendStop () {
	sendLow ();
	delay1us ();
	sendHigh ();

	/* Now, we would be supposed to delay 2 us here, but we're cutting it a bit
	 * short since we need to enable interrupts and be sure not to miss the first
	 * falling edge driven by the controller.
	 */
	delay1us ();		
	//~ delay05us ();
}

// This must be implemented like this, as it cannot be too slow, or the controller won't recognize the signal
inline static void sendCmd (const byte *cmdbuf, const byte cmdsz) {
	for (byte j = 0; j < cmdsz; j++) {
		byte cmdbyte = cmdbuf[j];
		for (byte i = 0; i < 8; i++) {
			// MSB first
			if (cmdbyte & 0x80)
				sendOne ();
			else
				sendZero ();
			cmdbyte <<= 1;
		}
	}
	sendStop ();
}

boolean N64PadProtocol::runCommand (const byte *cmdbuf, const byte cmdsz, byte *repbuf, byte repsz) {
	for (byte i = 0; i < repsz; i++)
		repbuf2[i] = 0;

	// Prepare things for the INT0 ISR
	*curBit = 8;
	*curByte = 0;

	// Disable "things happening in the background" as needed
#ifdef DISABLE_MILLIS
	noInterrupts ();
	byte oldTIMSK0 = TIMSK0;
	TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A) | (1 << TOIE0));
	TIFR0 |= (1 << OCF0B) | (1 << OCF0A) | (1 << TOV0);
	interrupts ();
#else
	unsigned long start = millis ();
#endif

#ifdef DISABLE_USART
	byte oldUCSR0B = UCSR0B;
	UCSR0B &= ~((1 << UCSZ02) | (1 << RXB80) | (1 << TXB80));
#endif

#ifdef DISABLE_USB_INTERRUPTS
	usbMagic.pause ();
#endif

#ifdef DISABLE_MILLIS
	// Start timeout timer
	startTimer ();
#endif

	// We can send the command now
	sendCmd (cmdbuf, cmdsz);

	// Enable INT0 handling - QUICK!!!
	enableInterrupt ();

	// OK, just wait for the reply buffer to fill at last
	while (*curByte < repsz
#ifndef DISABLE_MILLIS
		&& millis () - start <= 200
#else
		&& !timeout
#endif
	)
	;

	// Done, ISRs are no longer needed
#ifdef DISABLE_MILLIS
	stopTimer ();			// Even if it already happened, it won't hurt
	TIMSK0 = oldTIMSK0;
#endif
	disableInterrupt ();

	// Reenable things happening in background
#ifdef DISABLE_USB_INTERRUPTS
	usbMagic.resume ();
#endif

#ifdef DISABLE_USART
	UCSR0B = oldUCSR0B;
#endif


		
	// FIXME
	memcpy (repbuf, repbuf2, *curByte);

	return *curByte == repsz;
}
