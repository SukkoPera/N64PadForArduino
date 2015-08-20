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
 * First of all be aware that for this to work you will need to use either:
 * - A 74HC157 Quad Multiplexer (The same chip that is used in original
 *   MegaDrive controllers!)
 * - A second Arduino running the MegaDriveMultiplexer example.
 *
 * This is due to the speed required to give the Megadrive correct readings.
 * Please refer to the following documents to understand the protocol:
 * - https://code.google.com/p/micro-64-controller/wiki/Protocol
 * - http://afermiano.com/index.php/n64-controller-protocol
 * - http://eecs.umich.edu/courses/eecs373/Lec/StudentW14/N64%20Controller.pdf
 *
 * Experience shows that when the SELECT signal changes state, the outputs must
 * be switched *very* quickly (i.e.: a couple us). I had to get rid of all the
 * bitwise logic and even of interrupts to get a 16 MHz Arduino-based
 * multiplexer deliver correct readings. Of course this doesn't play very well
 * with the fact that N64 controller readings take ~200 us, so the chances of
 * doing everything with a single Arduino are very low.
 *
 * The sketch works by polling the N64 Controller and delivering outputs as
 * follows:
 * - UP and DOWN are connected straight to the MegaDrive
 * - LEFT, RIGHT, B and C are delivered as a set to the external multiplexer.
 * - A and START are delivered as another set to the external multiplexer.
 *
 * Button mappings:
 * - A and L are mapped to A
 * - B and R are mapped to B
 * - Z and the four C buttons are mapped to C
 * - Start is mapped to... well, Start
 * - The analog stick is mapped to the directional buttons
 *
 * Connections if using the 74HC157:
 * - Arduino Pin 2 -> 74HC157 Pin 2
 * - Arduino Pin 3 -> 74HC157 Pin 5
 * - Arduino Pin 4 -> 74HC157 Pin 11
 * - Arduino Pin 5 -> 74HC157 Pin 14
 * - Arduino Pin 8 -> MegaDrive Pad Port Pin 1
 * - Arduino Pin 9 -> MegaDrive Pad Port Pin 2
 * - Arduino Pin 10 -> 74HC157 Pin 3
 * - Arduino Pin 11 -> 74HC157 Pin 6
 * - Arduino Pin 12 -> 74HC157 Pin 10
 * - Arduino Pin 13 -> 74HC157 Pin 13
 * - Arduino +5V -> 74HC157 Pin 16
 * - Arduino GND -> 74HC157 Pin 8
 * - 74HC157 Pin 15 -> 74HC157 Pin 8 (or Arduino GND)
 * - MegaDrive Pad Port Pin 7 -> 74HC157 Pin 1
 * - 74HC157 Pin 4 -> MegaDrive Pad Port Pin 3
 * - 74HC157 Pin 7 -> MegaDrive Pad Port Pin 4
 * - 74HC157 Pin 9 -> MegaDrive Pad Port Pin 6
 * - 74HC157 Pin 12 -> MegaDrive Pad Port Pin 9
 * - MegaDrive Pad Port Pin 8 -> Arduino GND (or 74HC157 Pin 8)
 *
 * Connections if using a second Arduino with MegaDriveMultiplexer:
 * - Arduino Pin 2 -> MegaDriveMultiplexer Pin 8
 * - Arduino Pin 3 -> MegaDriveMultiplexer Pin 9
 * - Arduino Pin 4 -> MegaDriveMultiplexer Pin 10
 * - Arduino Pin 5 -> MegaDriveMultiplexer Pin 11
 * - Arduino Pin 8 -> MegaDrive Pad Port Pin 1
 * - Arduino Pin 9 -> MegaDrive Pad Port Pin 2
 * - Arduino Pin 10 -> MegaDriveMultiplexer Pin 0
 * - Arduino Pin 11 -> MegaDriveMultiplexer Pin 1
 * - Arduino Pin 12 -> MegaDriveMultiplexer Pin 2
 * - Arduino Pin 13 -> MegaDriveMultiplexer Pin 3
 * - Arduino +5V -> MegaDriveMultiplexer Vin (If not otherwise powered)
 * - Arduino GND -> MegaDriveMultiplexer GND
 * - MegaDrive Pad Port Pin 7 -> MegaDriveMultiplexer Pin 7
 * - MegaDriveMultiplexer Pin A0 -> MegaDrive Pad Port Pin 3
 * - MegaDriveMultiplexer Pin A1 -> MegaDrive Pad Port Pin 4
 * - MegaDriveMultiplexer Pin A2 -> MegaDrive Pad Port Pin 6
 * - MegaDriveMultiplexer Pin A3 -> MegaDrive Pad Port Pin 9
 * - MegaDrive Pad Port Pin 8 -> Arduino GND (or MegaDriveMultiplexer GND)
 *
 * In both cases:
 * - (Optional) Pin A5 goes to a LED + series resistor that will blink when
 *   buttons are pressed. Useful to show that something is working.
 * - If you want to power the Arduinos from the MegaDrive, you can take +5V from
 *   Pad Port Pin 5. I can't guarantee the MegaDrive can stand the extra load,
 *   but its voltage regulators are mounted on a big heatsink, so I guess it
 *   will :).
 *
 * Note that in this sketch we use direct port manipulation to change all the
 * bits at once. This is not strictly necessary, so it may change in the future.
 */

