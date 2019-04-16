#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <stdio.h>
#include "gps.h"

#define totalPoints 3
#define Range 5

struct Map{

	struct GPS checkpoint[totalPoints];
	//uint16_t displacement;			//How far a person actually is from checkpoint
	struct GPS* prev;
	struct GPS* next;
	int index;
};

void init_points(struct Map* map_ptr);
void reverse(char*, int);
int intToStr(int, char[], int);
void ftoa(float, char*, int);
double radDegrees(double rad);

void updateNext(struct GPS* current, struct Map* map);
double distanceNext(struct GPS* current, struct Map* map);
double distancePrev(struct GPS* current,struct Map* map);
char* directionTo(struct GPS* current, struct GPS* to);
char* directionNext(struct GPS* current, struct Map* map);
char* nextCheck(struct GPS* current, struct Map* map);	//output ex: "30 degrees NW"


double calcDistance(struct GPS* current, struct GPS* gps);
double distFromMap(struct GPS* current, struct Map* map); 
#endif