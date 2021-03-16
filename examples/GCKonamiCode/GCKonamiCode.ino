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
 * This sketch shows how a precise sequence of button presses can be detected.
 *
 * For details on the Konami code, see https://en.wikipedia.org/wiki/Konami_Code
 */

#include <GCPad.h>

GCPad pad;

void setup () {
	Serial.begin (115200);

	Serial.println ("Probing for pad...");
	if (pad.begin ()) {
		Serial.println ("Pad detected");
	}
	delay (500);

	pinMode (LED_BUILTIN, OUTPUT);

	Serial.println ("Enter the Konami code on your pad (Up, up, down, down, left, right, left, right, B, A)");
}

#define KONAMI_SEQ_LEN 10

word konami_seq[KONAMI_SEQ_LEN] = {
	GCPad::BTN_D_UP,
	GCPad::BTN_D_UP,
	GCPad::BTN_D_DOWN,
	GCPad::BTN_D_DOWN,
	GCPad::BTN_D_LEFT,
	GCPad::BTN_D_RIGHT,
	GCPad::BTN_D_LEFT,
	GCPad::BTN_D_RIGHT,
	GCPad::BTN_B,
	GCPad::BTN_A
};

void loop () {
	static byte konami_cnt = 0;

	pad.read ();

	digitalWrite (LED_BUILTIN, pad.buttons != 0);

	if (konami_cnt % 2 == 0) {
		// Wait for next button to be PRESSED alone
		if (pad.buttons == konami_seq[konami_cnt / 2])
			konami_cnt++;
		else if (pad.buttons != 0)
			konami_cnt = 0;
	} else {
		// Wait for last button pressed to be RELEASED
		if (pad.buttons == 0)
			konami_cnt++;
		else if (pad.buttons != konami_seq[konami_cnt / 2])
			konami_cnt = 0;
	}

	// Check if sequence is complete
	if (konami_cnt == KONAMI_SEQ_LEN * 2) {
		Serial.println ("Konami got you!");
		konami_cnt = 0;
	}
}
