#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#define MAX_ARRAY_SIZE 1000000
int main (int argc, char** argv) {
    if (argc < 2) {
        printf ("Command usage : %s %s\n",argv[0],"k");
        return 1;
    }
    int k = atoi(argv[1]);
    double numbers[k];
    double number;
    int n = 0;
    int pos = 0;
    double max;
    while (scanf("%lf", &number) == 1) {
	if(n < k) {
	    numbers[n] = number;
	        
	} else {
	    max = DBL_MIN;
	    for(int i = 0; i < k; i++) {
		if(numbers[i] > max) {
		    max = numbers[i];
		    pos = i;
		}	
	    }
	    if(number < numbers[pos]) {
	        numbers[pos] = number;
	    }
    for(int i = 0; i < k - 1; i++) {
        for(int j = 0; j < k - i - 1; j++) {
            if(numbers[j] > numbers[j + 1]) {
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
