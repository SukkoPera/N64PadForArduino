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

#include "protocol/N64PadProtocol.h"

class N64Pad {
public:
	const byte MIN_POLL_INTERVAL_MS = 1000U / 60U;

	enum PadButton {
		BTN_A       = 1 << 15,
		BTN_B       = 1 << 14,
		BTN_Z       = 1 << 13,
		BTN_START   = 1 << 12,
		BTN_UP      = 1 << 11,
		BTN_DOWN    = 1 << 10,
		BTN_LEFT    = 1 << 9,
		BTN_RIGHT   = 1 << 8,
		BTN_LRSTART = 1 << 7,	// This is set when L+R+Start are pressed (and BTN_START is not)
		/* Unused   = 1 << 6, */
		BTN_L       = 1 << 5,
		BTN_R       = 1 << 4,
		BTN_C_UP    = 1 << 3,
		BTN_C_DOWN  = 1 << 2,
		BTN_C_LEFT  = 1 << 1,
		BTN_C_RIGHT = 1 << 0
	};

	// Button status register. Use PadButton values to test this. 1 means pressed.
	uint16_t buttons;

	/* X-Axis coordinate (Positive RIGHT)
	 *
	 * Range for analog position is -128 to 127, however, true Nintendo 64
	 * controller range is about 63% of it (mechanically limited), so the actual
	 * range is about -81 to 81 (less for worn-out controllers).
	 */
	int8_t x;

	/* Y-Axis Coordinate (Positive UP)
	 *
	 * See the comment about x above
	 */
	int8_t y;

	// This can also be called anytime to reset the controller
	boolean begin ();

	/* Reads the current state of the joystick.
	 *
	 * Note that this functions disables interrupts and runs for 160+ us!
	 */
	boolean read ();

private:
	N64PadProtocol proto;
	
	enum ProtoCommand {
		CMD_IDENTIFY = 0,
		CMD_POLL,
		CMD_READ,
		CMD_WRITE,
		CMD_RESET,

		CMD_NUMBER    // Leave at end
	};

	// First byte is expected reply length, second byte is actual command byte
	static const byte protoCommands[CMD_NUMBER][1 + 1];

	// 4 is enough for all our uses
	byte buf[4];

	// millis() last time controller was polled
	unsigned long last_poll;
	
	byte *runCommand (const ProtoCommand cmd);
};
