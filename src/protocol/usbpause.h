/* The code in this file has been derived from the USBPause library 2.0.0
 * https://github.com/pololu/usb-pause-arduino
 *
 * Hence, this file retains its original license, which is as follows:
 *
 * Copyright (c) 2014 Pololu Corporation.  For more information, see
 *
 * http://www.pololu.com/
 * http://forum.pololu.com/
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <Arduino.h>

#ifdef __AVR_ATmega32U4__
class UsbPause
{
    /// The saved value of the UDIEN register.
    uint8_t savedUDIEN;

    /// The saved value of the UENUM register.
    uint8_t savedUENUM;

    /// The saved value of the UEIENX register for endpoint 0.
    uint8_t savedUEIENX0;

public:

    void pause()
    {
        // Disable the general USB interrupt.  This must be done
        // first, because the general USB interrupt might change the
        // state of the EP0 interrupt, but not the other way around.
        savedUDIEN = UDIEN;
        UDIEN = 0;

        // Select endpoint 0.
        savedUENUM = UENUM;
        UENUM = 0;

        // Disable endpoint 0 interrupts.
        savedUEIENX0 = UEIENX;
        UEIENX = 0;
    }

    void resume()
    {
        // Restore endpoint 0 interrupts.
        UENUM = 0;
        UEIENX = savedUEIENX0;

        // Restore endpoint selection.
        UENUM = savedUENUM;

        // Restore general device interrupt.
        UDIEN = savedUDIEN;
    }
};
#endif
