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


#include "N64PadProtocol.h"

/* A read will be considered failed if it hasn't completed within this amount of
 * microseconds. The N64/GC protocol takes 4us per bit. The longest command
 * reply we support is GC's poll command which returns 8 bytes, so this must be
 * at least 8 * 8 * 4 = 256 us plus some margin. Note that this is only used
 * when DISABLE_MILLIS is NOT defined, when it is a hw timer is used, which is
 * initialized in begin(), so if you change this make sure to tune the value
 * there accordingly, too.
 */
#define COMMAND_TIMEOUT 300

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


void N64PadProtocol::begin () {
	//~ // Prepare interrupts: INT0 is triggered by pin 2 FALLING
	//~ noInterrupts ();
	//~ prepareInterrupt ();
	//~ interrupts ();
	//~ // Do not enable interrupt here!

//~ #ifdef DISABLE_MILLIS
	//~ /* Since we disable the timer interrupt we need some other way to trigger a
	 //~ * read timeout, let's use timer 1
	 //~ */
	//~ TCCR1A = 0;
	//~ TCCR1B = 0;
	//~ TCCR1B |= (1 << WGM12);					// Clear Timer on Compare (CTC)
	//~ TCCR1B |= (1 << CS10);					// Prescaler = 1
	//~ OCR1A = 4799;							// 16000000/((4799+1)*1) => 3333Hz/300us
//~ #endif

	// Signalling output
	//~ DDRC |= (1 << DDC7);
}

// To send a 0 bit the data line is pulled low for 3us and let high for 1us
void N64PadProtocol::sendZero () {
	sendLow ();
	delay3us ();
	sendHigh ();
	delay1us ();
}

// To send a 1 the data line is pulled low for 1us and let high for 3us
void N64PadProtocol::sendOne () {
	sendLow ();
	delay1us ();
	sendHigh ();
	delay3us ();
}

// "Console stop bit" is line low for 1us, and high for 2us (3us total).
void N64PadProtocol::sendStop () {
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
void N64PadProtocol::sendCmd (const byte *cmdbuf, const byte cmdsz) {
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
