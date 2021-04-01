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

class N64PadProtocol {
public:
	virtual void begin ();

	virtual void sendLow () = 0;

	virtual void sendHigh () = 0;

	virtual boolean runCommand (const byte *cmdbuf, const byte cmdsz, byte *repbuf, byte repsz) = 0;

private:
	inline void sendZero ();
	
	inline void sendOne ();

	inline void sendStop ();
	
	inline void sendCmd (const byte *cmdbuf, const byte cmdsz);
};
