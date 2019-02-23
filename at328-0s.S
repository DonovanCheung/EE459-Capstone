;*************************************************************
;       at328-0s.S - Demonstrate simple I/O functions of ATmega328
;
;       Program loops turning PC0 on and off as fast as possible.
;
; The loop consists of
;   LOOP:   SBI  PORTC,0        (2 cycles)
;           CBI  PORTC,0        (2 cycles)
;           RJMP LOOP           (2 cycles)
;
; PC0 will be low for 4 / XTAL freq
; PC0 will be high for 2 / XTAL freq
; A 9.8304MHz clock gives a loop period of about 600 nanoseconds.
;
; Revision History
; Date     Author      Description
; 06/06/13 A. Weber    Translated from C version
; 11/18/13 A. Weber    Renamed for ATmega328P
;************************************************************

#include <avr/io.h>

	.section .text
	.global	main

main:
	sbi	_SFR_IO_ADDR(DDRC), PC0  ; Set PC0 for output

loop:	sbi	_SFR_IO_ADDR(PORTC), PC0 ; PC0 = 1
	cbi	_SFR_IO_ADDR(PORTC), PC0 ; PC0 = 0
	rjmp	loop
	
.end
