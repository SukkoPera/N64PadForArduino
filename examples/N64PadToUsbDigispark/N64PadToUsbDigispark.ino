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
 * This sketch shows how the library can be used to turn a N64 controller into
 * an USB one that can be used on PCs or on a Raspberry Pi running a N64
 * emulator with a simple and cheap Digispark board :).
 *
 * For details on the Digispark, see http://digistump.com/products/1.
 *
 * NOTE: This does not currently seem 100% reliable, as it has occasional
 * hiccups. I'm not sure why, it might be due to the Digispark core being a bit
 * outdated or maybe the software USB stack interferes with the library. I don't
 * know but well, ATtiny85 is not a supported platform anyway, take this as a
 * gift ;).
 */

#include <N64Pad.h>
#include <DigiJoystick.h>

N64Pad pad;

void setup () {
	// We'll never touch these, let's leave them halfway through all along
	DigiJoystick.setSLIDER ((byte) 128);
	DigiJoystick.setZROT ((byte) 128);
}

void loop () {
	static boolean haveController = false;

	if (!haveController) {
		if (pad.begin ()) {
			// Controller detected!
			haveController = true;
		} else {
			DigiJoystick.delay (333);
		}
	} else {
		if (!pad.read ()) {
			// Controller lost :(
			haveController = false;
		} else {
			// Map buttons!
			byte buttonsLow = 0, buttonsHigh = 0;
			if ((pad.buttons & N64Pad::BTN_B) != 0)
				buttonsLow |= 1 << 0;
			if ((pad.buttons & N64Pad::BTN_A) != 0)
				buttonsLow |= 1 << 1;
			if ((pad.buttons & N64Pad::BTN_C_LEFT) != 0)
				buttonsLow |= 1 << 2;
			if ((pad.buttons & N64Pad::BTN_C_DOWN) != 0)
				buttonsLow |= 1 << 3;
			if ((pad.buttons & N64Pad::BTN_C_UP) != 0)
				buttonsLow |= 1 << 4;
			if ((pad.buttons & N64Pad::BTN_C_RIGHT) != 0)
				buttonsLow |= 1 << 5;
			if ((pad.buttons & N64Pad::BTN_L) != 0)
				buttonsLow |= 1 << 6;
			if ((pad.buttons & N64Pad::BTN_R) != 0)
				buttonsLow |= 1 << 7;
			if ((pad.buttons & N64Pad::BTN_Z) != 0)
				buttonsHigh |= 1 << 0;
			if ((pad.buttons & N64Pad::BTN_START) != 0)
				buttonsHigh |= 1 << 1;
			DigiJoystick.setButtons (buttonsLow, buttonsHigh);

			// D-Pad makes up the X/Y axes
			if ((pad.buttons & N64Pad::BTN_UP) != 0) {
				DigiJoystick.setY ((byte) 0);
			} else if ((pad.buttons & N64Pad::BTN_DOWN) != 0) {
				DigiJoystick.setY ((byte) 255);
			} else {
				DigiJoystick.setY ((byte) 128);
			}

			if ((pad.buttons & N64Pad::BTN_LEFT) != 0) {
				DigiJoystick.setX ((byte) 0);
			} else if ((pad.buttons & N64Pad::BTN_RIGHT) != 0) {
				DigiJoystick.setX ((byte) 255);
			} else {
				DigiJoystick.setX ((byte) 128);
			}

			// The analog stick gets mapped to the X/Y rotation axes
			DigiJoystick.setXROT ((byte) (pad.x + (byte) 128));
			DigiJoystick.setYROT ((byte) (((byte) 128) - pad.y));	// Y grows the opposite way!
		}
	}

	// Send data for real - Call this at least every 50ms in any case
	DigiJoystick.update ();
}
