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
 * This sketch is the multiplexer that should be run on a second Arduino to get
 * the MegaDriveInterface sketch working. Please see that sketch for
 * instructions.
 *
 * Note that this sketch MUST BE RUN ON AN ARDUINO wired exactly as described.
 * The timing was very tricky to figure out. I have tried to use an ATtiny861
 * instead but couldn't get it working, for the sake of it!
 */

#define INPORT1 PINB
#define DIR1 DDRB
#define MASK1 0x0F

#define INPORT2 PIND
#define DIR2 DDRD
#define MASK2 0x0F

#define OUTPORT PORTC
#define OUTDIR DDRC
#define OUTMASK 0x0F

#define SELECT_PIN_PINPORT PIND
#define SELECT_PIN_DIR DDRD
#define SELECT_PIN_BIT PD7

void setup () {
  DIR1 &= ~MASK1;
  DIR2 &= ~MASK2;

  OUTDIR |= OUTMASK;

  SELECT_PIN_DIR &= ~(1 << SELECT_PIN_BIT);

  noInterrupts ();
}

#define NOP __asm__ __volatile__ ("nop\n\t")

void loop () {
  while (1) {
    if (SELECT_PIN_PINPORT & (1 << SELECT_PIN_BIT)) {
      OUTPORT = INPORT2;
    } else {
      OUTPORT = INPORT1;
      NOP;      // Hold for a bit
    }
  }
}
