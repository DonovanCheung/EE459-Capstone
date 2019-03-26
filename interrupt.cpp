#include "avr/interrupt.h"
#include "avr/io.h"
 
//#define FOSC 16000000           // Clock frequency
#define BAUD 9600               // Baud rate used
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register


volatile char receivedByte;	// Stores recieved char

int main(void){
	
	
	
	serial_init();
	
	sei(); // Enable Global Interrupt Enable Flag
	
	
	while(1){
		
		
		
	}
}


ISR(USART_RXC_vect){
	char received = UDR0;
	receivedByte = received; // Fetch the received byte value
	
	
}

void serial_init(){
	// initialize USART (must call this before using it)
	UBRR0=MYUBRR; // set baud rate
	UCSR0B|=(1<<RXEN0); //enable RX
	UCSR0B|=(1<<RXCIE0); //RX complete interrupt
	UCSR0C|=(1<<UCSZ01)|(1<<UCSZ01); // no parity, 1 stop bit, 8-bit data
}