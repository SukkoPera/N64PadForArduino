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

class InterruptManager {
public:
	virtual void prepareInterrupt () = 0;
	virtual void enableInterrupt () = 0;
	virtual void disableInterrupt () = 0;
};


#ifndef N64_CUSTOM_INTMAN

class DefaultInterruptManager final: public InterruptManager {
public:
	inline __attribute__((always_inline))
	virtual void prepareInterrupt() override final {
		PREPARE_INTERRUPT ();
	}

	inline __attribute__((always_inline))
	virtual void enableInterrupt () override final {
		ENABLE_INTERRUPT ();
	}

	inline __attribute__((always_inline))
	virtual void disableInterrupt () override final {
		DISABLE_INTERRUPT ();
	}
};
	
#endif
