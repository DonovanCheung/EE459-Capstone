#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "map.h"
#include "lcd.h"
#define PI 3.141592654
#define EXIT_FAILURE 1
#define KM_TO_FEET 3280.84

double distanceNext(struct GPS* current, struct Point* pt){
    return calcDistance(current, pt->next->gps);
}
double distancePrev(struct GPS* current, struct Point* pt){
    return calcDistance(current, pt->prev->gps);
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
  double d = R * c * 1000; //distance in miles
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
  double d = R * c * 1000; //distance in miles
  return d;
}

char* directionTo(struct GPS* current, struct GPS* to){
    char* dir = "";
    double long_dist = calcDistance_long(current, to);
    double lat_dist = calcDistance_lat(current, to);

    if(to->latitude > current->latitude && long_dist < 10){
        dir = "N";
    }else if(to->latitude < current->latitude && long_dist < 10){
        dir = "S";
    }else if(to->longitude > current->longitude){
        dir = "W";
    }else{
        dir = "E";
    }
    // if(long_dist > 5 && lat_dist > 5){
    //     if(current->longitude > to->longitude && current->latitude > to->latitude){
    //       dir = "SE";
    //     }else if(current->longitude > to->longitude){
    //       dir = "NE";
    //     }else if(current->latitude > to->latitude){
    //       dir = "SW";
    //     }else{
    //       dir = "NW";
    //     }
    //     // if(current->longitude > to->longitude){
    //     //     strcpy(dir, "S");
    //     //     // dir = "S";
    //     // }else{
    //     //     // dir = "N";
    //     //     strcpy(dir, "N");
    //     // }
    // }else if(long_dist > 5){
    //   if(current->longitude > to->longitude){
    //     dir = "E";
    //   }else{
    //     dir = "W";
    //   }
    // }else if(lat_dist > 5){
    //   if(current->latitude > to->latitude){
    //     dir = "S";
    //   }else{
    //     dir = "N";
    //   }
    // }
    // if(lat_dist > 5){
    //     if(current->latitude > to->latitude){
    //         // dir += "E";
    //         strcat(dir, "E");
    //     }else{
    //         // dir += "W";
    //         strcat(dir, "W");
    //     }
    // }

    return dir;
}

char* directionNext(struct GPS* current, struct Point* pt){
    return directionTo(current, pt->next->gps);
}
char* directionPrev(struct GPS* current, struct Point* pt){
    return directionTo(current, pt->prev->gps);
}

double calcDistance(struct GPS* current, struct GPS* gps){
  int R = 6371; // Radius of the Earth
  double dLat = degRadians(gps->latitudeDegrees - current->latitudeDegrees);
  double dLon = degRadians(gps->longitudeDegrees - current->longitudeDegrees);
  double a = sin(dLat/2)*sin(dLat/2) + cos(degRadians(gps->latitudeDegrees)) *
    cos(degRadians(current->latitudeDegrees)) * sin(dLon/2) *
    sin(dLon/2);
  double c = 2 * atan2(sqrt(a), sqrt(1-a));
  double d = R * c * 1000; //distance in miles
  return d;
}
void updatePrev(struct GPS* current, struct Map* map){
    if(!(map->curr->prev)){
      lcd_yellowon();
      return;
    }
    double dist = calcDistance(current,map->curr->next->gps);
    if(dist < 15){
        map->index = map->index - 1;
        map->curr = map->curr->prev;
        lcd_yellowon();
        return;
    }
    else if (!(/*calcDistance(current, map->curr->gps) < 50 || */dist < 50)){
        lcd_redon();
    }
    else{
      lcd_redoff();
      lcd_yellowoff();
      return;
    }
}
void updateNext(struct GPS* current, struct Map* map){
    if(!(map->curr->next)){
      lcd_yellowon();  
      return;
    }
    double dist = calcDistance(current,map->curr->next->gps);
    if(dist < 15){
        map->index = map->index + 1;
        map->curr = map->curr->next;
        lcd_yellowon();
        return;
    }
    else if (!(/*calcDistance(current, map->curr->gps) < 50 || */dist < 50)){
        lcd_redon();
        lcd_moveto(2, 5);
        lcd_stringout("CAUTION");
        lcd_moveto(3, 2);
        lcd_stringout("Return to trail");
    }
    else{
      lcd_redoff();
      lcd_yellowoff();
      return;
    }

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
