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

#pragma once

#include <Arduino.h>

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


class N64PadProtocol {
public:
	virtual void begin () {
	}

	virtual boolean runCommand (const byte *cmdbuf, const byte cmdsz, byte *repbuf, const byte repsz) = 0;

private:
	// To send a 0 bit the data line is pulled low for 3us and let high for 1us
	inline __attribute__((always_inline))
	void sendZero () {
		sendLow ();
		delay3us ();
		sendHigh ();
		delay1us ();
	}

	// To send a 1 the data line is pulled low for 1us and let high for 3us
	inline __attribute__((always_inline))
	void sendOne () {
		sendLow ();
		delay1us ();
		sendHigh ();
		delay3us ();
	}

	// "Console stop bit" is line low for 1us, and high for 2us (3us total).
	inline __attribute__((always_inline))
	void sendStop () {
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

protected:	
	//~ inline void sendCmd (const byte *cmdbuf, const byte cmdsz);
	// This must be implemented like this, as it cannot be too slow, or the controller won't recognize the signal
	inline void sendCmd (const byte *cmdbuf, const byte cmdsz) {
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

	virtual void sendLow () = 0;

	virtual void sendHigh () = 0;
};
