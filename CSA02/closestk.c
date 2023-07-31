#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#define MAX_ARRAY_SIZE 1000000
int main (int argc, char** argv) {
    if (argc < 3) {
        printf ("Command usage : %s %s %s\n",argv[0],"k", "center");
        return 1;
    }
    int k = atoi(argv[1]);
    double center = atof(argv[2]);
    double numbers[k];
    double number;
    int n = 0;
    int pos = 0;
    double max_sq;
    while (scanf("%lf", &number) == 1) {
	double distance_sq = (center - number) * (center - number);
	if(n < k) {
	    numbers[n] = number;
	        
	} else {
	    max_sq = DBL_MIN;
	    for(int i = 0; i < k; i++) {
		double dist_sq = (center - numbers[i]) * (center - numbers[i]);		 
		if(dist_sq > max_sq) {
		    max_sq = dist_sq;
		    pos = i;
		}	
	    }
	    if(distance_sq < (center - numbers[pos]) * (center - numbers[pos])) {
	        numbers[pos] = number;
	    }    
	    for(int i = 0; i < k - 1; i++) {
                for(int j = 0; j < k - i - 1; j++) {
                    if(abs(center - numbers[j]) > abs(center - numbers[j + 1])) {
                    double temp = numbers[j];
                    numbers[j] = numbers[j + 1];
                    numbers[j + 1] = temp;
                
            }   
        
                   }
        
                                }
	}
	
	n++;
    }
    for(int i = 0; i < k; i++) {
	printf("%lf\n", numbers[i]);
    }
}
