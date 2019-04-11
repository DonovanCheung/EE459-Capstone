#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <math.h>
#include "gps.h"
#include "lcd.h"
#include "map.h"


#define FOSC 7372800            // Clock frequency: 7.3728MHz
#define BAUD 9600               // Baud rate used
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register
#define MAXCHAR 1000

void serial_init();
void init_timer(unsigned short);

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
  FILE *fp;
   char str[MAXCHAR];
   char* filename = "c:\\temp\\test.txt";

   fp = fopen(filename, "r");
   if (fp == NULL){
       printf("Could not open file %s",filename);
       //return 1;
   }
   else{
     while (fgets(str, MAXCHAR, fp) != NULL){
       printf("%s", str);
     }
   }
   fclose(fp);

  lcd_init();                 // Initialize the LCD display
  lcd_moveto(0, 0);
  lcd_clear();
  serial_init();
  memset(GGA_Buffer, 0, 150); //allocate memory for GGA buffer
  memset(GGA_CODE, 0, 5); //allocate memory for GGA code buffer

  DDRD |= (1 << PD2);

	sei(); // Enable Global Interrupt Enable Flag

	int num = 0;
	float lat;		//Latitude
	struct GPS gps;
	struct GPS* gps_ptr = &gps;

	char buf[6];



  while(1){
    //check to see if found entire string and print to lcd
    if(end_string){

      cli();
    /*	num = parse(GGA_Buffer, gps_ptr);	//Uncomment when ready to test parse
		if(num){
			lat = gps_ptr->latitude;
			ftoa(lat,buf,6);
			lcd_moveto(0,0);
			lcd_stringout("Lat: ");
			lcd_stringout(buf);
			_delay_ms(50);

		} */
	  lcd_stringout(GGA_Buffer);	//Comment out when
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
    }
	else{ //if first time $ found, check if GPGGA
      found_start = 1;
      GGA_Index = 0;
      code_index = 0;
      IsItGGAString = 0;
    }
  }
  else if(IsItGGAString){ //if GPGGA, store in GGA buffer
    GGA_Buffer[GGA_Index++] = received_char;
  }
  else if(found_start && code_index == 5 ){ //if found start and found 5 code characters, check if GPGGA
    //if GPGGA, start storing to buffer
    if(GGA_CODE[0] == 'G' && GGA_CODE[1] == 'P' && GGA_CODE[2] == 'G' && GGA_CODE[3] == 'G' && GGA_CODE[4] == 'A'){
      IsItGGAString = 1;
    }else{ //if not GPGGA, restart
      found_start = 0;
    }
  }
  else if(found_start){ //if start, but not found all code, continue getting character for GPGGA
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
