/*******************************************************************************
 * This file is part of N64Pad for Arduino.                                    *
 *                                                                             *
 * Copyright (C) 2015-2021 by SukkoPera                                        *
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
 ******************************************************************************/

#pragma once

#include <Arduino.h>
#include <DigitalIO.h>
#include "N64PadProtocol.h"
#include "pinconfig.h"
#include "N64Options.h"

//~ #define HAVE_EXTERNAL_PULLUPS

extern byte isrBuf[8];
static volatile byte *curByte = &GPIOR2;
static volatile byte *curBit = &GPIOR1;

template <byte PIN_DATA>
class N64PadProtocolExtInt: public N64PadProtocol {
public:
	virtual void begin () override {
		// Start as INPUT, i.e. Hi-Z, with pull-up enabled, i.e.: HIGH
#ifdef HAVE_EXTERNAL_PULLUPS
		fastPinMode (PIN_DATA, INPUT);
#else
		fastPinMode (PIN_DATA, INPUT_PULLUP);
#endif

		// Prepare interrupts: INT0 is triggered by pin 2 FALLING
		noInterrupts ();
		prepareInterrupt ();
		interrupts ();
		// Do not enable interrupt here!
	}

	//~ inline operator bool () const __attribute__((always_inline)) {
		//~ return fastDigitalRead (PIN_DATA);
	//~ }

	virtual boolean runCommand (const byte *cmdbuf, const byte cmdsz, byte *repbuf, const byte repsz) override {
		// Clear incoming buffer
		for (byte i = 0; i < repsz; i++)
			isrBuf[i] = 0;

		// Prepare things for the INT0 ISR
		*curBit = 8;
		*curByte = 0;

		unsigned long start = micros ();

		// We can send the command now
		sendCmd (cmdbuf, cmdsz);

		// Enable interrupt handling - QUICK!!!
		enableInterrupt ();

		// OK, just wait for the reply buffer to fill at last
		while (*curByte < repsz	&& micros () - start <= N64_COMMAND_TIMEOUT)
			;

		// Done, ISRs are no longer needed
		disableInterrupt ();
			
		memcpy (repbuf, isrBuf, *curByte);

		return *curByte == repsz;
	}

protected:
	inline __attribute__((always_inline))
	virtual void sendLow () override final {
		// Bring down the line!
#ifndef HAVE_EXTERNAL_PULLUPS
		fastDigitalWrite (PIN_DATA, LOW);		// Disable pull-up
#endif
		fastPinMode (PIN_DATA, OUTPUT);		// Implicitly low
	}

	inline __attribute__((always_inline))
	virtual void sendHigh () override final {
		/* Stop driving the line and let the pull-up resistor do the job
		 * Here's some GOTCHA: depending on the pull-up resistor value, the line
		 * might take quite some time  before it actually reaches a HIGH value.
		 * Since code might rely on the line already being high when this method
		 * returns, we'd better give it some time... But not too much either :).
		 */
#ifdef HAVE_EXTERNAL_PULLUPS
		fastPinMode (PIN_DATA, INPUT);
#else
		fastPinMode (PIN_DATA, INPUT_PULLUP);
#endif
		//~ delayMicroseconds (3);
	}
};
