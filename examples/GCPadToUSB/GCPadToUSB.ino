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
 * This sketch shows how the library can be used, together with the Arduino
 * Joystick Library, to turn a GameCube controller into an USB one that can be
 * used on PCs or on a Raspberry Pi running a GC emulator (one day...) :).
 *
 * For details on the Arduino Joystick Library, see
 * https://github.com/MHeironimus/ArduinoJoystickLibrary.
 */

#include <GCPad.h>
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
const uint8_t ANALOG_MIN_VALUE = 20;

/** \brief Analog sticks maximum value
 *
 * Maximum value reported by analog sticks. This usually means that the stick is
 * fully either at the bottom or right position. Note that some sticks might not
 * get fully up to this value.
 *
 * \sa ANALOGI_MIN_VALUE
 * \sa ANALOG_IDLE_VALUE
 */
const uint8_t ANALOG_MAX_VALUE = 225;

/** \brief Analog sticks idle value
 *
 * Value reported when an analog stick is in the (ideal) center position. Note
 * that old and worn-out sticks might not self-center perfectly when released,
 * so you should never rely on this precise value to be reported.
 *
 * \sa ANALOG_MIN_VALUE
 * \sa ANALOG_MAX_VALUE
 */
const uint8_t ANALOG_IDLE_VALUE = 127;

/** \brief Analog triggers minimum value
 *
 * Minimum value reported by analog triggers.
 *
 * Note that this should actually b -128 to 127, however, the true Nintendo 64
 * controller range is about 63% of it (mechanically limited), so the actual
 * range is about -81 to 81. Worn controllers might report even less, so we are
 * leaving this here for easy customization.
 *
 * \sa TRIGGER_MAX_VALUE
 */
const uint8_t TRIGGER_MIN_VALUE = 10;

/** \brief Analog triggers maximum value
 *
 * Maximum value reported by analog sticks. This means that the stick is fully
 * pressed. Note that some sticks might not get fully up to this value.
 *
 * \sa TRIGGER_MIN_VALUE
 */
const uint8_t TRIGGER_MAX_VALUE = 250;

/** \brief Analog triggers threshold value
 *
 * Trigger buttons appear both as analog accelerator/brake pedals and as digital
 * buttons. The latter will be reported as pressed when the analog value gets
 * past this threshold.
 *
 * \sa TRIGGER_MIN_VALUE
 * \sa TRIGGER_MAX_VALUE
 */
const uint8_t L_R_THRESHOLD = 100;

/******************************************************************************/


GCPad pad;

Joystick_ usbStick (
	JOYSTICK_DEFAULT_REPORT_ID,
	JOYSTICK_TYPE_MULTI_AXIS,
	8,			// buttonCount
	0,			// hatSwitchCount (0-2)
	true,		// includeXAxis
	true,		// includeYAxis
	false,		// includeZAxis
	true,		// includeRxAxis
	true,		// includeRyAxis
	false,		// includeRzAxis
	false,		// includeRudder
	false,		// includeThrottle
	true,		// includeAccelerator
	true,		// includeBrake
	false		// includeSteering
);

bool mapLeftStickToDPad = false;

#define	toDegrees(rad) (rad * 180.0 / PI)

#define deadify(var, thres) (abs (var) > thres ? (var) : 0)


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

	// Check for button A
	pad.read ();
	if ((pad.buttons & GCPad::BTN_A) != 0) {
		mapLeftStickToDPad = true;

		// Signal we got it!
		digitalWrite (LED_BUILTIN, HIGH);
		delay (200);
		digitalWrite (LED_BUILTIN, LOW);
		delay (200);
		digitalWrite (LED_BUILTIN, HIGH);
		delay (200);
		digitalWrite (LED_BUILTIN, LOW);
		delay (200);
		digitalWrite (LED_BUILTIN, HIGH);
		delay (1000);
	}

	// Init Joystick library
	usbStick.begin (false);		// We'll call sendState() manually to minimize lag
	usbStick.setXAxisRange (ANALOG_MIN_VALUE, ANALOG_MAX_VALUE);
	usbStick.setYAxisRange (ANALOG_MIN_VALUE, ANALOG_MAX_VALUE);
	usbStick.setRxAxisRange (ANALOG_MIN_VALUE, ANALOG_MAX_VALUE);
	usbStick.setRyAxisRange (ANALOG_MAX_VALUE, ANALOG_MIN_VALUE);		// Analog is positive UP on controller, DOWN in joystick library
	usbStick.setAcceleratorRange (0, 260);
	usbStick.setBrakeRange (0, 260);
}

