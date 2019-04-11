#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
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

// char GGA_Buffer[150]; //stores gpgga string
// char GGA_CODE[5]; //stores string indicating whether GPGGA or some other code
// int IsItGGAString = 0; //verifies whether string is GPGGA
// int found_start = 0; //signal $ found
// volatile int end_string = 0; //signals end of string we're looking for

// volatile uint16_t GGA_Index, code_index;

void convert_time_to_UTC();
void convert_to_degrees(char *);

#define Buffer_Size 150
#define degrees_buffer_size 20

char Latitude_Buffer[15],Longitude_Buffer[15],Time_Buffer[15],Altitude_Buffer[8];
char degrees_buffer[degrees_buffer_size];                      /* save latitude or longitude in degree */
char GGA_Buffer[Buffer_Size];                                  /* save GGA string */
uint8_t GGA_Pointers[20];                                      /* to store instances of ',' */
char GGA_CODE[3];

volatile uint16_t GGA_Index, CommaCounter;

bool  IsItGGAString = false,
    flag1     = false,
    flag2     = false;

void get_gpstime(){
  cli();
  uint8_t time_index=0;
  /* parse Time in GGA string stored in buffer */
  int index;
  for(index = 0;GGA_Buffer[index]!=','; index++){
    
    Time_Buffer[time_index] = GGA_Buffer[index];
    time_index++;
  }
  convert_time_to_UTC();
  sei();
}

void get_latitude(uint16_t lat_pointer){
  cli();
  uint8_t lat_index;
  uint8_t index = lat_pointer+1;
  lat_index=0;
  
  /* parse Latitude in GGA string stored in buffer */
  for(;GGA_Buffer[index]!=',';index++){
    Latitude_Buffer[lat_index]= GGA_Buffer[index];
    lat_index++;
  }
  
  Latitude_Buffer[lat_index++] = GGA_Buffer[index++];
  Latitude_Buffer[lat_index]= GGA_Buffer[index];    /* get direction */
  convert_to_degrees(Latitude_Buffer);
  sei();
}

void get_longitude(uint16_t long_pointer){
  cli();
  uint8_t long_index;
  uint8_t index = long_pointer+1;
  long_index=0;
  
  /* parse Longitude in GGA string stored in buffer */
  for( ; GGA_Buffer[index]!=','; index++){
    Longitude_Buffer[long_index]= GGA_Buffer[index];
    long_index++;
  }
  
  Longitude_Buffer[long_index++] = GGA_Buffer[index++];
  Longitude_Buffer[long_index]   = GGA_Buffer[index]; /* get direction */
  convert_to_degrees(Longitude_Buffer);
  sei();
}

void get_altitude(uint16_t alt_pointer){
  cli();
  uint8_t alt_index;
  uint8_t index = alt_pointer+1;
  alt_index=0;
  /* parse Altitude in GGA string stored in buffer */
  for( ; GGA_Buffer[index]!=','; index++){
    Altitude_Buffer[alt_index]= GGA_Buffer[index];
    alt_index++;
  }
  
  Altitude_Buffer[alt_index]   = GGA_Buffer[index+1];
  sei();
}
void convert_time_to_UTC()
{
  unsigned int hour, min, sec;
  uint32_t Time_value;
    
  Time_value = atol(Time_Buffer);               /* convert string to integer */
  hour = (Time_value / 10000);                  /* extract hour from integer */
  min = (Time_value % 10000) / 100;             /* extract minute from integer */
  sec = (Time_value % 10000) % 100;             /* extract second from integer*/

  sprintf(Time_Buffer, "%d:%d:%d", hour,min,sec);
  
}

void convert_to_degrees(char *raw){
  
  double value;
  float decimal_value,temp;
  
  int32_t degrees;
  
  float position;
  value = atof(raw);                             /* convert string into float for conversion */
  
  /* convert raw latitude/longitude into degree format */
  decimal_value = (value/100);
  degrees = (int)(decimal_value);
  temp = (decimal_value - (int)decimal_value)/0.6; 
  position = (float)degrees + temp;
  
  dtostrf(position, 6, 4, degrees_buffer);       /* convert float value into string */  
}

