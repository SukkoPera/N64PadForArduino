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
 *******************************************************************************
 * 
 * GameCube reference:
 * http://www.int03.co.uk/crema/hardware/gamecube/gc-control.html
 */

#include "protocol/N64PadProtocol.h"

class GCPad {
public:
	enum PadButton {
		/* Always 0 = 1 << 15, */
		/* Always 0 = 1 << 14, */
		/* Unknown  = 1 << 13, */
		BTN_START   = 1 << 12,
		BTN_Y       = 1 << 11,
		BTN_X       = 1 << 10,
		BTN_B       = 1 << 9,
		BTN_A       = 1 << 8,
		/* Always 1 = 1 << 7, */
		BTN_L       = 1 << 6,
		BTN_R       = 1 << 5,
		BTN_Z       = 1 << 4,
		BTN_D_UP    = 1 << 3,
		BTN_D_DOWN  = 1 << 2,
		BTN_D_RIGHT = 1 << 1,
		BTN_D_LEFT  = 1 << 0
	};

	// Button status register. Use PadButton values to test this. 1 means pressed.
	uint16_t buttons;

	/* X-Axis coordinate (Growing RIGHT)
	 *
	 * Range for analog position is -128 to 127, however, true GameCube
	 * controller is mechanically limited, so the actual range is about
	 * 20 to 225.
	 */
	uint8_t x;

	/* Y-Axis Coordinate (Growing UP)
	 *
	 * See the comment about x above
	 */
	uint8_t y;

	/* C-Stick X-Axis coordinate (Growing RIGHT)
	 *
	 * Range for analog position is -128 to 127, however, true GameCube
	 * controller is mechanically limited, so the actual range is about
	 * 20 to 225.
	 */
	uint8_t c_x;

	/* C-Stick Y-Axis Coordinate (Growing UP)
	 *
	 * See the comment about c_x above
	 */
	uint8_t c_y;
	
	/* Left trigger
	 * 
	 * Range is 0-255, but full range seems to be hard to reach. The L
	 * button seems to trigger at ~200. Note that this might not be 0 when
	 * fully unpressed!
	 */
	uint8_t left_trigger;

	/* Right trigger
	 *
	 * See the comment about left_trigger above
	 */
	uint8_t right_trigger;

	// This can also be called anytime to reset the controller
	boolean begin ();

	/* Reads the current state of the joystick.
	 *
	 * Note that this functions disables interrupts and runs for 350+ us!
	 */
	boolean read ();

private:
	N64PadProtocol proto;
	
	// Size of a single command in bytes, seems fixed
	static const int COMMAND_SIZE = 3;
	
	enum ProtoCommand {
		CMD_POLL = 0,
		CMD_RUMBLE_ON = 1,
		CMD_RUMBLE_OFF = 2,
		CMD_NUMBER    // Leave at end
	};

	// First byte is expected reply length
	static const byte protoCommands[CMD_NUMBER][COMMAND_SIZE + 1];

	// 8 should enough for all our uses
	byte buf[8];

	// millis() last time controller was polled
	unsigned long last_poll;

	byte *runCommand (const ProtoCommand cmd);
};
