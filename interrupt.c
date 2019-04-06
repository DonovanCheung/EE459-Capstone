#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "gps.h"
#include "lcd.h"

#define FOSC 16000000           // Clock frequency
#define BAUD 9600               // Baud rate used
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register

void serial_init();
void init_timer(unsigned short);

volatile char buf[74];	// Stores recieved char
volatile int flag;	// Interrupt flag
volatile int pos = 0;	// LCD position

int main(void){

  lcd_init();                 // Initialize the LCD display
  lcd_moveto(0, 0);
  cli();

  // DDRD &= ~(1 << PD0); // Clear Pin D0 (now an input)
  // PORTD |= (1 << PD0); // Turn on internal pull-up for Pin D0
  // PCICR |= (1 << PCIE2);
  // PCMSK2 |= (1 << PCINT16);

  serial_init();
  // init_timer(15625);


	sei(); // Enable Global Interrupt Enable Flag
	flag = 0;
	int num=0;
	struct GPS* gps;
  char sflag[5];

  while(1){
    lcd_stringout(itoa(flag, sflag, 10));
    if(flag == 1){
			num = parse(buf , gps);
			if(num){
				lcd_moveto(0,0);
				lcd_stringout(buf);
			}
			_delay_ms(250);
			flag = 0;
			pos = 0;
		}
	}

  return 0;
}

ISR(USART_RX_vect){
  lcd_stringout("<");
  char ch;
  ch = UDR0;                  // Get the received charater

	buf[pos] = ch;				// Store in buffer
	pos++;
	if(pos ==74){
		buf[74] = '\0';
		flag = 1;				// If message complete, set flag
	}
}

void serial_init(){
	// initialize USART (must call this before using it)
	UBRR0=MYUBRR; // set baud rate
  UCSR0B|=(1<<RXEN0); //enable RX
	UCSR0B|=(1<<RXCIE0); //RX complete interrupt
	UCSR0C|=(1<<UCSZ01)|(1<<UCSZ01); // no parity, 1 stop bit, 8-bit data
}

void init_timer(unsigned short m){
  TCCR1B |= (1 << WGM12);
  TIMSK1 |= (1 << OCIE1A);
  OCR1A = m;
  TCCR1B |= (1 << CS12);
}

ISR(TIMER1_COMPA_vect){          // timer compare interrupt service routine

}
