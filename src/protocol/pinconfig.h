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
 ******************************************************************************/

// NOTE: This file is included both from C and assembly code!

/* You can set the following to any pins you like as long as it's Pin-Change
 * Interrupt capable. This usually means it's indicated as PCINTx on the
 * datasheet
 */

#if defined (__AVR_ATtinyX5__)
	// PB2 is analog input 1
	// Note that ATtiny85 is UNTESTED!!!
	#define PAD_DIR DDRB
	#define PAD_OUTPORT PORTB
	#define PAD_INPORT PINB
	#define PAD_BIT PB2
	#define PCINT_VECTOR PCINT0_vect
#elif defined(__AVR_ATmega328P__) || defined (__AVR_ATmega328__) || defined (__AVR_ATmega168__) || defined (__AVR_ATtiny88__) || defined (__AVR_ATtiny48__)
	// Pin 2
	#define PAD_DIR DDRD
	#define PAD_OUTPORT PORTD
	#define PAD_INPORT PIND
	#define PAD_BIT PD2
	#define PCINT_VECTOR PCINT2_vect
#elif defined (__AVR_ATmega32U4__)
	// Pin 3
	//~ #define PAD_DIR DDRD
	//~ #define PAD_OUTPORT PORTD
	//~ #define PAD_INPORT PIND
	//~ #define PAD_BIT PD0
	// Pin 8
	#define PAD_DIR DDRB
	#define PAD_OUTPORT PORTB
	#define PAD_INPORT PINB
	#define PAD_BIT PB4
	#define PCINT_VECTOR PCINT0_vect
#else
  // At least for the moment...
  #error "This library is not currently supported on this platform"
#endif
