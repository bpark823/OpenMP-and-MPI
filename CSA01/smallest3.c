#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#define MAX_ARRAY_SIZE 1000
int main () {
    double smallest = DBL_MAX;
    double smallest2 = DBL_MAX;
    double smallest3 = DBL_MAX;
    double numbers[MAX_ARRAY_SIZE];
    double number;
    int n = 0;

    while (scanf("%lf",&number) == 1) {
	if(n < MAX_ARRAY_SIZE) {
		numbers[n++] = number;
	}
	if(number < smallest) {
		smallest3 = smallest2;
		smallest2 = smallest;
		smallest = number;
	}
        else if(number != smallest && number < smallest2) {
        	smallest3 = smallest2;
                smallest2 = number;
        }
        else if (number != smallest && number != smallest2 && number < smallest3) {
        	smallest3 = number;
        }
}
printf ("The smallest number is %lf\n", smallest); 
printf("The second smallest is %lf\n", smallest2);
printf("The third smallest is %lf\n", smallest3);
}
