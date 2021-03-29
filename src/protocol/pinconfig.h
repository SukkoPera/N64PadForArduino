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

#if defined (__AVR_ATtiny25__) || defined (__AVR_ATtiny45__) || defined (__AVR_ATtiny85__)
	// Pin 2, PB2, INT0 - Tested OK
	#define PAD_DIR DDRB
	#define PAD_OUTPORT PORTB
	#define PAD_INPORT PINB
	#define PAD_BIT PB2
	#define N64PAD_USE_INTX
	#define N64PAD_INT_VECTOR INT0_vect
	#define prepareInterrupt() {MCUCR |= (1 << ISC01); MCUCR &= ~(1 << ISC00);}
	#define enableInterrupt() {GIFR |= (1 << INTF0); GIMSK |= (1 << INT0);}
	#define disableInterrupt() {GIMSK &= ~(1 << INT0);}
#elif defined(__AVR_ATmega328P__) || defined (__AVR_ATmega328__) || defined (__AVR_ATmega168__) || defined (__AVR_ATtiny88__) || defined (__AVR_ATtiny48__)
	// Arduino Uno, Nano, Pro Mini
	
	// Pin 2, INT0 - Tested OK
	//~ #define PAD_DIR DDRD
	//~ #define PAD_OUTPORT PORTD
	//~ #define PAD_INPORT PIND
	//~ #define PAD_BIT PD2
	//~ #define N64PAD_USE_INTX
	//~ #define N64PAD_INT_VECTOR INT0_vect
	//~ #define prepareInterrupt() {EICRA |= (1 << ISC01); EICRA &= ~(1 << ISC00);}
	//~ #define enableInterrupt() {EIFR |= (1 << INTF0); EIMSK |= (1 << INT0);}
	//~ #define disableInterrupt() {EIMSK &= ~(1 << INT0);}
	
	// Pin 3, INT1 - Tested OK
	#define PAD_DIR DDRD
	#define PAD_OUTPORT PORTD
	#define PAD_INPORT PIND
	#define PAD_BIT PD3
	#define N64PAD_USE_INTX
	#define N64PAD_INT_VECTOR INT1_vect
	#define prepareInterrupt() {EICRA |= (1 << ISC11); EICRA &= ~(1 << ISC10);}
	#define enableInterrupt() {EIFR |= (1 << INTF1); EIMSK |= (1 << INT1);}
	#define disableInterrupt() {EIMSK &= ~(1 << INT1);}
#elif defined (__AVR_ATmega32U4__)
	// Arduino Leonardo, Micro
	
	// Pin 3, PD0, INT0 - Tested OK
	#define PAD_DIR DDRD
	#define PAD_OUTPORT PORTD
	#define PAD_INPORT PIND
	#define PAD_BIT PD0
	#define N64PAD_USE_INTX
	#define N64PAD_INT_VECTOR INT0_vect
	#define prepareInterrupt() {EICRA |= (1 << ISC01); EICRA &= ~(1 << ISC00);}
	#define enableInterrupt() {EIFR |= (1 << INTF0); EIMSK |= (1 << INT0);}
	#define disableInterrupt() {EIMSK &= ~(1 << INT0);}

	// Pin 8, PB4, PCINT4 - Tested OK
	//~ #define PAD_DIR DDRB
	//~ #define PAD_OUTPORT PORTB
	//~ #define PAD_INPORT PINB
	//~ #define PAD_BIT PB4
	//~ #define N64PAD_USE_PCINT
	//~ #define N64PAD_INT_VECTOR PCINT0_vect
	//~ #define prepareInterrupt() {PCMSK0 |= (1 << PCINT4);}
	//~ #define enableInterrupt() {PCIFR |= (1 << PCIF0); PCICR |= (1 << PCIE0);}
	//~ #define disableInterrupt() {PCICR &= ~(1 << PCIE0);}
	
#elif defined (__AVR_ATmega2560__)
	// Arduino Mega
	
	// Pin 3, PE5, INT5 - Tested OK
	#define PAD_DIR DDRE
	#define PAD_OUTPORT PORTE
	#define PAD_INPORT PINE
	#define PAD_BIT PE5
	#define N64PAD_USE_INTX
	#define N64PAD_INT_VECTOR INT5_vect
	#define prepareInterrupt() {EICRB |= (1 << ISC51); EICRB &= ~(1 << ISC50);}
	#define enableInterrupt() {EIFR |= (1 << INTF5); EIMSK |= (1 << INT5);}
	#define disableInterrupt() {EIMSK &= ~(1 << INT5);}
#else
	// At least for the moment...
	#error "This library is not currently supported on this platform"
#endif
