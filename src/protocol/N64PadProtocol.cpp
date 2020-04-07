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
 * See the following links for some references on the N64/GC controller
 * protocol:
 * - http://www.int03.co.uk/crema/hardware/gamecube/gc-control.htm
 * - https://www.mixdown.ca/n64dev/
 */

// Pad data pin will be set to input (Hi-Z) automatically

#include "N64PadProtocol.h"

// Delay 62.5ns on a 16MHz AtMega
#define NOP __asm__ __volatile__ ("nop\n\t")

#define delay025us() do { \
  NOP; NOP; NOP; NOP; \
} while (0)

#define delay05us() do { \
  delay025us(); \
  delay025us(); \
} while (0)

#define delay1us() do { \
  delay05us(); \
  delay05us(); \
} while (0)

#define delay2us() do { \
  delay1us(); \
  delay1us(); \
} while (0)

#define delay3us() do { \
  delay1us(); \
  delay2us(); \
} while (0)


inline static void sendLow () {
  // Switch pin to output mode, it will be low by default
  PAD_DIR |= (1 << PAD_BIT);
}

inline static void sendHigh () {
  // Switch pin to input mode (Hi-Z), pullups will be disabled by default
  PAD_DIR &= ~(1 << PAD_BIT);
}

// To send a 0 bit the data line is pulled low for 3us and let high for 1us
inline static void sendZero () {
  sendLow ();
  delay3us ();
  sendHigh ();
  delay1us ();
}

// To send a 1 the data line is pulled low for 1us and let high for 3us
inline static void sendOne () {
  sendLow ();
  delay1us ();
  sendHigh ();
  delay3us ();
}

// "Console stop bit" is line low for 1us, and high for 2us (3us total).
inline static void sendStop () {
  sendLow ();
  delay1us ();
  sendHigh ();
  delay2us ();
}

inline static byte readPad () {
  return PAD_INPORT & (1 << PAD_BIT);
}

// This must be implemented like this, as it cannot be too slow, or the controller won't recognize the signal
inline static void sendCmd (const byte *cmdbuf, const byte cmdsz) {
  for (byte j = 0; j < cmdsz; j++) {
    byte cmdbyte = cmdbuf[j];
	for (byte i = 0; i < 8; i++) {
	// MSB first
	if (cmdbyte & 0x80)
	  sendOne ();
	else
	  sendZero ();
	cmdbyte <<= 1;
	}
  }
  sendStop ();
}

byte *N64PadProtocol::runCommand (const byte *cmdbuf, const byte cmdsz, byte *repbuf, byte repsz) {
  register byte i;

  for (i = 0; i < repsz; i++)
    repbuf[i] = 0;

  noInterrupts ();

  sendCmd (cmdbuf, cmdsz);

  // Wait for first falling edge
  while (readPad ())
    ;

  register byte x, y, prev;
  for (i = 0; i < repsz * 8; i++) {
    x = 0;
    prev = 0;
    while (1) {
      y = readPad ();
      if (y) {
        x = (x << 1) | 0x01;
      } else if (prev != 0) {
        // Falling edge, new bit
        break;
      } else {
        // Just sample again
      }
      prev = y;
    }

    repbuf[i / 8] <<= 1;
    if (x >= 7) {
      repbuf[i / 8] |= 0x01;
    }
  }

  interrupts ();

  return repbuf;
}
