#include "gps.h"

int main()
{
	struct GPS test_gps;

	char test[] = "$GPGGA,064951.000,2307.1256,N,12016.4438,E,1,8,0.95,39.9,M,17.8,M,,*65";
	
	// output the test string 
	printf("%s \n", test);
	
	parse(test, &test_gps);

	printf("lat (N/S): %c \n", test_gps.lat);
	printf("latitude: %f \n", test_gps.latitude);
	
	printf("lon (E/W): %c \n", test_gps.lon);
	printf("longitude: %f \n", test_gps.longitude);

	return 0;
}
