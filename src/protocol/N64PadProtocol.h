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

#if defined ( __AVR_ATtinyX5__ )
	// P2 is analog input 1
	#define PAD_DIR DDRB
	#define PAD_OUTPORT PORTB
	#define PAD_INPORT PINB
	#define PAD_BIT PB2
#elif defined( __AVR_ATmega328P__ ) || defined( __AVR_ATmega328__ ) || defined( __AVR_ATmega168__ ) || defined (__AVR_ATtiny88__) || defined (__AVR_ATtiny48__)
	#define PAD_DIR DDRD
	#define PAD_OUTPORT PORTD
	#define PAD_INPORT PIND
	#define PAD_BIT PD2
#else
  // At least for the moment...
  #error “This library is not currently supported on this platform”
#endif

class N64PadProtocol {
public:
  /* NOTE: This disables interrupts and runs for 32+ us per byte to
   * exchange!
   */
  byte *runCommand (const byte *cmdbuf, const byte cmdsz, byte *repbuf, byte repsz);
};
