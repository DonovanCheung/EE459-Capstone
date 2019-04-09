#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "gps.h"
#include "lcd.h"
/*
#ifndef __WITH_AVRLIBC__
#error use --with-avrlibc to configure avr-gcc + AVR-Libc
#endif
*/
#define FOSC 7372800           // Clock frequency
#define BAUD 9600               // Baud rate used
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register

void serial_init();
void init_timer(unsigned short);
void reverse(char*, int);
int intToStr(int, char[], int);
void ftoa(float, char*, int);

volatile char buf[20];	// Stores recieved char
volatile int flag;	// Interrupt flag
volatile int pos = 0;	// LCD position

char GGA_Buffer[150]; //stores gpgga string
char GGA_CODE[5]; //stores string indicating whether GPGGA or some other code
int IsItGGAString = 0; //verifies whether string is GPGGA
int found_start = 0; //signal $ found
volatile int end_string = 0; //signals end of string we're looking for

volatile uint16_t GGA_Index, code_index;



int main(void){

  lcd_init();                 // Initialize the LCD display
  lcd_moveto(0, 0);
  lcd_clear();
  serial_init();
  memset(GGA_Buffer, 0, 150); //allocate memory for GGA buffer
  memset(GGA_CODE, 0, 5); //allocate memory for GGA code buffer

  DDRD |= (1 << PD2);

	sei(); // Enable Global Interrupt Enable Flag

	struct GPS* gps;


  while(1){
    //check to see if found entire string and print to lcd
    if(end_string){
      cli();
      lcd_stringout(GGA_Buffer);
      end_string = 0;
      sei();
    }
	}

  return 0;
}


ISR(USART_RX_vect){
  char received_char = UDR0;
  if(received_char =='$'){  /* check for '$' */
    if(IsItGGAString){ // if $ at end, means end of GPGGA string
      end_string = 1;
      IsItGGAString = 0;
      GGA_Buffer[GGA_Index] = '\0';
      code_index = 0;
    }else{ //if first time $ found, check if GPGGA
      found_start = 1;
      GGA_Index = 0;
      code_index = 0;
      IsItGGAString = 0;  
    }
  }else if(IsItGGAString){ //if GPGGA, store in GGA buffer
    GGA_Buffer[GGA_Index++] = received_char;
  }else if(found_start && code_index == 5 ){ //if found start and found 5 code characters, check if GPGGA
    //if GPGGA, start storing to buffer
    if(GGA_CODE[0] == 'G' && GGA_CODE[1] == 'P' && GGA_CODE[2] == 'G' && GGA_CODE[3] == 'G' && GGA_CODE[4] == 'A'){
      IsItGGAString = 1;
    }else{ //if not GPGGA, restart
      found_start = 0;
    } 
  }else if(found_start){ //if start, but not found all code, continue getting character for GPGGA
    GGA_CODE[code_index++] = received_char;
  }
  
}


void serial_init(){
	// initialize USART (must call this before using it)
	UBRR0=MYUBRR; // set baud rate
  UCSR0B|=(1<<RXEN0); //enable RX 
	UCSR0B|=(1<<RXCIE0); //RX complete interrupt
	UCSR0C|=(1<<UCSZ01)|(1<<UCSZ01); // no parity, 1 stop bit, 8-bit data
}