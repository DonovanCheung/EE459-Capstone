#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
 
#define FOSC 16000000           // Clock frequency
#define BAUD 9600               // Baud rate used
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register

void serial_init();

volatile char buf[74];	// Stores recieved char
volatile int flag;	// Interrupt flag
volatile int pos =0;	// LCD position

int main(void){
	
	
	
	serial_init();
	
	sei(); // Enable Global Interrupt Enable Flag
	
	flag = 0;
	
	int num=0;
	GPS* gps;
	
	while(1){
		
		
		if(flag == 1){
			num = parse(buf , gps);
			if(num){
				lcd_moveto(1,0);
				lcd_stringout(buf); 		
		
			}	
			_delay_ms(250);
			flag = 0;
			pos = 0;
		}
	}
}

ISR(USART_RXC_vect)
    {
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