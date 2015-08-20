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
 ******************************************************************************/

#include <Arduino.h>

#if !defined(ARDUINO_ARCH_AVR)
  // At least for the moment...
  #error “This library only supports boards with an AVR processor.”
#endif

#define PAD_DIR DDRC
#define PAD_OUTPORT PORTC
#define PAD_INPORT PINC
#define PAD_BIT 0

class N64PadProtocol {
public:
  enum Command {
    // Buffer size required: 3 bytes
    CMD_IDENTIFY = 0x00,
    // 4
    CMD_POLL = 0x01,
    // ?
    CMD_READ = 0x02,
    // ?
    CMD_WRITE = 0x03,
    // 3
    CMD_RESET = 0xFF
  };

  // WARNING: This disables interrupts and runs for 160+ us!
  byte *runCommand (Command cmd, byte *repbuf, byte repsz);
};
