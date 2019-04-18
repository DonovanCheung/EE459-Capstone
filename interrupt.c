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

  struct GPS gps; //struct for GPS
  struct Map map; //struct for Map
  double dist; //distance to next location
  char* direction; //direction to next location
  float pointsA[7][2] = {{3401.219940, -11817.382202}, {3401.211929, -11817.365723}, {3401.224289, -11817.352905}, 
    {3401.233673, -11817.346497}, {3401.227493, -11817.333679}, {3401.221542, -11817.321320}, {3401.216965, -11817.309418}};
  init_map(&map, pointsA, 7); //initialize map
  char res[5]; //hold test values to print out

  while(1){
    _delay_ms(1000); 
    parse(GGA_Buffer, &gps); //get gps data in gps struct

    //tally of check points
    lcd_moveto(0,0);
    lcd_stringout("Checkpoint ");
    memset(res, 0, 5);
    itoa(map.index, res, 3);
    lcd_stringout(res);
    lcd_stringout(" of ");
    memset(res, 0, 5);
    itoa(map.totalPoints, res, 3);
    lcd_stringout(res);

    //Next point
    lcd_moveto(1,0);
    lcd_stringout("Next: ");
    memset(degrees_buffer,0,degrees_buffer_size);
    dist =  distanceNext(&gps, map.curr);
    dtostrf(dist, 6, 2, degrees_buffer);
    lcd_stringout(degrees_buffer);
    direction = directionNext(map.curr);
    lcd_stringout(" ");
    lcd_stringout(direction);
    updateNext(&gps, &map);

    //current point
    lcd_moveto(2, 0);
    //print current location
    memset(degrees_buffer,0,degrees_buffer_size);
    dtostrf(gps.latitude, 6, 4, degrees_buffer);
    lcd_stringout(degrees_buffer);
    lcd_stringout(" ");
    memset(degrees_buffer,0,degrees_buffer_size);
    dtostrf(gps.longitude, 6, 4, degrees_buffer);
    lcd_stringout(degrees_buffer);

   //  //print distance to next location
   //  lcd_moveto(1,0);
   //  // lcd_stringout("DistNext: ");
   //  // dtostrf(gps.latitudeDegrees, 6, 4, degrees_buffer);
   //  memset(degrees_buffer,0,degrees_buffer_size);
	  // dist =  distanceNext(gps_ptr, map_ptr);
	  // dtostrf(dist, 6, 2, degrees_buffer);
	  // lcd_stringout(degrees_buffer);

	  // direction = directionNext(gps_ptr,map_ptr);
	  // // lcd_moveto(2,0);
	  // lcd_stringout(direction);
	 
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