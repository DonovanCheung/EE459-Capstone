#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <stdio.h>
#include "gps.h"

// #define totalPoints 3
#define Range 20

struct Point{
	float latitude;
	float longitude;
	struct Point *prev;
	struct Point *next;
};

struct Map{

	int totalPoints;
	struct Point *head;
	struct Point *curr;
	int index;
};

void init_map(struct Map* map_ptr, float points[][2], int numPoints);
double radDegrees(double rad);

void updateNext(struct GPS* current, struct Map* map);
double distanceNext(struct GPS * gps, struct Point* current);
char* directionTo(struct Point* current, struct Point* to);
char* directionNext(struct Point* current);


double calcDistance(struct GPS* current, struct Point* to);
#endif