#include <avr/io.h>
#include "pinconfig.h"

.section .text

.extern repbuf2

.global INT0_vect

#define REG_DATA _SFR_IO_ADDR (GPIOR0)
#define REG_CURBIT _SFR_IO_ADDR (GPIOR1)
#define REG_CURBYTE _SFR_IO_ADDR (GPIOR2)

; Uno
;~ #define SIGNAL sbi _SFR_IO_ADDR (PINB), PB5

; Leonardo
;~ #define SIGNAL sbi _SFR_IO_ADDR (PINC), PC7

; Disable
#define SIGNAL

INT0_vect:
	push    r24
	in      r24, _SFR_IO_ADDR (SREG)
	push    r24
	push	ZL
	push	ZH

	; OK, so... We got here so fast that the line is not ready yet, so we'd
	; better sit down for a while, LOL :). The number of NOPs might need to be
	; tailored, but 2 to 4 seems the sweet spot for the Uno (I didn't try more
	; though). 2 also seems to be good on the Leonardo, so let's go with that by
	; default
	nop
	nop
	;~ nop
	;~ nop

	; Got a one, store it
	in      r24, REG_DATA
	lsl		r24
	SIGNAL
	sbic    _SFR_IO_ADDR (PAD_INPORT), PAD_BIT
	sbr		r24, 1
	SIGNAL
	out     REG_DATA, r24

	; Next bit
	in      r24, REG_CURBIT
	dec		r24
	brne	done

	; Current byte is done
	ldi		ZL, lo8 (repbuf2)
    ldi		ZH, hi8 (repbuf2)
    in      r24, REG_CURBYTE
    add		ZL, r24
    clr		r24
    adc		ZH, r24
	in      r24, REG_DATA
	st      Z, r24

	; Prepare for next byte
	in      r24, REG_CURBYTE		; Byte count += 1
	inc		r24
	out     REG_CURBYTE, r24
	clr		r24						; Zero buffer
	out		REG_DATA, r24
	ldi		r24, 8					; Bit count = 8

done:
	out     REG_CURBIT, r24
		
	pop		ZH
	pop		ZL
	pop     r24
	out     _SFR_IO_ADDR (SREG), r24
	pop     r24
	
	reti
