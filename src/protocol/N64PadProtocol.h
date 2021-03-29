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

#ifndef N64PADPROTOCOL_INCLUDED
#define N64PADPROTOCOL_INCLUDED

#include <Arduino.h>

class N64PadProtocol {
public:
	void begin ();

	/* NOTE: This disables interrupts and runs for ~30 us per byte to
	 * exchange!
	 */
	boolean runCommand (const byte *cmdbuf, const byte cmdsz, byte *repbuf, byte repsz);

	// Needs to be public as called from ISR
	static void stopTimer ();

private:
	static void startTimer ();
};

#endif
