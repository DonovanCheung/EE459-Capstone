#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "map.h"

#define EXIT_FAILURE 1
#define KM_TO_FEET 3280.84

double distanceNext(struct GPS* current, struct Point* pt){
	return calcDistance(current, pt->next->gps);
}

char* directionTo(struct GPS* current, struct GPS* to){
	char* dir;
    if((current->longitude - to->longitude) > 0 && (current->latitude - to->latitude) > 0){
        dir = "NE";
    }else if((current->longitude - to->longitude) > 0 && (current->latitude - to->latitude) < 0){
        dir = "NW";
    }else if((current->longitude - to->longitude) < 0 && (current->latitude - to->latitude) > 0){
        dir = "SE";
    }else if((current->longitude - to->longitude) < 0 && (current->latitude - to->latitude) < 0){
        dir = "SW";
    }else if((current->longitude - to->longitude) > 0){
        dir = "N";
    }else if((current->longitude - to->longitude) < 0){
        dir = "S";
    }else if((current->latitude - to->latitude) > 0){
        dir = "E";
    }else{
        dir = "W";
    }

	return dir;
}

char* directionNext(struct GPS* current, struct Point* pt){
	return directionTo(current, pt->next->gps);
}

double radDegrees(double rad){return (180/3.141593)*rad;}
double degRadians(double deg){return deg*(3.141593/180);}

double calcDistance(struct GPS* current, struct GPS* gps){
  int R = 6371; // Radius of the Earth
  double dLat = degRadians(gps->latitudeDegrees - current->latitudeDegrees);
  double dLon = degRadians(gps->longitudeDegrees - current->longitudeDegrees);
  double a = sin(dLat/2)*sin(dLat/2) + cos(degRadians(gps->latitudeDegrees)) *
    cos(degRadians(current->latitudeDegrees)) * sin(dLon/2) *
    sin(dLon/2);
  double c = 2 * atan2(sqrt(a), sqrt(1-a));
  double d = R * c * KM_TO_FEET; //distance in miles
  return d;
}

void updateNext(struct GPS* current, struct Map* map){
	if(!(map->curr->next)) return; 
	double dist = calcDistance(current,map->curr->next->gps);
	if(dist < Range){
		map->index +=1;
		// map->prev = map->next;
		// map->next = &map->checkpoint[map->index];
        map->curr = map->curr->next;
		return;
	}
	else return;
	
}

void init_map(struct Map* map_ptr, float points[][2], int numPoints){
    map_ptr->totalPoints = numPoints;
    map_ptr->index = 1;
    map_ptr->head = NULL;
    map_ptr->curr = NULL;

    struct Point* head = (struct Point*)malloc(sizeof(struct Point)); 
    struct GPS * head_gps = (struct GPS*)malloc(sizeof(struct GPS));

    head_gps->latitudeDegrees  = points[0][0];
    head_gps->longitudeDegrees = points[0][1]; 
    head->gps = head_gps;
    head->prev = NULL;
    head->next = NULL;

    map_ptr->head = head;
    map_ptr->curr = head;
    struct Point* prev = head;
    int j;
    for(j = 1; j < numPoints; j++){
        struct Point* new_point = (struct Point*)malloc(sizeof(struct Point)); 
        struct GPS * new_gps = (struct GPS*)malloc(sizeof(struct GPS));
        // printf("%f, %f\n", points[j][0], points[j][1]);
        new_gps->latitudeDegrees = points[j][0];
        new_gps->longitudeDegrees = points[j][1];
        new_point->gps = new_gps;
        // printf("%f, %f\n", new_point->gps.latitudeDegrees, new_point->gps.longitudeDegrees);
        new_point->prev = prev;
        new_point->next = NULL;
        prev->next = new_point;
        prev = new_point;
    }
}
    // int points = 6;
    // float latitude_points[] = {34.020331, 34.020201, 34.020403, 34.020560,
    //     34.020458, 34.020282};
    // float longitude_points[] = {-118.289703, -118.289425, -118.289213, 
    //     -118.289106, -118.288494};

//     int j;
//     for(j = 0; j < points; j++)
//     {
//         map_ptr->checkpoint[j].latitudeDegrees = latitude_points[j];
//         map_ptr->checkpoint[j].longitudeDegrees = longitude_points[j];
//     }
// 	map_ptr->prev = NULL;
// 	map_ptr->next = &map_ptr->checkpoint[0];
// 	map_ptr->index = 1;
// }