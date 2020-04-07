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
 * This sketch shows how the library can be used, together with the Arduino
 * Joystick Library, to turn a N64 controller into an USB one that can be used
 * on PCs or on a Raspberry Pi running a N64 emulator :).
 *
 * For details on the Arduino Joystick Library, see
 * https://github.com/MHeironimus/ArduinoJoystickLibrary.
 */

#include <N64Pad.h>
#include <Joystick.h>

/** \brief Dead zone for analog sticks
 *  
 * If the analog stick moves less than this value from the center position, it
 * is considered still.
 * 
 * \sa ANALOG_IDLE_VALUE
 */
const byte ANALOG_DEAD_ZONE = 20U;

/** \brief Analog sticks minimum value
 * 
 * Minimum value reported by analog sticks. This usually means that the stick is
 * fully either at the top or left position. Note that some sticks might not get
 * fully down to this value.
 *
 * Note that this should actually b -128 to 127, however, the true Nintendo 64
 * controller range is about 63% of it (mechanically limited), so the actual
 * range is about -81 to 81. Worn controllers might report even less, so we are
 * leaving this here for easy customization.
 *
 * \sa ANALOG_MAX_VALUE
 * \sa ANALOG_IDLE_VALUE
 */
const int8_t ANALOG_MIN_VALUE = -80;

/** \brief Analog sticks maximum value
 * 
 * Maximum value reported by analog sticks. This usually means that the stick is
 * fully either at the bottom or right position. Note that some sticks might not
 * get fully up to this value.
 *
 * \sa ANALOGI_MIN_VALUE
 * \sa ANALOG_IDLE_VALUE
 */
const int8_t ANALOG_MAX_VALUE = 80;

/** \brief Analog sticks idle value
 * 
 * Value reported when an analog stick is in the (ideal) center position. Note
 * that old and worn-out sticks might not self-center perfectly when released,
 * so you should never rely on this precise value to be reported.
 *
 * \sa ANALOG_MIN_VALUE
 * \sa ANALOG_MAX_VALUE
 */
const int8_t ANALOG_IDLE_VALUE = 0;

/******************************************************************************/

N64Pad pad;

Joystick_ usbStick (
	JOYSTICK_DEFAULT_REPORT_ID,
	JOYSTICK_TYPE_MULTI_AXIS,
	10,			// buttonCount
	0,			// hatSwitchCount (0-2)
	true,		// includeXAxis
	true,		// includeYAxis
	false,		// includeZAxis
	true,		// includeRxAxis
	true,		// includeRyAxis
	false,		// includeRzAxis
	false,		// includeRudder
	false,		// includeThrottle
	false,		// includeAccelerator
	false,		// includeBrake
	false		// includeSteering
);

void setup () {
  pinMode (LED_BUILTIN, OUTPUT);

  if (!pad.begin ()) {
    // Report error, somehow
    while (1) {
        digitalWrite (LED_BUILTIN, HIGH);
        delay (300);
        digitalWrite (LED_BUILTIN, LOW);
        delay (700);
    }
  }

  // Init Joystick library
  usbStick.begin (false);		// We'll call sendState() manually to minimize lag
  usbStick.setXAxisRange (ANALOG_MIN_VALUE, ANALOG_MAX_VALUE);
  usbStick.setYAxisRange (ANALOG_MIN_VALUE, ANALOG_MAX_VALUE);
  usbStick.setRxAxisRange (ANALOG_MIN_VALUE, ANALOG_MAX_VALUE);
  usbStick.setRyAxisRange (ANALOG_MAX_VALUE, ANALOG_MIN_VALUE);		// Analog is positive UP on controller, DOWN in joystick library
}


void loop () {
  pad.read ();

  /* Note that this slows down the controller response, so you might want to
   * comment it out!
   */
  digitalWrite (LED_BUILTIN, pad.buttons != 0);

  // Buttons first!
  usbStick.setButton (0, (pad.buttons & N64Pad::BTN_B) != 0);
  usbStick.setButton (1, (pad.buttons & N64Pad::BTN_A) != 0);
  usbStick.setButton (2, (pad.buttons & N64Pad::BTN_C_LEFT) != 0);
  usbStick.setButton (3, (pad.buttons & N64Pad::BTN_C_DOWN) != 0);
  usbStick.setButton (4, (pad.buttons & N64Pad::BTN_C_UP) != 0);
  usbStick.setButton (5, (pad.buttons & N64Pad::BTN_C_RIGHT) != 0);
  usbStick.setButton (6, (pad.buttons & N64Pad::BTN_L) != 0);
  usbStick.setButton (7, (pad.buttons & N64Pad::BTN_R) != 0);
  usbStick.setButton (8, (pad.buttons & N64Pad::BTN_Z) != 0);
  usbStick.setButton (9, (pad.buttons & N64Pad::BTN_START) != 0);

  // D-Pad makes up the X/Y axes
  if ((pad.buttons & N64Pad::BTN_UP) != 0) {
  	usbStick.setYAxis (ANALOG_MIN_VALUE);
  } else if ((pad.buttons & N64Pad::BTN_DOWN) != 0) {
  	usbStick.setYAxis (ANALOG_MAX_VALUE);
  } else {
  	usbStick.setYAxis (ANALOG_IDLE_VALUE);
  }
  
  if ((pad.buttons & N64Pad::BTN_LEFT) != 0) {
  	usbStick.setXAxis (ANALOG_MIN_VALUE);
  } else if ((pad.buttons & N64Pad::BTN_RIGHT) != 0) {
  	usbStick.setXAxis (ANALOG_MAX_VALUE);
  } else {
  	usbStick.setXAxis (ANALOG_IDLE_VALUE);
  }
  
  // The analog stick gets mapped to the X/Y rotation axes
  usbStick.setRxAxis (pad.x);
  usbStick.setRyAxis (pad.y);

  // All done, send data for real!
  usbStick.sendState ();
}
