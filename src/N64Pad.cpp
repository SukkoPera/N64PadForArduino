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
 ******************************************************************************/

#include "N64Pad.h"

/* These must follow the order from ProtoCommand, first byte is expected length
 * of reply
 */
const byte N64Pad::protoCommands[CMD_NUMBER][1 + 1] = {
	// CMD_IDENTIFY - Buffer size required: 3 bytes
	{3, 0x00},

	// CMD_POLL - 4
	{4, 0x01},

	// CMD_READ - ?
	{1, 0x02},

	// CMD_WRITE - ?
	{1, 0x03},

	// CMD_RESET - 3
	{3, 0xFF}
};

boolean N64Pad::begin () {
	proto.begin ();

	buttons = 0;
	x = 0;
	y = 0;
	last_poll = 0;
	
	// I'm not sure non-Nintendo controllers return 5
	if (runCommand (CMD_RESET)) {
		last_poll = millis ();
		return buf[0] == 5;
	} else {
		return false;
	}
}

boolean N64Pad::read () {
	boolean ret = true;
	
	if (millis () - last_poll >= MIN_POLL_INTERVAL_MS) {
		if ((ret = (runCommand (CMD_POLL) != NULL))) {
			buttons = ((((uint16_t) buf[0]) << 8) | buf[1]);
			x = (int8_t) buf[2];
			y = (int8_t) buf[3];

			last_poll = millis ();
		}
	}

	return ret;
}

byte *N64Pad::runCommand (const ProtoCommand cmd) {
	byte *ret = NULL;
	if (proto.runCommand (&(protoCommands[(byte) cmd][1]), 1, buf, protoCommands[(byte) cmd][0])) {
		ret = buf;
	}

	return ret;
}
