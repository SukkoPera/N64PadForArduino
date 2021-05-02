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

#pragma once

/* A read will be considered failed if it hasn't completed within this amount of
 * microseconds. The N64/GC protocol takes 4 us per bit. The longest command
 * reply we support is GC's poll command which returns 8 bytes. The timer is
 * started before the command is sent (to save time), so this must account for
 * that, too (3 bytes on GC at worst). Thus this must be at least
 * (8 + 3) * 8 * 4 = 352 us plus some margin. Taking into account time for the
 * CPU to jump and return for functions, it seems that 500 is a good value.
 * Note that this is only used when DISABLE_MILLIS is NOT defined, when it is a
 * hw timer is used, which is initialized in begin(), so if you change this make
 * sure to tune the value there accordingly, too.
 */
const unsigned long N64_COMMAND_TIMEOUT = 500UL;
