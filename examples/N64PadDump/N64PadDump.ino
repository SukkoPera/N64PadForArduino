/*******************************************************************************
 * This file is part of N64Pad for Arduino.                                    *
 *                                                                             *
 * Copyright (C) 2015-2020 by SukkoPera                                        *
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
 * Sketch that shows basic usage of the library, reading the controller status
 * and continuously reporting its state.
 *
 * When looking straight at the connector on the controller cable, pinout is as
 * follows:
 *                                     _
 *                                   /^ ^\
 *                                  /     \
 *                                 |_._._._|
 *                                   1 2 3
 *
 * Where:
 * - 1. Ground
 * - 2. Data
 * - 3. Vcc
 *
 * Connect:
 * - Ground with... well, ground
 * - Data with pin 2 - WARNING: Only this pin is supported for input at the
 *   moment!
 * - Vcc with 3v3 (Warning! The controller is NOT a 5v part!)
 *
 * Finally add a pullup resistor between 3v3 and Data. I used 10 kOhm, but
 * probably anything in the range 1-10k will be fine.
 *
 * (Pardon my sub-par ASCII-art skillz!)
 */

#include <N64Pad.h>

N64Pad pad;

void setup () {
  Serial.begin (115200);

  Serial.println ("Probing for pad...");
  while (!pad.begin ()) {
	Serial.println ("Pad missing");
	delay (1000);
  }
  Serial.println ("Pad detected!");

  pinMode (LED_BUILTIN, OUTPUT);
}


void loop () {
  static uint16_t oldButtons = 0;
  static int8_t oldX = 0, oldY = 0;
  
  pad.read ();

  digitalWrite (LED_BUILTIN, pad.buttons != 0);

  if (pad.buttons != oldButtons || pad.x != oldX || pad.y != oldY) {
	  Serial.print ("Pressed: ");
	  if (pad.buttons & N64Pad::BTN_A)
		Serial.print ("A ");
	  if (pad.buttons & N64Pad::BTN_B)
		Serial.print ("B ");
	  if (pad.buttons & N64Pad::BTN_Z)
		Serial.print ("Z ");
	  if (pad.buttons & N64Pad::BTN_START)
		Serial.print ("Start ");
	  if (pad.buttons & N64Pad::BTN_UP)
		Serial.print ("Up ");
	  if (pad.buttons & N64Pad::BTN_DOWN)
		Serial.print ("Down ");
	  if (pad.buttons & N64Pad::BTN_LEFT)
		Serial.print ("Left ");
	  if (pad.buttons & N64Pad::BTN_RIGHT)
		Serial.print ("Right ");
	  if (pad.buttons & N64Pad::BTN_L)
		Serial.print ("L ");
	  if (pad.buttons & N64Pad::BTN_R)
		Serial.print ("R ");
	  if (pad.buttons & N64Pad::BTN_C_UP)
		Serial.print ("C_Up ");
	  if (pad.buttons & N64Pad::BTN_C_DOWN)
		Serial.print ("C_Down ");
	  if (pad.buttons & N64Pad::BTN_C_LEFT)
		Serial.print ("C_Left ");
	  if (pad.buttons & N64Pad::BTN_C_RIGHT)
		Serial.print ("C_Right ");
	  Serial.println ("");

	  Serial.print ("X = ");
	  Serial.println (pad.x);
	  Serial.print ("Y = ");
	  Serial.println (pad.y);
	  
	  Serial.println ("");

	  oldButtons = pad.buttons;
	  oldX = pad.x;
	  oldY = pad.y;
  }  
}
