#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <stdio.h>
#include "gps.h"

#define points 3

struct Map{
	
	struct GPS checkpoint[points]; 
	uint16_t range;					//Distance person can be from checkpoint
	uint16_t displacement;			//How far a person actually is from checkpoint
	struct GPS* prev;
	struct GPS* next;
};

void reverse(char*, int);
int intToStr(int, char[], int);	 
void ftoa(float, char*, int);
double radDegrees(double rad);

double distanceNext(struct GPS* current, struct Map* map);
double distancePrev(struct GPS* current,struct Map* map); 
char* directionTo(struct GPS* current, struct GPS* to);
char* nextCheck(struct GPS* current, struct Map* map);	//output ex: "30 degrees NW"

#endif