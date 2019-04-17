#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gps.h"
#include "lcd.h"
#include "map.h"
 

#define FOSC 7372800           // Clock frequency
#define BAUD 9600               // Baud rate used
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register

#define Buffer_Size 150
#define degrees_buffer_size 20

void serial_init();
void init_timer(unsigned short);
void reverse(char*, int);
int intToStr(int, char[], int);
void ftoa(float, char*, int);



char degrees_buffer[degrees_buffer_size];     /* save latitude or longitude in degree */
char GGA_Buffer[Buffer_Size];         		  /* save GGA string */
char GGA_CODE[3];

volatile uint16_t GGA_Index;

bool  IsItGGAString = false;

volatile bool end_string = false;


int main(void){
  GGA_Index = 0;
  memset(GGA_Buffer, 0, 150); //set GGA_Buffer to beginning
  memset(GGA_CODE, 0, 5); //set GGA_code to beginning
  lcd_init();                 // Initialize the LCD display
  lcd_moveto(0, 0);
  
  serial_init(); //initialize interrupt
  sei(); // Enable Global Interrupt Enable Flag
  lcd_clear();

  struct GPS gps;
  struct GPS* gps_ptr;
  struct Map map;
  struct Map* map_ptr;
  gps_ptr = &gps;
  map_ptr = &map;
  init_points(map_ptr);
  double dist;
  char* direction;
  
 
  while(1){
    _delay_ms(1000); 
    parse(GGA_Buffer, &gps); //get gps data in gps struct

    lcd_moveto(0,0);
    // lcd_stringout("CurrLoc: ");
    memset(degrees_buffer,0,degrees_buffer_size);
    dtostrf(gps.latitudeDegrees, 6, 2, degrees_buffer);
    lcd_stringout(degrees_buffer);
    memset(degrees_buffer,0,degrees_buffer_size);
    dtostrf(gps.longitudeDegrees, 6, 2, degrees_buffer);
    lcd_stringout(",");
    lcd_stringout(degrees_buffer);

      lcd_moveto(1,0);
      lcd_stringout("DistNext: ");
     // dtostrf(gps.latitudeDegrees, 6, 4, degrees_buffer);
    memset(degrees_buffer,0,degrees_buffer_size);
	  dist =  distanceNext(gps_ptr, map_ptr);
	  dtostrf(dist, 6, 4, degrees_buffer);
	  lcd_stringout(degrees_buffer);
	  direction = directionNext(gps_ptr,map_ptr);
	  lcd_moveto(2,0);
	  lcd_stringout(direction);
	 
    /*  lcd_stringout(degrees_buffer);
      memset(degrees_buffer,0,degrees_buffer_size);

      lcd_moveto(2,0);
      lcd_stringout("Long:");
      dtostrf(gps.longitudeDegrees, 6, 4, degrees_buffer);

      lcd_stringout(degrees_buffer);
      memset(degrees_buffer,0,degrees_buffer_size);
	*/
	}

  return 0;
}


ISR(USART_RX_vect){
  uint8_t oldsrg = SREG;
  cli();
  char received_char = UDR0;
  
  if(received_char =='$'){                                                    /* check for '$' */
    GGA_Index = 0;
    end_string = false;
    if(IsItGGAString){
      end_string = true;
    }
    IsItGGAString = false;
    GGA_Buffer[GGA_Index++] = received_char;
  }
  else if(IsItGGAString == true){                                             /* if true save GGA info. into buffer */
    GGA_Buffer[GGA_Index++] = received_char;
  }
  else if(GGA_CODE[0] == 'G' && GGA_CODE[1] == 'G' && GGA_CODE[2] == 'A'){    /* check for GGA string */
    IsItGGAString = true;
    GGA_CODE[0] = 0; GGA_CODE[1] = 0; GGA_CODE[2] = 0;
    GGA_Buffer[GGA_Index++] = received_char;
  }
  else{
    GGA_CODE[0] = GGA_CODE[1];  GGA_CODE[1] = GGA_CODE[2]; GGA_CODE[2] = received_char; 
  }
  SREG = oldsrg;
}


void serial_init(){
  // initialize USART (must call this before using it)
  uint8_t oldsrg = SREG;
  cli();
  UBRR0=MYUBRR; // set baud rate
  UCSR0B|=(1<<RXEN0); //enable RX 
  UCSR0B|=(1<<RXCIE0); //RX complete interrupt
  UCSR0C|=(1<<UCSZ01)|(1<<UCSZ01); // no parity, 1 stop bit, 8-bit data
  SREG = oldsrg;
}