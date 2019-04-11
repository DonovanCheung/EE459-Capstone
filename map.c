#include <stdio.h>
#include <math.h>
#include "map.h"

double distanceNext(struct GPS* current, struct Map* map){
	return calcDistance(current, map->next);
}
double distancePrev(struct GPS* current, struct Map* map){
	return calcDistance(current, map->prev);
}
char* directionTo(struct GPS* current, struct GPS* to){
	char* dir;
	if((current->longitude - to->longitude) > 0)
		dir = "N";
	else	dir = "S";

	if((current->latitude - to->latitude) > 0)
		dir = strcat(dir,"E");
	else	dir = strcat(dir,"W");

	return dir;
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
  double d = R * c;
  return d;
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
