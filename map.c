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
double radDegrees(double rad){return (180/3.141593)*rad;}
double degRadians(double deg){return deg*(3.141593/180);}
double calcDistance_long(struct GPS* current, struct GPS* gps){
  int R = 6371; // Radius of the Earth
  double dLat = 0;
  double dLon = degRadians(gps->longitudeDegrees - current->longitudeDegrees);
  double a = sin(dLat/2)*sin(dLat/2) + cos(degRadians(gps->latitudeDegrees)) *
    cos(degRadians(current->latitudeDegrees)) * sin(dLon/2) *
    sin(dLon/2);
  double c = 2 * atan2(sqrt(a), sqrt(1-a));
  double d = R * c * KM_TO_FEET; //distance in miles
  return d;
}

double calcDistance_lat(struct GPS* current, struct GPS* gps){
  int R = 6371; // Radius of the Earth
  double dLat = degRadians(gps->latitudeDegrees - current->latitudeDegrees);
  double dLon = 0;
  double a = sin(dLat/2)*sin(dLat/2) + cos(degRadians(gps->latitudeDegrees)) *
    cos(degRadians(current->latitudeDegrees)) * sin(dLon/2) *
    sin(dLon/2);
  double c = 2 * atan2(sqrt(a), sqrt(1-a));
  double d = R * c * KM_TO_FEET; //distance in miles
  return d;
}

char* directionTo(struct GPS* current, struct GPS* to){
    char* dir;
    double long_dist = calcDistance_long(current, to);
    double lat_dist = calcDistance_lat(current, to);
    strcpy(dir, "");
    if(long_dist > 5){
        if(current->longitude > to->longitude){
            strcpy(dir, "S");
            // dir = "S";
        }else{
            // dir = "N";
            strcpy(dir, "N");
        }
    }
    if(lat_dist > 5){
        if(current->latitude > to->latitude){
            // dir += "E";
            strcat(dir, "E");
        }else{
            // dir += "W";
            strcat(dir, "W");
        }
    }
    // if((current->longitude - to->longitude) > 0 && (current->latitude - to->latitude) > 0){
    //     dir = "NE";
    // }else if((current->longitude - to->longitude) > 0 && (current->latitude - to->latitude) < 0){
    //     dir = "NW";
    // }else if((current->longitude - to->longitude) < 0 && (current->latitude - to->latitude) > 0){
    //     dir = "SE";
    // }else if((current->longitude - to->longitude) < 0 && (current->latitude - to->latitude) < 0){
    //     dir = "SW";
    // }else if((current->longitude - to->longitude) > 0){
    //     dir = "N";
    // }else if((current->longitude - to->longitude) < 0){
    //     dir = "S";
    // }else if((current->latitude - to->latitude) > 0){
    //     dir = "E";
    // }else{
    //     dir = "W";
    // }

    return dir;
}

char* directionNext(struct GPS* current, struct Point* pt){
    return directionTo(current, pt->next->gps);
}

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
    if(dist < 5){
        map->index = map->index + 1;
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
        new_gps->latitudeDegrees = points[j][0];
        new_gps->longitudeDegrees = points[j][1];
        new_point->gps = new_gps;
        new_point->prev = prev;
        new_point->next = NULL;
        prev->next = new_point;
        prev = new_point;
    }
}