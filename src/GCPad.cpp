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

#include "GCPad.h"

/* These must follow the order from ProtoCommand, first byte is expected length
 * of reply
 */
const byte GCPad::protoCommands[CMD_NUMBER][COMMAND_SIZE + 1] = {
	// CMD_POLL - Buffer size required: 8 bytes
	{8, 0x40, 0x03, 0x02},

	// CMD_RUMBLE_ON - Do we even have a reply?
	{1, 0x40, 0x00, 0x01},

	// CMD_RUMBLE_OFF - Ditto
	{1, 0x40, 0x00, 0x00}
};

boolean GCPad::begin () {
	buttons = 0;
	x = 0;
	y = 0;
	c_x = 0;
	c_y = 0;
	left_trigger = 0;
	right_trigger = 0;
	
	last_poll = 0;

	// It seems we need nothing special
	return true;
}

boolean GCPad::read () {
	boolean ret = true;
	
	if (last_poll == 0 || millis () - last_poll >= 10) {
		if ((ret = (runCommand (CMD_POLL) != NULL))) {
			// The mask makes sure unused bits are 0, some seem to be always 1
			buttons = ((((uint16_t) buf[0]) << 8) | buf[1]) & ~(0xE080);
			x = buf[2];
			y = buf[3];
			c_x = buf[4];
			c_y = buf[5];
			left_trigger = buf[6];
			right_trigger = buf[7];

			last_poll = millis ();
		}
	}

	return ret;
}

byte *GCPad::runCommand (const ProtoCommand cmd) {
	byte *ret = NULL;
	if (proto.runCommand (protoCommands[cmd] + 1, COMMAND_SIZE, buf, protoCommands[cmd][0])) {
		ret = buf;
	}

	return ret;
}
