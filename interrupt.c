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

#define MAINMENU 0
#define PATHMENU 1
#define SENDLOC 2
#define NAVDISPLAY 3

void serial_init();
void button_init();
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

// Button/Menu Navigation flags
volatile char select = 0;
volatile char screen = MAINMENU;
char refresh_count = 0;
char refresh_rate = 7;

int main(void){
  GGA_Index = 0;
  memset(GGA_Buffer, 0, 150); //set GGA_Buffer to beginning
  memset(GGA_CODE, 0, 5); //set GGA_code to beginning
  lcd_init();                 // Initialize the LCD display
  lcd_moveto(0, 0);

  serial_init(); //initialize interrupt
  button_init();
  sei(); // Enable Global Interrupt Enable Flag
  lcd_clear();
  struct GPS gps; //struct for GPS
  struct Map map; //struct for Map
  double dist; //distance to next location
  char* direction; //direction to next location
  float pointsA[7][2] = {{34.020331, -118.289703}, {34.020201, -118.289425}, {34.020403, -118.289213}, 
    {34.020560, -118.289106}, {34.020458, -118.288893}, {34.020331, -118.288690}, {34.020331, -118.288494}};
  init_map(&map, pointsA, 7); //initialize map
  char res[5]; //hold test values to print out

  // Splash Screen
  lcd_moveto(0,7);
  lcd_stringout("Welcome");
  lcd_moveto(1,9);
  lcd_stringout("to");
  lcd_moveto(2,3);
  lcd_stringout("Jurassic Park");
  _delay_ms(3000);
  lcd_clear();


  while(1){
    /*
    if (screen == MAINMENU){
      if (refresh_count == refresh_rate){
        lcd_clear();
        refresh_count = 0;
      }
      else{
        refresh_count++;
      }
      lcd_moveto(0,0);
      lcd_stringout("1. Navigate");
      lcd_moveto(1,0);
      lcd_stringout("2. Help, I'm Lost!");
      lcd_moveto(2,0);
      lcd_stringout("3. Return to Start");
    }
    else if (screen == PATHMENU){
      if (refresh_count == refresh_rate){
        lcd_clear();
        refresh_count = 0;
      }
      else{
        refresh_count++;
      }
      lcd_moveto(0,0);
      lcd_stringout("1. T-Rex Trail");
      lcd_moveto(1,0);
      lcd_stringout("2. Pterodactyl Path");
      lcd_moveto(2,0);
      lcd_stringout("3. Old Town Road");
      lcd_moveto(3,0);
      lcd_stringout("4. Back");
    }
    else if (screen == SENDLOC){
      if (refresh_count == refresh_rate){
        lcd_clear();
        refresh_count = 0;
      }
      else{
        refresh_count++;
      }
      lcd_moveto(0,2);
      lcd_stringout("Sending Location");
      lcd_moveto(1,1);
      lcd_stringout("to Ranger Station");
      lcd_moveto(3,7);
      lcd_stringout("Cancel");
    }
    else if (screen == NAVDISPLAY){
      if (refresh_count == refresh_rate){
        lcd_clear();
        refresh_count = 0;
      }
      else{
        refresh_count++;
      }
      lcd_moveto(0,0);
      lcd_stringout("NAV Display");
      lcd_moveto(1,0);
      lcd_stringout("Coming Soon");
      lcd_moveto(3,0);
      lcd_stringout("Back");
    }*/

    _delay_ms(1000); 
    parse(GGA_Buffer, &gps);
    lcd_moveto(0,0);

    //tally of check points
    struct Point* currPoint = map.curr;

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
    dist =  distanceNext(&gps, currPoint);
    dtostrf(dist, 6, 2, degrees_buffer);
    lcd_stringout(degrees_buffer);
    direction = directionNext(&gps, currPoint);
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

    /*
    if (flag == 1){
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
  //}

	}

  return 0;
}


ISR(USART_RX_vect){
  uint8_t oldsrg = SREG;
  //cli();
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

ISR(PCINT1_vect){
  // Button #1 pressed
  if ((PIN1 & (1<<PC1)) == 0){
    if (screen == MAINMENU){
      screen = NAVDISPLAY;
    }
    else if (screen == PATHMENU){

    }
  }
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

void button_init(){
  DDRC &= ~(1 << PC1);     // Clear the PD1 pin
  // PC4 (PCINT12 pin) is now an input
  PORTC |= (1 << PC1);    // turn On the Pull-up
  // PC4 is now an input with pull-up enabled
  PCICR |= (1 << PCIE1);				// Enable interrupts for Port D
	PCMSK1 |= (1 << PCINT9);	// Set mask register for PD2
}