int main(void){
  GGA_Index = 0;
  memset(GGA_Buffer, 0, 150); //allocate memory for GGA buffer
  memset(GGA_CODE, 0, 5); //allocate memory for GGA code buffer
  lcd_init();                 // Initialize the LCD display
  lcd_moveto(0, 0);
  serial_init();
  

  DDRD |= (1 << PD2);

	sei(); // Enable Global Interrupt Enable Flag
  lcd_clear();

	struct GPS* gps;


  while(1){
    //check to see if found entire string and print to lcd
    // if(end_string){
    //   cli();
    //   lcd_stringout(GGA_Buffer);
    //   end_string = 0;
    //   sei();
    // }
    _delay_ms(1000);
      // LCD_String_xy(1,0,"UTC Time: ");
      // lcd_moveto(0,0);
      // lcd_stringout("UTC Time: ");
      // get_gpstime();                          Extract Time in UTC 
      // // LCD_String(Time_Buffer);
      // lcd_stringout(Time_Buffer);
      // lcd_stringout(" ");
      // LCD_String("  ");
      
      // LCD_String_xy(2,0,"Lat: ");
      lcd_moveto(0,0);
      lcd_stringout("Lat: ");
      get_latitude(GGA_Pointers[0]);         /* Extract Latitude */
      // LCD_String(degrees_buffer);            /* display latitude in degree */
      lcd_stringout(degrees_buffer);
      memset(degrees_buffer,0,degrees_buffer_size);
      
      // LCD_String_xy(3,0,"Long: ");
      // lcd_moveto(2,0);
      // lcd_stringout("Long: ");
      // get_longitude(GGA_Pointers[2]);        /* Extract Longitude */
      // // LCD_String(degrees_buffer);            /* display longitude in degree */
      // lcd_stringout(degrees_buffer);
      // memset(degrees_buffer,0,degrees_buffer_size);
      
      // // LCD_String_xy(4,0,"Alt: ");
      // lcd_moveto(3,0);
      // lcd_stringout("Alt: ");
      // get_altitude(GGA_Pointers[7]);         /* Extract Altitude in meters*/
      // // LCD_String(Altitude_Buffer);
      // lcd_stringout(Altitude_Buffer);
	}

  return 0;
}


ISR(USART_RX_vect){
  // char received_char = UDR0;
  // if(received_char =='$'){  /* check for '$' */
  //   if(IsItGGAString){ // if $ at end, means end of GPGGA string
  //     end_string = 1;
  //     IsItGGAString = 0;
  //     GGA_Buffer[GGA_Index] = '\0';
  //     code_index = 0;
  //   }else{ //if first time $ found, check if GPGGA
  //     found_start = 1;
  //     GGA_Index = 0;
  //     code_index = 0;
  //     IsItGGAString = 0;  
  //   }
  // }else if(IsItGGAString){ //if GPGGA, store in GGA buffer
  //   GGA_Buffer[GGA_Index++] = received_char;
  // }else if(found_start && code_index == 5 ){ //if found start and found 5 code characters, check if GPGGA
  //   //if GPGGA, start storing to buffer
  //   if(GGA_CODE[0] == 'G' && GGA_CODE[1] == 'P' && GGA_CODE[2] == 'G' && GGA_CODE[3] == 'G' && GGA_CODE[4] == 'A'){
  //     IsItGGAString = 1;
  //   }else{ //if not GPGGA, restart
  //     found_start = 0;
  //   } 
  // }else if(found_start){ //if start, but not found all code, continue getting character for GPGGA
  //   GGA_CODE[code_index++] = received_char;
  // }
  uint8_t oldsrg = SREG;
  cli();
  char received_char = UDR0;
  
  if(received_char =='$'){                                                    /* check for '$' */
    GGA_Index = 0;
    CommaCounter = 0;
    IsItGGAString = false;
  }
  else if(IsItGGAString == true){                                             /* if true save GGA info. into buffer */
    if(received_char == ',' ) GGA_Pointers[CommaCounter++] = GGA_Index;     /* store instances of ',' in buffer */
    GGA_Buffer[GGA_Index++] = received_char;
  }
  else if(GGA_CODE[0] == 'G' && GGA_CODE[1] == 'G' && GGA_CODE[2] == 'A'){    /* check for GGA string */
    IsItGGAString = true;
    GGA_CODE[0] = 0; GGA_CODE[1] = 0; GGA_CODE[2] = 0;
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