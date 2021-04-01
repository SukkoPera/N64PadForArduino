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

//~ #define HAVE_EXTERNAL_PULLUPS

template <byte PIN_DATA>
class N64PadProtocolExtInt: public N64PadProtocol {
private:
	DigitalPin<PIN_DATA> data;
	
public:
	virtual void begin () {
		// Start as INPUT, i.e. Hi-Z, with pull-up enabled, i.e.: HIGH
#ifdef HAVE_EXTERNAL_PULLUPS
		fastPinMode (pinNumber, INPUT);
#else
		fastPinMode (pinNumber, INPUT_PULLUP);
#endif
	}

	//~ virtual void startTimer () {
	//~ }
	
	//~ // Needs to be public as called from ISR
	//~ virtual void stopTimer () {
	//~ }

	virtual void sendLow () {
		// Bring down the line!
#ifndef HAVE_EXTERNAL_PULLUPS
		fastDigitalWrite (pinNumber, LOW);		// Disable pull-up
#endif
		fastPinMode (pinNumber, OUTPUT);		// Implicitly low
	}

	virtual void sendHigh () {
		/* Stop driving the line and let the pull-up resistor do the job
		 * Here's some GOTCHA: depending on the pull-up resistor value, the line
		 * might take quite some time  before it actually reaches a HIGH value.
		 * Since code might rely on the line already being high when this method
		 * returns, we'd better give it some time... But not too much either :).
		 */
#ifdef HAVE_EXTERNAL_PULLUPS
		fastPinMode (pinNumber, INPUT);
#else
		fastPinMode (pinNumber, INPUT_PULLUP);
#endif
		//~ delayMicroseconds (3);
	}
	
	//~ inline operator bool () const __attribute__((always_inline)) {
		//~ return fastDigitalRead (pinNumber);
	//~ }

boolean N64PadProtocol::runCommand (const byte *cmdbuf, const byte cmdsz, byte *repbuf, byte repsz) {
	// Clear incoming buffer
	for (byte i = 0; i < repsz; i++)
		repbuf2[i] = 0;

	// Prepare things for the INT0 ISR
	*curBit = 8;
	*curByte = 0;

	unsigned long start = micros ();

	// We can send the command now
	sendCmd (cmdbuf, cmdsz);

	// Enable interrupt handling - QUICK!!!
	enableInterrupt ();

	// OK, just wait for the reply buffer to fill at last
	while (*curByte < repsz	&& micros () - start <= COMMAND_TIMEOUT)
		;

	// Done, ISRs are no longer needed
	disableInterrupt ();
		
	// FIXME
	memcpy (repbuf, repbuf2, *curByte);

	return *curByte == repsz;
}

};