// Value axes report when centered
#define CENTER_POS 127

void loop () {
	pad.read ();

	digitalWrite (LED_BUILTIN, pad.buttons != 0);

	// Buttons first!
	usbStick.setButton (0, (pad.buttons & GCPad::BTN_A) != 0);
	usbStick.setButton (1, (pad.buttons & GCPad::BTN_B) != 0);
	usbStick.setButton (2, (pad.buttons & GCPad::BTN_X) != 0);
	usbStick.setButton (3, (pad.buttons & GCPad::BTN_Y) != 0);
	usbStick.setButton (4, (pad.buttons & GCPad::BTN_Z) != 0);
	usbStick.setButton (5, (pad.buttons & GCPad::BTN_START) != 0);

	// Use analog value to trigger L & R
	usbStick.setButton (6, pad.left_trigger > L_R_THRESHOLD);
	usbStick.setButton (7, pad.right_trigger > L_R_THRESHOLD);

	// If you prefer to trigger them on full stop use this
	//~ usbStick.setButton (6, (pad.buttons & GCPad::BTN_L) != 0);
	//~ usbStick.setButton (7, (pad.buttons & GCPad::BTN_R) != 0);

	// L & R are also mapped to accelerator and brake
	usbStick.setBrake (pad.left_trigger);
	usbStick.setAccelerator (pad.right_trigger);

	// D-Pad makes up the X/Y axes
	if ((pad.buttons & GCPad::BTN_D_UP) != 0) {
		usbStick.setYAxis (ANALOG_MIN_VALUE);
	} else if ((pad.buttons & GCPad::BTN_D_DOWN) != 0) {
		usbStick.setYAxis (ANALOG_MAX_VALUE);
	} else {
		usbStick.setYAxis (ANALOG_IDLE_VALUE);
	}

	if ((pad.buttons & GCPad::BTN_D_LEFT) != 0) {
		usbStick.setXAxis (ANALOG_MIN_VALUE);
	} else if ((pad.buttons & GCPad::BTN_D_RIGHT) != 0) {
		usbStick.setXAxis (ANALOG_MAX_VALUE);
	} else {
		usbStick.setXAxis (ANALOG_IDLE_VALUE);
	}

	// Set the analog sticks
	if (!mapLeftStickToDPad) {
		// The analog stick gets mapped to the X/Y rotation axes
		usbStick.setRxAxis (pad.x);
		usbStick.setRyAxis (pad.y);
	} else {
		// TBD
		//~ controllerData.dpadUpOn |= pad.y > (CENTER_POS + STICK_DPAD_EMU_THRESHOLD);
		//~ controllerData.dpadDownOn |= pad.y < (CENTER_POS - STICK_DPAD_EMU_THRESHOLD);
		//~ controllerData.dpadLeftOn |= pad.x < (CENTER_POS - STICK_DPAD_EMU_THRESHOLD);
		//~ controllerData.dpadRightOn |= pad.x > (CENTER_POS + STICK_DPAD_EMU_THRESHOLD);
	}

	// "C" analog is the hat switch
	// We flip coordinates to avoid having to invert them in atan2()
	int8_t rx = ANALOG_IDLE_VALUE - pad.c_x - 1;	// [0 ... 255] -> [127 ... -128]
	rx = deadify (rx, ANALOG_DEAD_ZONE);

	int8_t ry = ANALOG_IDLE_VALUE - pad.c_y - 1;
	ry = deadify (ry, ANALOG_DEAD_ZONE);

	if (rx == 0 && ry == 0) {
		usbStick.setHatSwitch (0, JOYSTICK_HATSWITCH_RELEASE);
	} else {
		/* atan2() will yield something between -PI and +PI, so we
		* add 2*PI first to make it always positive, and then we
		* subtract PI / 2 because setHatSwitch() has 0 degrees at
		* north.
		*
		* Also we would need to invert the arguments to atan2()
		* since setHatSwitch() grows clockwise while radians go the
		* other way, but we have already done that when we
		* calculated rx and ry. Smart, huh?
		*/
		float angle = atan2 (ry, rx) + 2 * PI - PI / 2;
		uint16_t intAngle = ((uint16_t) (toDegrees (angle) + 0.5)) % 360;
		usbStick.setHatSwitch (0, intAngle);
	}

	// All done, send data for real!
	usbStick.sendState ();
}
