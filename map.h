#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <stdio.h>
#include "gps.h"

#define RANGE 20

struct Point{
	struct GPS * gps;
	struct Point *prev;
	struct Point *next;
};

struct Map{
	int totalPoints;
	struct Point* head;
	struct Point* curr;
	int index;
};

void init_map(struct Map* map_ptr, float points[][2], int numPoints);
double radDegrees(double rad);

void updateNext(struct GPS* current, struct Map* map);
double distanceNext(struct GPS* current, struct Point* pt);
char* directionTo(struct GPS* current, struct GPS* to);
char* directionNext(struct GPS* current, struct Point* pt);

double calcDistance(struct GPS* current, struct GPS* gps);

#endif