#include <N64Pad.h>

/* These are the offsets that the analog stick must move before we trigger the
 * corresponding directional button
 *
 * 20 means about a quarter, feels fine to me
 */
#define MIN_X_OFFSET 20
#define MIN_Y_OFFSET MIN_X_OFFSET

// We use pin 13 for other stuff
#define LED_PIN A5

N64Pad pad;

void setup () {
  pinMode (LED_PIN, OUTPUT);

  if (!pad.begin ()) {
    // Report error, somehow
    while (1) {
        digitalWrite (LED_PIN, HIGH);
        delay (300);
        digitalWrite (LED_PIN, LOW);
        delay (700);
    }
  }

  // Setup pins for MD output
  DDRD |= ((1 << DDD2) | (1 << DDD3) | (1 << DDD4) | (1 << DDD5));
  DDRB |= ((1 << DDB2) | (1 << DDB3) | (1 << DDB4) | (1 << DDB5));
  DDRB |= ((1 << DDB1) | (1 << DDB0));
}


// Update as fast as we can
void loop () {
  pad.read ();

  /* To understand this, keep in mind that the MegaDrive uses the LOW state to
   * indicate that a button is pressed, and review De Morgan's laws
   */
  PORTB = (PORTB & 0xFC)
        | (pad.y < MIN_Y_OFFSET && ((pad.buttons & N64Pad::BTN_UP) == 0)) << PB0
        | (pad.y > -MIN_Y_OFFSET && ((pad.buttons & N64Pad::BTN_DOWN) == 0)) << PB1
        ;

  // This is the set of outputs to send the MegaDrive when SELECT is LOW
  PORTD = (PORTD & 0xC3)
        | ((pad.buttons & (N64Pad::BTN_A | N64Pad::BTN_L)) == 0) << PD4
        | ((pad.buttons & N64Pad::BTN_START) == 0) << PD5
        ;

  // This is the set of outputs to send the MegaDrive when SELECT is HIGH
  PORTB = (PORTB & 0xC3)
        | (pad.x > -MIN_X_OFFSET && ((pad.buttons & N64Pad::BTN_LEFT) == 0)) << PB2
        | (pad.x < MIN_X_OFFSET && ((pad.buttons & N64Pad::BTN_RIGHT) == 0)) << PB3
        | ((pad.buttons & (N64Pad::BTN_B | N64Pad::BTN_R)) == 0) << PB4
        | ((pad.buttons & (N64Pad::BTN_C_UP | N64Pad::BTN_C_DOWN | N64Pad::BTN_C_LEFT | N64Pad::BTN_C_RIGHT | N64Pad::BTN_Z)) == 0) << PB5
        ;

  // Blink led with buttons
  digitalWrite (LED_PIN, pad.buttons != 0);
}
