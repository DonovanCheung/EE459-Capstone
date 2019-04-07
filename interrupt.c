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
#define FOSC 16000000           // Clock frequency
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

int main(void){

  lcd_init();                 // Initialize the LCD display
  lcd_moveto(0, 0);
  lcd_clear();

  serial_init();


	sei(); // Enable Global Interrupt Enable Flag
	flag = 0;
	int num=0;
	struct GPS* gps;
  char sflag[5];
	char long_buf[10];
  int n = 5;

  while(1){
    //lcd_stringout(itoa(flag, sflag, 10)); // prints flag value as a test
    if(flag){
			num = parse(buf , gps);
			if(num){
        ftoa(gps->latitude, long_buf, n); // should convert long_buf into a string of gps->latitude (float), but is untested
        lcd_stringout(long_buf); // should print the latitude, but only prints ".0000" so data isn't being parsed correctly?
				//lcd_stringout(itoa(gps->lat, sflag, n)); // prints 0 if gps.c line 58 is gps->lat = any integer
        //lcd_stringout(gps->lat); // prints junk if gps.c line 58 isn't gps->lat = any integer
			}
			_delay_ms(250);
			flag = 0;
			pos = 0;
		}
	}

  return 0;
}


ISR(USART_RX_vect){
  char ch;
  ch = UDR0;                  // Get the received charater

	buf[pos] = ch;				// Store in buffer
	pos++;
	if(pos==19){
		buf[19] = '\0';
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

void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}


void ftoa(float n, char *res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}
