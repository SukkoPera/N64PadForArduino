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

#ifndef N64_CUSTOM_INTMAN
#include "pinconfig.h"
#endif

#include <Arduino.h>
#include <DigitalIO.h>
#include "N64PadProtocol.h"
#include "N64InterruptManager.h"
#include "N64Options.h"


#define N64P4A_HAVE_EXTERNAL_PULLUPS

extern byte isrBuf[8];
static volatile byte *curByte = &GPIOR2;
static volatile byte *curBit = &GPIOR1;

template <byte PIN_DATA, typename INTMAN>
class N64PadProtocolExtInt: public N64PadProtocol {
public:
	virtual void begin () {
		N64PadProtocol::begin ();
		
		// Start as INPUT, i.e. Hi-Z, with pull-up enabled, i.e.: HIGH
#ifdef N64P4A_HAVE_EXTERNAL_PULLUPS
		fastPinMode (PIN_DATA, INPUT);
#else
		fastPinMode (PIN_DATA, INPUT_PULLUP);
#endif

		// Prepare interrupts
		noInterrupts ();
		this -> intMan.prepareInterrupt ();
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

		// Prepare things for the ISR
		*curBit = 8;
		*curByte = 0;

		unsigned long start = micros ();

		// We can send the command now
		sendCmd (cmdbuf, cmdsz);

		// Enable interrupt handling - QUICK!!!
		this -> intMan.enableInterrupt ();

		// OK, just wait for the reply buffer to fill at last
		while (*curByte < repsz	&& micros () - start <= N64_COMMAND_TIMEOUT)
			;

		// Done, ISR is no longer needed
		this -> intMan.disableInterrupt ();
			
		memcpy (repbuf, isrBuf, *curByte);

		return *curByte == repsz;
	}

protected:
	INTMAN intMan;
	
	inline __attribute__((always_inline))
	virtual void sendLow () override final {		// This attribute + "final" combo is what makes the whole thing inline
		// Bring down the line!
#ifndef N64P4A_HAVE_EXTERNAL_PULLUPS
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
#ifdef N64P4A_HAVE_EXTERNAL_PULLUPS
		fastPinMode (PIN_DATA, INPUT);
#else
		fastPinMode (PIN_DATA, INPUT_PULLUP);
#endif
		//~ while (!fastDigitalRead (PIN_DATA))
			//~ ;
	}
};
