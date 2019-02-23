/*************************************************************
*       at328-0.c - Demonstrate simple I/O functions of ATmega328
*
*       Program loops turning PC0 on and off as fast as possible.
*
* The program should generate code in the loop consisting of
*   LOOP:   SBI  PORTC,0        (2 cycles)
*           CBI  PORTC,0        (2 cycles)
*           RJMP LOOP           (2 cycles)
*
* PC0 will be low for 4 / XTAL freq
* PC0 will be high for 2 / XTAL freq
* A 9.8304MHz clock gives a loop period of about 600 nanoseconds.
*
* Revision History
* Date     Author      Description
* 09/14/12 A. Weber    Initial Release
* 11/18/13 A. Weber    Renamed for ATmega328P
*************************************************************/

#include <avr/io.h>
#include <util/delay.h>
int main(void)
{
    // Original
    // DDRC |= 1 << DDC0 || 1 << DDC1 || 1 << DDC2 || 1 << DDC3 || 1 << DDC4 || 1 << DDC5;          // Set PORTC bit 0 for output

    DDRB |= 0xFF;          // Set DDRB bits 0, 1, 2, 3, 4, 5, 6, and 7 to 0 for output (11111111)
    DDRC |= 0x3F;          // Set DDRC bits 0, 1, 2, 3, 4, and 5 to 0 for output (00111111)
    DDRD |= 0xFF;          // Set DDRD bits 0, 1, 2, 3, 4, 5, 6, and 7 to 0 for output (11111111)
    PORTB &= ~(0xFF);      // Clear PORTB bits 0, 1, 2, 3, 4, 5, 6, and 7
    PORTC &= ~(0x3F);      // Clear PORTC bits 0, 1, 2, 3, 4, and 5
    PORTD &= ~(0xFF);      // Clear PORTD bits 0, 1, 2, 3, 4, 5, 6, and 7

    while (1) {
      // 0-1-0 transition for PB's
      PORTB |= 1 << PB0;      // Set PB0 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTB &= ~(1 << PB0);   // Set PB0 to a 0
			_delay_ms(10);

			PORTB |= 1 << PB1;      // Set PB1 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTB &= ~(1 << PB1);   // Set PB1 to a 0
			_delay_ms(10);

			PORTB |= 1 << PB2;      // Set PB2 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTB &= ~(1 << PB2);   // Set PB2 to a 0
			_delay_ms(10);

			PORTB |= 1 << PB3;      // Set PB3 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTB &= ~(1 << PB3);   // Set PB3 to a 0
			_delay_ms(10);

			PORTB |= 1 << PB4;      // Set PB4 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTB &= ~(1 << PB4);   // Set PB4 to a 0
			_delay_ms(10);

      PORTB |= 1 << PB5;      // Set PB5 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTB &= ~(1 << PB5);   // Set PB5 to a 0
			_delay_ms(10);

      PORTB |= 1 << PB6;      // Set PB6 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTB &= ~(1 << PB6);   // Set PB6 to a 0
			_delay_ms(10);

      PORTB |= 1 << PB7;      // Set PB7 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTB &= ~(1 << PB7);   // Set PB7 to a 0
			_delay_ms(10);

      // 0-1-0 transition for PC's
      PORTC |= 1 << PC0;      // Set PC0 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTC &= ~(1 << PC0);   // Set PC0 to a 0
			_delay_ms(10);

			PORTC |= 1 << PC1;      // Set PC1 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTC &= ~(1 << PC1);   // Set PC1 to a 0
			_delay_ms(10);

			PORTC |= 1 << PC2;      // Set PC2 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTC &= ~(1 << PC2);   // Set PC2 to a 0
			_delay_ms(10);

			PORTC |= 1 << PC3;      // Set PC3 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTC &= ~(1 << PC3);   // Set PC3 to a 0
			_delay_ms(10);

			PORTC |= 1 << PC4;      // Set PC4 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTC &= ~(1 << PC4);   // Set PC4 to a 0
			_delay_ms(10);

      PORTC |= 1 << PC5;      // Set PC5 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTC &= ~(1 << PC5);   // Set PC5 to a 0
			_delay_ms(10);

      // 0-1-0 transition for PD's
      PORTD |= 1 << PD0;      // Set PD0 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTD &= ~(1 << PD0);   // Set PD0 to a 0
			_delay_ms(10);

			PORTD |= 1 << PD1;      // Set PD1 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTD &= ~(1 << PD1);   // Set PD1 to a 0
			_delay_ms(10);

			PORTD |= 1 << PD2;      // Set PD2 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTD &= ~(1 << PD2);   // Set PD2 to a 0
			_delay_ms(10);

			PORTD |= 1 << PD3;      // Set PD3 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTD &= ~(1 << PD3);   // Set PD3 to a 0
			_delay_ms(10);

			PORTD |= 1 << PD4;      // Set PD4 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTD &= ~(1 << PD4);   // Set PD4 to a 0
			_delay_ms(10);

      PORTD |= 1 << PD5;      // Set PD5 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTD &= ~(1 << PD5);   // Set PD5 to a 0
			_delay_ms(10);

      PORTD |= 1 << PD6;      // Set PD6 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTD &= ~(1 << PD6);   // Set PD6 to a 0
			_delay_ms(10);

      PORTD |= 1 << PD7;      // Set PD7 to a 1
			_delay_ms(10);         // wait 10 ms
			PORTD &= ~(1 << PD7);   // Set PD7 to a 0
			_delay_ms(10);
    }
}
