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
 * a GameCube controller into an USB one that can be used on PCs or on a
 * Raspberry Pi running a GC emulator (one day...) :).
 *
 * For details on UnoJoy, see https://github.com/AlanChatham/UnoJoy.
 */

#include <GCPad.h>
#include <UnoJoy.h>

// Threshold to report L and R as pressed
#define L_R_THRESHOLD 100

#define STICK_DPAD_EMU_THRESHOLD 30


GCPad pad;

bool mapLeftStickToDPad = false;

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

  // Init UnoJoy
  setupUnoJoy ();
}

// Value axes report when centered
#define CENTER_POS 127

void loop () {
  pad.read ();

  digitalWrite (LED_BUILTIN, pad.buttons != 0);

  // Convert data for UnoJoy
  dataForController_t controllerData = getBlankDataForController ();

  controllerData.crossOn = (pad.buttons & GCPad::BTN_A) != 0;
  controllerData.squareOn = (pad.buttons & GCPad::BTN_B) != 0;
  controllerData.triangleOn = (pad.buttons & GCPad::BTN_Y) != 0;
  controllerData.circleOn = (pad.buttons & GCPad::BTN_X) != 0;
  controllerData.dpadUpOn = (pad.buttons & GCPad::BTN_D_UP) != 0;
  controllerData.dpadDownOn = (pad.buttons & GCPad::BTN_D_DOWN) != 0;
  controllerData.dpadLeftOn = (pad.buttons & GCPad::BTN_D_LEFT) != 0;
  controllerData.dpadRightOn = (pad.buttons & GCPad::BTN_D_RIGHT) != 0;
  
  // Use analog value to trigger L & R
  controllerData.l1On = pad.left_trigger > L_R_THRESHOLD;
  controllerData.r1On = pad.right_trigger > L_R_THRESHOLD;
  
  // If you prefer to trigger them on full stop use this
  //~ controllerData.l1On = (pad.buttons & GCPad::BTN_L) != 0;
  //~ controllerData.r1On = (pad.buttons & GCPad::BTN_R) != 0;
  
  //~ controllerData.l2On = (pad.buttons & GCPad::BTN_Z) != 0;
  controllerData.r2On = (pad.buttons & GCPad::BTN_Z) != 0;
  //~ controllerData.l3On = (pad.buttons & GCPad::BTN_C_UP) != 0;
  //~ controllerData.r3On = (pad.buttons & GCPad::BTN_C_RIGHT) != 0;
  controllerData.startOn = (pad.buttons & GCPad::BTN_START) != 0;

  // Set the analog sticks. UnoJoy wants a uint8_t value like this:
  // - 0 is fully left or up
  // - 255 is fully right or down
  // - 128 is centered
  if (!mapLeftStickToDPad) {
    controllerData.leftStickX = pad.x;
    controllerData.leftStickY = 255 - pad.y;  // GC is full-range UP
  } else {
    controllerData.dpadUpOn |= pad.y > (CENTER_POS + STICK_DPAD_EMU_THRESHOLD);
    controllerData.dpadDownOn |= pad.y < (CENTER_POS - STICK_DPAD_EMU_THRESHOLD);
    controllerData.dpadLeftOn |= pad.x < (CENTER_POS - STICK_DPAD_EMU_THRESHOLD);
    controllerData.dpadRightOn |= pad.x > (CENTER_POS + STICK_DPAD_EMU_THRESHOLD);
  }

  controllerData.rightStickX = pad.c_x;
  controllerData.rightStickY = 255 - pad.c_y; // Ditto

  // Go!
  setControllerData (controllerData);
}
