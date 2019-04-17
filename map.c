#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "map.h"

#define EXIT_FAILURE 1
#define KM_TO_FEET 3280.84
double distanceNext(struct GPS* current, struct Map* map){
	return calcDistance(current, map->next);
}
double distancePrev(struct GPS* current, struct Map* map){
	return calcDistance(current, map->prev);
}
char* directionTo(struct GPS* current, struct GPS* to){
	char* dir;
	// if((current->longitude - to->longitude) > 0)
	// 	dir = "N";
	// else	dir = "S";

	// if((current->latitude - to->latitude) > 0)
	// 	dir = strcat(dir,"E");
	// else	dir = strcat(dir,"W");
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

char* directionNext(struct GPS* current, struct Map* map){
	return directionTo(current, map->next);
}

char* nextCheck(struct GPS* current, struct Map* map){
	char* str;
	double rad;		//radians
	float degrees;
	str = directionTo(current, map->next);
	rad = atan2((current->longitude)-(map->next->longitude),
				(current->longitude)-(map->next->longitude));
	degrees = radDegrees(rad);
	char buf[10];
	ftoa(degrees,buf,3);
	strcat(buf, "degees ");
	return strcat(buf, str);
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

double distFromMap(struct GPS* current, struct Map* map){
	double n = sqrt(pow(calcDistance(current,map->next),2) + pow(calcDistance(current,map->next),2));
	double p = sqrt(pow(calcDistance(current,map->prev),2) + pow(calcDistance(current,map->prev),2)); 
	
	if(n < p) return n;
	else return p;
}

void updateNext(struct GPS* current, struct Map* map){
	if(!map->next) return; 
	double dist = calcDistance(current,map->next);
	if(dist < Range){
		map->index +=1;
		map->prev = map->next;
		map->next = &map->checkpoint[map->index];
		return;
	}
	else return;
	
}

void ftoa(float n, char *res, int afterpoint){
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}
void reverse(char *str, int len){
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}
int intToStr(int x, char str[], int d){
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

void init_points(struct Map* map_ptr){
    // FILE* fp;
    // char* line = NULL;
    // size_t len = 0;
    // int read;    
	
    // int points = 0;	

    // // read once to figure out how many points we need
    // fp = fopen("test.txt", "r");
    
    // if (fp == NULL)
    // {
    //     exit(EXIT_FAILURE);
    // }

    // while ((read = getline(&line, &len, fp)) != -1) 
    // {
    //     points += 1;
    // }

    // printf("points: %d \n", points);

    // fclose(fp);

    // // second read to store the points in an array
    // char* line_2 = NULL;
    // char delim[] = " ";

    // int i = 0;

    // float latitude_points[points];
    // float longitude_points[points];

    // fp = fopen("test.txt", "r");
    
    // if (fp == NULL)
    // {
    //     exit(EXIT_FAILURE);
    // }

    // while ((read = getline(&line_2, &len, fp)) != -1) 
    // {
    //     printf("Second read %d:\n", read);
    //     printf("%s", line_2);

    //     int initial_size = strlen(line_2);
    //     char *ptr = strtok(line_2, delim);

    //     // convert string to float and store 
    //     latitude_points[i] = atof(ptr);
    //     ptr = strtok(NULL, delim);
    //     longitude_points[i] = atof(ptr);

    //     i += 1;
    // }

    // fclose(fp);

    /*for(int j = 0; j < points; j++)
    {
        printf("latitude_points: %f \n", latitude_points[j]);
        printf("longitude_points: %f \n", longitude_points[j]);
    }*/
    int points = 6;
    float latitude_points[] = {34.020331, 34.020201, 34.020403, 34.020560,
        34.020458, 34.020282};
    float longitude_points[] = {-118.289703, -118.289425, -118.289213, 
        -118.289106, -118.288494};

    int j;
    for(j = 0; j < points; j++)
    {
        map_ptr->checkpoint[j].latitudeDegrees = latitude_points[j];
        map_ptr->checkpoint[j].longitudeDegrees = longitude_points[j];
    }
	map_ptr->prev = NULL;
	map_ptr->next = &map_ptr->checkpoint[0];
	map_ptr->index = 1;
}