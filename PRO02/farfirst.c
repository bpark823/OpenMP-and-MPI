#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include "mat.h"
#include <math.h>

#define MAX_POINTS 2000

/* calculate the cost of a given set of center locations */
double center_cost (mat_type* set, int* centers, int n, int* argmax) {
    double cost = 0;

    vec_type row1, row2, row3;
    for (int i=0; i<set->rows;i++) {
	double min_dist_sq = DBL_MAX;
	for (int j=0;j< n;j++) {
            mat_get_row(set, &row1, i);
	    mat_get_row(set, &row2, centers[j]);
            double dist_sq = vec_dist_sq(&row1, &row2);
	    if (dist_sq < min_dist_sq) {
		min_dist_sq = dist_sq;
		
	    }
	}	        
	if (min_dist_sq > cost) {
	    cost = min_dist_sq;
	    centers[n] = i;
            }
   }
    
    return cost;
}

int main (int argc, char** argv) {

    mat_type set;
    int rows, cols;

    /* read the shape of the matrix from STDIN */
    if (scanf("%d %d",&rows,&cols) != 2) {
	printf ("error reading the shape of the matrix\n");
	return 1;
    }
    /* get k and m from command line */
    if (argc < 2) {
	printf ("Command usage : %s %s\n",argv[0],"k");
	return 1;
    }
    mat_malloc(&set, rows, cols);
    mat_read(&set);
    int k = atoi(argv[1]);
    /* check the cost of m random sets of k centers */
    int centers[k];
    int argmax[k];
    centers[0] = 0;
    argmax[0] = 0;
    vec_type nice;
    int optimal_centers[k];
    double optimal_cost = DBL_MAX;
    for (int j=1;j<k;j++) {
	double cost = center_cost(&set,centers, j, argmax);
        
	if(optimal_cost > cost) {
	    optimal_cost = cost;
	}
    }

    optimal_cost = center_cost(&set, centers, k,  argmax);

    /* print the approximate minimal cost for the k-center problem */
    printf ("# approximate optimal cost = %.2lf\n", optimal_cost);
 
    /* print an approx optimal solution to the k-center problem */
    printf ("# approx optimal centers :\n ");
    vec_type row1;
    for (int j=0;j<k;j++) {
            mat_get_row(&set, &row1, centers[j]);
           vec_print(&row1);
    }
    printf ("\n");

    return 0; 

}
