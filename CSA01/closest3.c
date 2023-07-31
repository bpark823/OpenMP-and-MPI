#include <stdio.h>
#include <stdlib.h>
#include <float.h>

int main (int argc, char** argv) {
    if (argc < 2) {
        printf ("Command usage : %s %s\n",argv[0],"center");
        return 1;
    }
    double center = atof(argv[1]);
    double number;
    double closest = 0;
    double closest2 = 0;
    double closest3 = 0;
    double min_distance_sq = DBL_MAX;
    double min_distance_sq2 = DBL_MAX;
    double min_distance_sq3 = DBL_MAX;
    int num_read = 0;
    while (scanf("%lf",&number) == 1) {
        double distance_sq = (center-number)*(center-number);
	if(number == 40.000442) {
                printf("Good\n");
            }
        if (distance_sq < min_distance_sq) {
	    closest3 = closest2;
	    closest2 = closest;
            closest = number;
	
            min_distance_sq = distance_sq;
        }
	else if(distance_sq < min_distance_sq2) {
		closest3 = closest2;
		closest2 = number;
		min_distance_sq2 = distance_sq;
	}
	else if(distance_sq < min_distance_sq3) {
		closest3 = number;
		min_distance_sq3 = distance_sq;
	}

	num_read++;
    }
    if (num_read > 0) {
	printf ("The number closest to %lf is %lf\n",center,closest);
	printf("The number second closest to %lf is %lf\n", center, closest2);
	printf("The number third closest to %lf is %lf\n", center, closest3);
    }
    return 0;
}

