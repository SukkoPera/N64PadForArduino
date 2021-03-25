; This file is part of N64Pad for Arduino.
;
; Copyright (C) 2015-2021 by SukkoPera
;
; N64Pad is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; N64Pad is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with N64Pad. If not, see <http://www.gnu.org/licenses/>.

#include <avr/io.h>
#include "pinconfig.h"

#ifdef N64PAD_USE_PCINT

.section .text

.extern repbuf2

.global N64PAD_INT_VECTOR

#define REG_DATA _SFR_IO_ADDR (GPIOR0)
#define REG_CURBIT _SFR_IO_ADDR (GPIOR1)
#define REG_CURBYTE _SFR_IO_ADDR (GPIOR2)

; Uno
;~ #define SIGNAL sbi _SFR_IO_ADDR (PINB), PB5

; Leonardo
;~ #define SIGNAL sbi _SFR_IO_ADDR (PINC), PC7

; Disable
#define SIGNAL

N64PAD_INT_VECTOR:
    ; PCINTs are called for both edges, so make sure we're on the right one
    sbic _SFR_IO_ADDR (PAD_INPORT), PAD_BIT
    reti

    push    r24
    in      r24, _SFR_IO_ADDR (SREG)
    push    r24
    ;~ push ZL                      ; Don't push, save it GPIOR later to save clocks
    push    ZH

    in      r24, REG_DATA
    lsl     r24
    SIGNAL
    sbic    _SFR_IO_ADDR (PAD_INPORT), PAD_BIT
    sbr     r24, 1                  ; Got a one, store it
    SIGNAL
    out     REG_DATA, r24

    ; Next bit
    in      r24, REG_CURBIT
    dec     r24
    brne    done

    ; Current byte is done
    out     REG_CURBIT, ZL
    ldi     ZL, lo8 (repbuf2)
    ldi     ZH, hi8 (repbuf2)
    in      r24, REG_CURBYTE
    add     ZL, r24
    inc     r24                     ; Byte count += 1
    out     REG_CURBYTE, r24
    clr     r24
    adc     ZH, r24                 ; Maybe if we are sure the address doesn't carry, we can skip this?
    in      r24, REG_DATA
    st      Z, r24
    in      ZL, REG_CURBIT

    ; Prepare for next byte
    clr     r24                     ; Zero buffer
    out     REG_DATA, r24
    ldi     r24, 8                  ; Bit count = 8

done:
    out     REG_CURBIT, r24

    pop     ZH
    ;~ pop      ZL
    pop     r24
    out     _SFR_IO_ADDR (SREG), r24
    pop     r24

    reti

#endif
