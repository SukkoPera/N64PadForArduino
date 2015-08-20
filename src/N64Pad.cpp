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

#include "N64Pad.h"

N64Pad::N64Pad () {
  buttons = 0;
  x = 0;
  y = 0;
  last_poll = 0;
}

bool N64Pad::begin () {
  // I'm not sure non-Nintendo controllers return 5
  if (proto.runCommand (N64PadProtocol::CMD_RESET, buf, 3))
    return buf[0] == 5;
  else
    return false;
}

void N64Pad::read () {
  if (millis () - last_poll >= 10) {
    proto.runCommand (N64PadProtocol::CMD_POLL, buf, 4);
    buttons = ((((uint16_t) buf[0]) << 8) | buf[1]);
    x = (int8_t) buf[2];
    y = (int8_t) buf[3];

    last_poll = millis ();
  }
}
