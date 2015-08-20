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
 *******************************************************************************
 *
 * This sketch shows how the library can be used, together with UnoJoy, to turn
 * a N64 controller into an USB one that can be used on PCs or on a Raspberry Pi
 * running a N64 emulator :).
 *
 * For details on UnoJoy, see https://github.com/AlanChatham/UnoJoy.
 */

#include <N64Pad.h>
#include <UnoJoy.h>

N64Pad pad;

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

  // Init UnoJoy
  setupUnoJoy ();
}


void loop () {
  pad.read ();

  digitalWrite (LED_BUILTIN, pad.buttons != 0);

  // Convert data for UnoJoy
  dataForController_t controllerData = getBlankDataForController ();

  controllerData.triangleOn = (pad.buttons & N64Pad::BTN_C_LEFT) != 0;
  controllerData.circleOn = (pad.buttons & N64Pad::BTN_C_DOWN) != 0;
  controllerData.squareOn = (pad.buttons & N64Pad::BTN_B) != 0;
  controllerData.crossOn = (pad.buttons & N64Pad::BTN_A) != 0;
  controllerData.dpadUpOn = (pad.buttons & N64Pad::BTN_UP) != 0;
  controllerData.dpadDownOn = (pad.buttons & N64Pad::BTN_DOWN) != 0;
  controllerData.dpadLeftOn = (pad.buttons & N64Pad::BTN_LEFT) != 0;
  controllerData.dpadRightOn = (pad.buttons & N64Pad::BTN_RIGHT) != 0;
  controllerData.l1On = (pad.buttons & N64Pad::BTN_L) != 0;
  controllerData.r1On = (pad.buttons & N64Pad::BTN_R) != 0;
  controllerData.l2On = (pad.buttons & N64Pad::BTN_Z) != 0;
  controllerData.r2On = (pad.buttons & N64Pad::BTN_Z) != 0;
  controllerData.l3On = (pad.buttons & N64Pad::BTN_C_UP) != 0;
  controllerData.r3On = (pad.buttons & N64Pad::BTN_C_RIGHT) != 0;
  controllerData.startOn = (pad.buttons & N64Pad::BTN_START) != 0;

  // Set the analog sticks. UnoJoy wants a uint8_t value like this:
  // - 0 is fully left or up
  // - 255 is fully right or down
  // - 128 is centered
  controllerData.leftStickX = (uint8_t) (pad.x + 128);
  controllerData.leftStickY = (uint8_t) (128 - pad.y);  // N64 is positive DOWN

  // Go!
  setControllerData (controllerData);
}
