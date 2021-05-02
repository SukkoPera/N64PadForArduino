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
#include "N64PadProtocolExtInt.h"

#include "usbpause.h"
extern UsbPause usbMagic;

extern volatile boolean timeout;

inline void startTimer () {
	timeout = false;
	TCNT1 = 0;								// counter = 0
	TIFR1 |= (1 << OCF1A);					// Clear pending interrupt, if any
	TIMSK1 |= (1 << OCIE1A);				// Trigger ISR on output Compare Match A
}

inline void stopTimer () {
	timeout = true;
	TIMSK1 &= ~(1 << OCIE1A);				// Do not retrigger
}

template <byte PIN_DATA>
class N64PadProtocolExtIntLeo: public N64PadProtocolExtInt<PIN_DATA> {
public:
	virtual void begin () {
		N64PadProtocolExtInt<PIN_DATA>::begin ();
		
		/* Since we'll disable the timer interrupt while we are polling the
		 * controller, we need some other way to trigger a read timeout, let's
		 * use Timer1
		 */
		TCCR1A = 0;
		TCCR1B = 0;
		TCCR1B |= (1 << WGM12);			// Clear Timer on Compare (CTC)
		TCCR1B |= (1 << CS10);			// Prescaler = 1
		OCR1A = 7999;					// 16000000/((7999+1)*1) => 2000Hz/200us
	}

	virtual boolean runCommand (const byte *cmdbuf, const byte cmdsz, byte *repbuf, const byte repsz) override {
		for (byte i = 0; i < repsz; i++)
			isrBuf[i] = 0;

		// Prepare things for the INT0 ISR
		*curBit = 8;
		*curByte = 0;

		/* To get reliable readings on the Leonardo we need to disable some "things
		 * happening in the background" while we are polling the controller.
		 * 
		 * First, we disable the millis() timer...
		 */
		noInterrupts ();
		const byte oldTIMSK0 = TIMSK0;
		TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A) | (1 << TOIE0));
		TIFR0 |= (1 << OCF0B) | (1 << OCF0A) | (1 << TOV0);
		interrupts ();

		// ... then we disable USB interrupts
		usbMagic.pause ();

		/* Since we disable the timer interrupt we need some other way to trigger a
		 * read timeout, let's use timer 1
		 */
		// Start timeout timer
		startTimer ();

		// We can send the command now
		this -> sendCmd (cmdbuf, cmdsz);

		// Enable interrupt handling - QUICK!!!
		enableInterrupt ();

		// OK, just wait for the reply buffer to fill at last
		while (*curByte < repsz && !timeout)
			;

		// Done, ISRs are no longer needed
		stopTimer ();			// Even if it already happened, it won't hurt
		
		disableInterrupt ();

		// Reenable things happening in background
		TIMSK0 = oldTIMSK0;
		usbMagic.resume ();

		memcpy (repbuf, isrBuf, *curByte);

		return *curByte == repsz;
	}
};
