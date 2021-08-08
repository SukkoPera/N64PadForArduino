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
 * Sketch that shows basic usage of the library, reading the controller status
 * and continuously reporting its state.
 *
 * When looking straight at the connector on the controller cable, you will see
 * two rows of pins. Let's number the top row 3, 2, 1 from left to right and the
 * bottom row 6, 5, 4. Then:
 * 
 * 1. +5V (for rumble motor only)
 * 2. Data
 * 3. Ground for +5V
 * 4. Ground for 3.3V
 * 5. ?
 * 6. +3.3V
 * 7. Ground (for shielding)
 * 
 * There is a picture in the extras directory that will help with identifying
 * which pin is which.
 *
 * Connect:
 * - Ground for +3.3V
 * - +3.3V
 * - Data with pin 2 - WARNING: Only this pin is supported for input at the
 *   moment!
 * - Optionally +5V, if you want to use rumble functions
 *
 * Finally add a pullup resistor between +3.3V and Data. I used 10 kOhm, but
 * probably anything in the range 1-10k will be fine. This might even be
 * unnecessary, as some controllers have an internal one, so just try! But again,
 * note that it's +3.3V, NOT +5V!
 *
 * Note that I have distinghuished the grounds, as to get my official controller
 * to work, I had to connect pin 4, it didn't work with pin 3 only!
 */

//~ #include <protocol/pinconfig.h>
#include <GCPad.h>

#if defined(__AVR_ATmega328P__) || defined (__AVR_ATmega328__) || defined (__AVR_ATmega168__) || defined (__AVR_ATtiny88__) || defined (__AVR_ATtiny48__)
	#include <protocol/N64PadProtocolExtInt.h>
	typedef N64PadProtocolExtInt<N64PAD_PIN_NUMBER, DefaultInterruptManager> N64Driver;
#elif defined (__AVR_ATmega32U4__)
	#include <protocol/N64PadProtocolExtIntLeo.h>
	typedef N64PadProtocolExtIntLeo<N64PAD_PIN_NUMBER, DefaultInterruptManager> N64Driver;
#endif

N64Driver driver;
GCPad pad;

void setup () {
	Serial.begin (115200);
	while (!Serial)
		;

	pinMode (LED_BUILTIN, OUTPUT);

	driver.begin ();

	Serial.println ("Ready!");
}


void loop () {
	static boolean haveController = false;
	static uint16_t oldButtons = 0;
	static int8_t oldX = 0, oldY = 0;

	if (!haveController) {
		if (pad.begin (driver)) {
			// Controller detected!
			digitalWrite (LED_BUILTIN, HIGH);
			Serial.println (F("Controller found!"));
			haveController = true;
		} else {
			delay (333);
		}
	} else {
		if (!pad.read ()) {
			// Controller lost :(
			digitalWrite (LED_BUILTIN, LOW);
			Serial.println (F("Controller lost :("));
			haveController = false;
		} else {
			if (pad.buttons != oldButtons || pad.x != oldX || pad.y != oldY) {
				Serial.print ("Pressed: ");
				if (pad.buttons & GCPad::BTN_A)
					Serial.print ("A ");
				if (pad.buttons & GCPad::BTN_B)
					Serial.print ("B ");
				if (pad.buttons & GCPad::BTN_X)
					Serial.print ("X ");
				if (pad.buttons & GCPad::BTN_Y)
					Serial.print ("Y ");
				if (pad.buttons & GCPad::BTN_Z)
					Serial.print ("Z ");
				if (pad.buttons & GCPad::BTN_START)
					Serial.print ("Start ");
				if (pad.buttons & GCPad::BTN_D_UP)
					Serial.print ("Up ");
				if (pad.buttons & GCPad::BTN_D_DOWN)
					Serial.print ("Down ");
				if (pad.buttons & GCPad::BTN_D_LEFT)
					Serial.print ("Left ");
				if (pad.buttons & GCPad::BTN_D_RIGHT)
					Serial.print ("Right ");
				if (pad.buttons & GCPad::BTN_L)
					Serial.print ("L ");
				if (pad.buttons & GCPad::BTN_R)
					Serial.print ("R ");
				Serial.println ("");

				Serial.print ("X = ");
				Serial.println (pad.x);
				Serial.print ("Y = ");
				Serial.println (pad.y);
				
				Serial.print ("C-Stick X = ");
				Serial.println (pad.c_x);
				Serial.print ("C-Stick Y = ");
				Serial.println (pad.c_y);
				
				Serial.print ("Left Trigger = ");
				Serial.println (pad.left_trigger);
				Serial.print ("Right Trigger = ");
				Serial.println (pad.right_trigger);
				
				Serial.println ("");

				oldButtons = pad.buttons;
				oldX = pad.x;
				oldY = pad.y;
			}
		}
	}
}
