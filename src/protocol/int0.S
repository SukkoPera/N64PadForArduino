#include <avr/io.h>
#include "pinconfig.h"

.section .text

.extern repbuf2

.global PCINT0_vect

#define REG_DATA _SFR_IO_ADDR (GPIOR0)
#define REG_CURBIT _SFR_IO_ADDR (GPIOR1)
#define REG_CURBYTE _SFR_IO_ADDR (GPIOR2)

; Uno
;~ #define SIGNAL sbi _SFR_IO_ADDR (PINB), PB5

; Leonardo
;~ #define SIGNAL sbi _SFR_IO_ADDR (PINC), PC7

; Disable
#define SIGNAL

PCINT0_vect:
	; PCINTs are called for both edges, so make sure we're on the right one
	sbic _SFR_IO_ADDR (PAD_INPORT), PAD_BIT
	reti
	
	push    r24
	in      r24, _SFR_IO_ADDR (SREG)
	push    r24
	;~ push	ZL						; Don't push, save it GPIOR later to save clocks
	push	ZH

	in      r24, REG_DATA
	lsl		r24
	SIGNAL
	sbic    _SFR_IO_ADDR (PAD_INPORT), PAD_BIT
	sbr		r24, 1					; Got a one, store it
	SIGNAL
	out     REG_DATA, r24

	; Next bit
	in      r24, REG_CURBIT
	dec		r24
	brne	done

	; Current byte is done
	out		REG_CURBIT, ZL
	ldi		ZL, lo8 (repbuf2)
    ldi		ZH, hi8 (repbuf2)
    in      r24, REG_CURBYTE
    add		ZL, r24
    inc		r24						; Byte count += 1
    out     REG_CURBYTE, r24
    clr		r24
    adc		ZH, r24					; Maybe if we are sure the address doesn't carry, we can skip this?
	in      r24, REG_DATA
	st      Z, r24
	in		ZL, REG_CURBIT

	; Prepare for next byte
	clr		r24						; Zero buffer
	out		REG_DATA, r24
	ldi		r24, 8					; Bit count = 8

done:
	out     REG_CURBIT, r24
		
	pop		ZH
	;~ pop		ZL
	pop     r24
	out     _SFR_IO_ADDR (SREG), r24
	pop     r24
	
	reti
